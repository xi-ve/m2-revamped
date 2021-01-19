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
	if (!buffer) return "";
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
		this->ofstream << jobj << "\n";
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

	MEMORY_BASIC_INFORMATION m_info;
	auto page_size = VirtualQueryEx(GetCurrentProcess(), (void*)(mem->start + (uintptr_t)base), &m_info, sizeof(m_info));

	if (m_info.State != MEM_COMMIT) { mem->done = 1; return; }
	if (m_info.Protect & PAGE_NOACCESS) { mem->done = 1; return; }

	sdk::util::c_log::Instance().duo("[ (%04x) mem page: %04x, %04x ]\n", mem->start, (uint32_t)m_info.AllocationBase + mem->start, ((uint32_t)m_info.AllocationBase + mem->start + m_info.RegionSize));

	auto fns = std::vector<std::pair<uint32_t, uint8_t*>>();

	for (auto a = (uint32_t)m_info.AllocationBase + mem->start; a < ((uint32_t)m_info.AllocationBase + mem->start + m_info.RegionSize); a++)
	{
		uint8_t b[0x10] = { };
		memcpy(b, (void*)a, 0x10);
		if (!b) continue;
		if (b[0] == 0x55 && b[1] == 0x8B && b[2] == 0xEC) fns.push_back({ a, b });
	}

	for (auto&& a : fns)
	{
		auto size_of_function = sdk::util::c_mem::Instance().find_size(a.first);
		if (!size_of_function) continue;
		auto asm_fn = sdk::util::c_disassembler::Instance().get_pushes(a.first, size_of_function, (uint32_t)((uint32_t)m_info.AllocationBase + mem->start));
		if (!asm_fn.size()) continue;
		for (auto b : asm_fn)
		{
			if (!b || IsBadCodePtr((FARPROC)b)) continue;
			char cstring[0x256] = { };
			if (!ReadProcessMemory(GetCurrentProcess(), (void*)b, cstring, 0x128, nullptr)) continue;
			if (!cstring || !sdk::util::c_fn_discover::Instance().is_ascii(cstring)) continue;
			mem->listing[a.first].push_back(cstring);
		}
	}

	mem->done = 1;
	sdk::util::c_log::Instance().duo("[ worker %04x to %04x has finished with %i fns and %i fns with size ]\n", mem->start, mem->end, fns.size(), mem->listing.size());
}

bool sdk::util::c_fn_discover::text_section()
{
	auto current_crc32 = this->get_crc();
	auto conf_crc32 = sdk::util::c_config::Instance().get_var("dynamics", "last_file_crc");
	if (std::stoul(conf_crc32->container) != current_crc32)
	{
		sdk::util::c_log::Instance().duo("[ new file detected, %04x to %04x ]\n[ running dynamics system, please wait ]\n", std::stoul(conf_crc32->container), current_crc32);
doit:
		auto base = GetModuleHandleA(0);
		auto text_section = sdk::util::c_mem::Instance().get_section(".text", base);
		auto rdata_section = sdk::util::c_mem::Instance().get_section(".rdata", base);
		auto per_block_size = (text_section.first + text_section.second);
		auto block_next = 0x1000;
		std::vector<sdk::util::s_mem*> worker_data;
		for (auto a = 0; a < 1; a++)
		{
			auto p = new sdk::util::s_mem(block_next, block_next + per_block_size);
			worker_data.push_back(p);
			sdk::util::c_thread::Instance().spawn((LPTHREAD_START_ROUTINE)sdk::util::worker_thread, p);
			block_next += per_block_size;
		}
		while (1)
		{
			auto all_done = true;
			for (auto a : worker_data) if (!a->done) all_done = false;
			if (all_done) break;
		}
		for (auto a : worker_data)
		{
			for (auto b : a->listing) this->fns.push_back({ b.first,b.second });
		}
		sdk::util::c_log::Instance().duo("[ found %i total dynamics ]\n", this->fns.size());
		if (!this->fns.size()) return 0;
		this->save_db();
		conf_crc32->container = std::to_string(current_crc32);
		sdk::util::c_config::Instance().save();
	}
	else
	{
		sdk::util::c_log::Instance().duo("[ loading generated dynamics ]\n");
		this->load_db();
		if (this->fns.size() < 2) goto doit;
	}
	return 1;
}

bool sdk::util::c_fn_discover::data_section()
{
	sdk::util::c_log::Instance().duo("[ py dynamics system started ]\n");

	auto base = GetModuleHandleA(0);
	auto data_section = sdk::util::c_mem::Instance().get_section(".data", base);
	auto text_section = sdk::util::c_mem::Instance().get_section(".text", base);

	struct s_string_container
	{
		char string[32] = "";
	};
	struct s_function_container
	{
		uint32_t function = 0;
	};
	struct s_register
	{
		s_string_container* str_ptr = 0;
		uint32_t				fnc_ptr = 0;
		uint8_t					validator = 0;
	};

	for (auto a = data_section.first; a < data_section.first + data_section.second; a += 1)
	{
		auto addr = a + (uintptr_t)base;
		if (!sdk::util::c_mem::Instance().is_valid(addr, sizeof(s_register))) continue;
		auto reg = s_register();
		if (!ReadProcessMemory(GetCurrentProcess(), (void*)addr, &reg, sizeof(s_register), nullptr)) continue;
		if (reg.validator != 1) continue;
		if (IsBadCodePtr((FARPROC)reg.fnc_ptr) || !reg.fnc_ptr) continue;
		if (IsBadCodePtr((FARPROC)reg.str_ptr) || !reg.str_ptr) continue;
		if (!reg.str_ptr->string || strlen(reg.str_ptr->string) < 4 || !sdk::util::c_fn_discover::Instance().is_ascii(reg.str_ptr->string) || strstr(reg.str_ptr->string, ".")) continue;
		this->fns_py.push_back({ reg.fnc_ptr, { reg.str_ptr->string } });
	}

	if (!this->fns_py.size()) return 0;
	sdk::util::c_log::Instance().duo("[ py dynamics system completed with %i results ]\n", this->fns_py.size());

	if (this->should_gen_fn_list)
	{
		this->ofstream.open("M2++_PY_FN_DUMP.DB");
		for (auto&& a : this->fns_py)
		{			
			auto calls = sdk::util::c_disassembler::Instance().get_calls(a.address, 0, text_section.first + (uint32_t)base);
			auto pushes = sdk::util::c_disassembler::Instance().get_pushes(a.address, 0, 0x10);
			auto offsets = sdk::util::c_disassembler::Instance().get_custom(a.address, 0, text_section.first + (uint32_t)base, text_section.first + text_section.second + (uint32_t)base, { "push" });

			this->ofstream << sdk::util::c_log::Instance().string("[ function: %04x, python name: %s ]\n", a.address, a.strings[0].c_str());

			if (calls.empty()) this->ofstream << "[ no calls ]\n\n";
			else for (auto b : calls) this->ofstream << sdk::util::c_log::Instance().string("[ call to: %04x ]\n", b);
			
			if (pushes.empty()) this->ofstream << "[ no pushes ]\n\n";
			else for (auto b : pushes) this->ofstream << sdk::util::c_log::Instance().string("[ push: %04x ]\n", b);

			if (offsets.empty()) this->ofstream << "[ no offsets ]\n\n";
			else for (auto b : offsets) this->ofstream << sdk::util::c_log::Instance().string("[ offset: %04x ]\n", b);

			this->ofstream << "\n";			
		}
		this->ofstream.close();
	}

	return 1;
}

