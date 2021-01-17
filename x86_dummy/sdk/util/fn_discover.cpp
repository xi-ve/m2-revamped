#include "fn_discover.h"
using namespace std::chrono;
void sdk::util::c_fn_discover::setup()
{
	auto text_res = this->text_section();
	auto data_res = this->data_section();
	if (!text_res) { sdk::util::c_log::Instance().duo("[ text discovery failed! ]\n"); return; }
	if (!data_res) { sdk::util::c_log::Instance().duo("[ data discovery failed! ]\n"); return; }
	sdk::util::c_log::Instance().duo("[ c_fn_discover::setup completed ]\n");
}

bool sdk::util::c_fn_discover::is_ascii(const std::string& in)
{
	return !std::any_of(in.begin(), in.end(), [](char c)
	{
		return static_cast<unsigned char>(c) > 127;
	});
}

const char* sdk::util::c_fn_discover::get_exe_path()
{
	char* buffer = (char*)malloc(256);
	GetModuleFileNameA(0, buffer, 256);
	return buffer;
}

uint32_t sdk::util::c_fn_discover::get_crc()
{
	auto path = this->get_exe_path();
	return sdk::util::c_crc32::Instance().get_crc_raw(path);
}

void sdk::util::c_fn_discover::save_db()
{
	this->ofstream.open(this->file_name);
	for (auto a : this->fns)
	{
		auto jobj = nlohmann::json();
		jobj["address"] = a.address;
		jobj["strings"] = a.strings;
		this->ofstream << jobj << "\n" ;
	}
	this->ofstream.close();
}

void sdk::util::c_fn_discover::load_db()
{
	this->fstream.open(this->file_name);
	if (!this->fstream.is_open()) { sdk::util::c_log::Instance().duo("[ failed to open fn_discover DB ]\n"); return; }
	std::string buffer;
	while (std::getline(this->fstream, buffer))
	{
		auto strct_obj = sdk::util::json_fn_discover::s_info_entry();
		json::parse(buffer).get_to(strct_obj);
		this->fns.push_back(strct_obj);
	}
	sdk::util::c_log::Instance().duo("[ loaded %i dynamics objects ]\n", this->fns.size());
	this->fstream.close();
}

void sdk::util::worker_thread(s_mem* mem)
{
	auto base = GetModuleHandleA(0);
	auto text_section = sdk::util::c_mem::Instance().get_section(".text", base);
	auto rdata_section = sdk::util::c_mem::Instance().get_section(".rdata", base);
	auto max_text = (uintptr_t)base + text_section.first + text_section.second;
	auto max_rdata = (uintptr_t)base + rdata_section.first + rdata_section.second;
	sdk::util::c_log::Instance().print("[ starting worker for block %04x to %04x ]\n", mem->start, mem->end);
	for (auto a = mem->start; a < mem->end; a += 1)
	{
		if (!sdk::util::c_mem::Instance().is_valid((a + (uintptr_t)base), 0x4)) continue;
		auto byte_0 = *(byte*)((a + (uintptr_t)base));
		auto byte_1 = *(byte*)((a + (uintptr_t)base + 1));
		auto byte_2 = *(byte*)((a + (uintptr_t)base + 2));
		if (byte_0 == 0x55 && byte_1 == 0x8B && byte_2 == 0xEC)
		{
			auto fn_size = sdk::util::c_mem::Instance().find_size(((uintptr_t)base + a));
			//sdk::util::c_log::Instance().duo("[ fn: %04x, size: %04x ]\n", a, fn_size);
			if (!fn_size) continue;
			auto fn_pushes = sdk::util::c_disassembler::Instance().get_pushes((a + (uintptr_t)base), fn_size);
			if (fn_pushes.empty()) continue;
			//sdk::util::c_log::Instance().duo("[ fn: %04x, size: %04x, pushes: %i ]\n", a, fn_size, fn_pushes.size());
			for (auto b : fn_pushes)
			{
				if (b < (rdata_section.first + (uintptr_t)base) || b > max_rdata) continue;
				char cstring[256] = "\0";
				memcpy((void*)cstring, (void*)b, 256);
				if (!cstring || !sdk::util::c_fn_discover::Instance().is_ascii(cstring)) continue;
				mem->listing[(uintptr_t)base + a].push_back(cstring);
			}
			a += fn_size;
		}
	}
	mem->done = 1;
	sdk::util::c_log::Instance().duo("[ worker %04x to %04x has finished with %i results ]\n", mem->start, mem->end, mem->listing.size());
}

bool sdk::util::c_fn_discover::text_section()
{
	auto current_crc32 = this->get_crc();
	auto conf_crc32 = sdk::util::c_config::Instance().get_var("dynamics", "last_file_crc");
	sdk::util::c_log::Instance().duo("[ current file crc %04x ]\n", current_crc32);
	sdk::util::c_log::Instance().duo("[ cached  file crc %04x ]\n", std::stoul(conf_crc32->container));
	if (std::stoul(conf_crc32->container) != current_crc32)
	{		
		sdk::util::c_log::Instance().duo("[ new file detected, %04x to %04x ]\n[ running dynamics system, please wait ]\n", std::stoul(conf_crc32->container), current_crc32);
		auto base = GetModuleHandleA(0);
		auto text_section = sdk::util::c_mem::Instance().get_section(".text", base);
		auto rdata_section = sdk::util::c_mem::Instance().get_section(".rdata", base);
		auto per_block_size = (text_section.first + text_section.second) / 8;
		auto block_next = 0x800;
		std::vector<sdk::util::s_mem*> worker_data;
		for (auto a = 0; a < 8; a++)//fixme: error when using more than 1 thread!
		{
			auto p = new sdk::util::s_mem(block_next, block_next + per_block_size);
			worker_data.push_back(p);
			sdk::util::c_thread::Instance().spawn((LPTHREAD_START_ROUTINE)sdk::util::worker_thread, p);
			block_next += per_block_size;
		}
		while (1)
		{
			std::this_thread::sleep_for(1s);
			auto all_done = true;
			for (auto a : worker_data) if (!a->done) all_done = false;
			if (all_done) break;
		}
		for (auto a : worker_data)
		{
			for (auto b : a->listing) this->fns.push_back({ b.first,b.second });			
		}
		sdk::util::c_log::Instance().duo("[ found %i total dynamics ]\n", this->fns.size());
		this->save_db();
		conf_crc32->container = std::to_string(current_crc32);
		sdk::util::c_config::Instance().save();
	}
	else
	{
		sdk::util::c_log::Instance().duo("[ loading generated dynamics ]\n");
		this->load_db();
	}
	return 1;
}

bool sdk::util::c_fn_discover::data_section()
{
	return 1;
}

