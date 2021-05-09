#include "fn_discover.h"
using namespace std::chrono;
void sdk::util::c_fn_discover::setup()
{
	this->get_server();
	//control ConnectToAccountServer
retry_text:
	auto text_res = this->text_section();
	auto base = GetModuleHandleA(0);

	//check if we have our control pyfunc
	for (auto a : this->fns)
	{
		for (auto b : a.strings) if (strstr(b.c_str(), (XorStr("ID3DXMeshOptimize: Cannot do AttributeSort"))) || strstr(b.c_str(), XorStr("FrameSkip"))) goto text_done;//quick jumpout
	}
	//failed to find control pyfunc, we failed!
	if (this->text_run > sdk::util::c_mem::Instance().get_section_count(base))
	{
		sdk::util::c_log::Instance().duo(XorStr("[ text discovery failed! ]\n"));
		return;
	}
	else
	{
		this->text_run++;
		sdk::util::c_log::Instance().duo(XorStr("[ text discovery scanning section %i ]\n"), this->text_run);
		goto retry_text;
	}
text_done:
	sdk::util::c_log::Instance().duo(XorStr("[ text discovery OK! ]\n"));

	auto current_crc32 = this->get_crc();
	auto conf_crc32 = sdk::util::c_config::Instance().get_var(XorStr("dynamics"), XorStr("last_file_crc"));

	this->save_fn_db();
	conf_crc32->container = std::to_string(current_crc32);
	sdk::util::c_config::Instance().save();

	//

retry_data:
	auto data_res = this->data_section();

	//check if we have our control pyfunc
	for (auto a : this->fns_py)
	{
		for (auto b : a.strings) if (strstr(b.c_str(), (XorStr("ConnectToAccountServer")))) goto data_done;//quick jumpout
	}
	//failed to find control pyfunc, we failed!
	if (this->data_run > sdk::util::c_mem::Instance().get_section_count(base))
	{
		sdk::util::c_log::Instance().duo(XorStr("[ data discovery failed! ]\n"));
		return;
	}
	else
	{
		this->data_run++;
		sdk::util::c_log::Instance().duo(XorStr("[ data discovery scanning section %i ]\n"), this->data_run);
		goto retry_data;
	}
data_done:
	sdk::util::c_log::Instance().duo(XorStr("[ data discovery OK! ]\n"));
	this->save_py_fn_db();

	//

	sdk::util::c_log::Instance().duo(XorStr("[ c_fn_discover::setup completed ]\n"));
}

void sdk::util::c_fn_discover::get_server()
{
	char cur_exe[256];
	GetModuleFileNameA(0, cur_exe, 256);

	auto servers_known = std::vector<std::string>() =
	{
		"xaleas",
		"Celestial World 2.0",
		"Arithra2",
		"Kevra",
		"Anoria2",
		"SunshineMt2",
		"Realm2",
		"Tamidia",
		"Aeldra",
		"Akeno2",
		"Erco",
		"Ekstasia",
		"Yumano3",
		"Zeros2",
		"Kuragari2",
		"Hybrid2",
		"Calliope2",
		"Rosaria2",
		"Kuba2",
		"Metin2 SG"
	};

	auto cur_server = std::string("generic-server");
	for (auto a : servers_known) if (strstr(cur_exe, a.c_str())) { cur_server = a; break; }
	this->server_name = cur_server;

	sdk::util::c_log::Instance().duo("[ server detected as: %s ]\n", this->server_name.c_str());

	if (strstr(cur_server.c_str(), "Erco") || strstr(cur_server.c_str(), "xaleas") || strstr(cur_server.c_str(), "Zeros2") || strstr(cur_server.c_str(), "Kuragari2")) sdk::game::chr::c_char::Instance().use_alt_mode_list = 1;
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

void sdk::util::c_fn_discover::save_fn_db()
{
	auto base = (uintptr_t)GetModuleHandleA(0);
	this->ofstream.open(this->file_name);
	for (auto a : this->fns)
	{
		auto jobj = nlohmann::json();
		jobj["address"] = a.address - base;
		jobj["strings"] = a.strings;
		this->ofstream << jobj << "\n";
	}
	this->ofstream.close();
}

void sdk::util::c_fn_discover::save_py_fn_db()
{
	auto base = (uintptr_t)GetModuleHandleA(0);
	this->ofstream.open(this->py_file_name);
	for (auto a : this->fns_py)
	{
		auto jobj = nlohmann::json();
		jobj["address"] = a.address - base;
		jobj["strings"] = a.strings;
		this->ofstream << jobj << "\n";
	}
	this->ofstream.close();
}

void sdk::util::c_fn_discover::load_fn_db()
{
	auto base = (uintptr_t)GetModuleHandleA(0);
	this->fstream.open(this->file_name);
	if (!this->fstream.is_open()) { sdk::util::c_log::Instance().duo(XorStr("[ failed to open fn_discover DB ]\n")); return; }
	std::string buffer;
	while (std::getline(this->fstream, buffer))
	{
		auto strct_obj = sdk::util::json_fn_discover::s_info_entry();
		json::parse(buffer).get_to(strct_obj);
		strct_obj.address += base;
		this->fns.push_back(strct_obj);
	}
	sdk::util::c_log::Instance().duo(XorStr("[ loaded %i dynamics objects ]\n"), this->fns.size());
	this->fstream.close();
}

void sdk::util::c_fn_discover::load_py_fn_db()
{
	auto base = (uintptr_t)GetModuleHandleA(0);
	this->fstream.open(this->py_file_name);
	if (!this->fstream.is_open()) { sdk::util::c_log::Instance().duo(XorStr("[ failed to open py_fn_discover DB ]\n")); return; }
	std::string buffer;
	while (std::getline(this->fstream, buffer))
	{
		auto strct_obj = sdk::util::json_fn_discover::s_info_entry();
		json::parse(buffer).get_to(strct_obj);
		strct_obj.address += base;
		this->fns_py.push_back(strct_obj);
	}
	sdk::util::c_log::Instance().duo(XorStr("[ loaded %i py dynamics objects ]\n"), this->fns_py.size());
	this->fstream.close();
}

uint32_t sdk::util::c_fn_discover::get_fn_py(const char* fn_name)
{
	for (auto a : this->fns_py) if (strstr(a.strings.front().c_str(), fn_name)) return a.address;
	sdk::util::c_log::Instance().duo(XorStr("[ failed to get py fn %s ]\n"), fn_name);
	return 0;
}

bool sdk::util::c_fn_discover::is_python_fn(uint32_t address)
{
	auto pbase = GetModuleHandleA(XorStr("python27.dll"));
	if (!pbase) pbase = GetModuleHandleA(XorStr("python22.dll"));
	if (!pbase) 
	{ 
		return 0;
	}

	MODULEINFO inf;
	K32GetModuleInformation(GetCurrentProcess(), pbase, &inf, sizeof(inf));

	auto pmax = (uintptr_t)pbase + inf.SizeOfImage;

	if (address > (uintptr_t)pbase && address < pmax) return 1;
	return 0;
}

bool sdk::util::c_fn_discover::singletons()
{
	auto base = GetModuleHandleA(0);
	auto text_section = sdk::util::c_mem::Instance().get_section(XorStr(".text"), base);
	auto rdata_section = sdk::util::c_mem::Instance().get_section(XorStr(".rdata"), base);
	auto max_text = (uintptr_t)base + text_section.first + text_section.second;
	auto max_rdata = (uintptr_t)base + rdata_section.first + rdata_section.second;

	sdk::util::c_log::Instance().duo(XorStr("[ gathering singletons ]\n"));

	for (auto&& a : this->offs_singletons)
	{
		sdk::util::c_log::Instance().duo(XorStr("[ potential singleton fn: %04x ]\n"), a.address);
	}

	sdk::util::c_log::Instance().duo(XorStr("[ singletons completed ]\n"));
	return 1;
}

uint32_t sdk::util::c_fn_discover::get_fn(const char* fn_str_ref)
{
	for (auto a : this->fns) for (auto b : a.strings) if (strstr(b.c_str(), fn_str_ref)) return a.address;	
	sdk::util::c_log::Instance().duo(XorStr("[ failed to find reference for %s ]\n"), fn_str_ref);
	return 0;
}
sdk::util::t_addrs sdk::util::c_fn_discover::get_adr_str(const char* fn_name, int max_strings)
{
	auto l = sdk::util::t_addrs();
	for (auto a : this->fns) if (a.strings.size() == max_strings) for (auto b : a.strings) if (strstr(b.c_str(), fn_name)) l.push_back(a.address);
	return l;
}

sdk::util::t_addrs sdk::util::c_fn_discover::get_adr_str(const char* ref)
{
	auto l = sdk::util::t_addrs();
	for (auto a : this->fns) for (auto b : a.strings) if (strstr(b.c_str(), ref)) l.push_back(a.address);
	return l;
}

uint32_t sdk::util::c_fn_discover::discover_fn(uint32_t origin, size_t approx_size_min, size_t approx_size_max, size_t approx_calls/*min cnt*/, size_t approx_off_movs/*min cnt*/, bool no_calls_inside, bool no_off_push_inside, bool skip_py_exports, bool shoul_reverse_calls, bool should_include_jmp, uint32_t must_be_above, uint32_t size_fn)
{
	if (!size_fn) size_fn = sdk::util::c_mem::Instance().find_size(origin);
	if (!size_fn) return 0;
	auto fns_in_origin = sdk::util::c_disassembler::Instance().get_calls(origin, size_fn, 0, skip_py_exports);
	if (!fns_in_origin.size()) return 0;
	if (shoul_reverse_calls) std::reverse(fns_in_origin.begin(), fns_in_origin.end());
	for (auto&& a : fns_in_origin)
	{		
		if (!a) continue;
		auto inside_fn_size = sdk::util::c_mem::Instance().find_size(a);
		if (!inside_fn_size || inside_fn_size > approx_size_max || inside_fn_size < approx_size_min) continue;
		auto calls_inside = sdk::util::t_asm_res();
		if (should_include_jmp)
		{
			calls_inside = sdk::util::c_disassembler::Instance().get_calls(a, inside_fn_size, 0, skip_py_exports);
			
			auto list_jmps = sdk::util::c_disassembler::Instance().get_jumps(a, size_fn, 0);
			for (auto a : list_jmps)
			{
				sdk::util::c_log::Instance().duo(XorStr("[ jmp: %04x ]\n"), a);
				calls_inside.push_back(a);
			}
		}
		else
		{
			calls_inside = sdk::util::c_disassembler::Instance().get_calls(a, inside_fn_size, 0, skip_py_exports);
		}
		sdk::util::c_log::Instance().duo(XorStr("[ scanning: %04x => %04x, size: %04x, calls: %i ]\n"), origin, a, inside_fn_size, calls_inside.size());
		if (no_off_push_inside)
		{
			auto movs_in = sdk::util::c_disassembler::Instance().get_custom(a, inside_fn_size, 0x3000, 0xFFFFFFF, { "mov" });
			if (movs_in.size()) continue;
		}
		if (must_be_above)
		{
			auto offs = sdk::util::c_disassembler::Instance().get_custom(a, inside_fn_size, 0, 0, { "mov", "add", "push", "lea" });
			auto should_skip = false;
			for (auto b : offs)
			{
				sdk::util::c_log::Instance().duo(XorStr("[ must_be_above: %04x -> %04x ]\n"), b, must_be_above);

				if (b < must_be_above) { should_skip = true; break; }
			}
			if (should_skip) continue;
		}
		if (no_calls_inside) if (!calls_inside.empty()) continue;
		if (approx_calls) if (calls_inside.size() < approx_calls) continue;
		if (approx_off_movs)
		{
			auto off_pushes_inside = sdk::util::c_disassembler::Instance().get_custom(a, inside_fn_size, 0, 0, { "mov", "push" });
			//sdk::util::c_log::Instance().duo("[ scanning: %04x => %04x, size: %04x, movs: %i ]\n", origin, a, inside_fn_size, off_pushes_inside.size());
			if (off_pushes_inside.size() < approx_off_movs) continue;
		}
		//sdk::util::c_log::Instance().duo("[ found res for: %04x => %04x, size: %04x, calls: %i ]\n", origin, a, inside_fn_size, calls_inside.size());
		return a;
	}
	return 0;
}

void sdk::util::c_fn_discover::add_singleton(uint32_t address)
{
	for (auto a : this->offs_singletons) if (a.address == address) return;
	this->offs_singletons.push_back({ address, {} });
}

void sdk::util::worker_thread(s_mem* mem)
{
	MEMORY_BASIC_INFORMATION m_info;
	auto base = GetModuleHandleA(0);

	auto text_section = sdk::util::c_mem::Instance().get_section(XorStr(".text"), base);
	VirtualQueryEx(GetCurrentProcess(), (void*)(mem->start + (uintptr_t)base), &m_info, sizeof(m_info));

	if (mem->section_id != 0)
	{
		text_section = sdk::util::c_mem::Instance().get_section_idx(mem->section_id, base);
		m_info.RegionSize = text_section.second;
	}

	if (m_info.State != MEM_COMMIT) { mem->done = 1; return; }
	if (m_info.Protect & PAGE_NOACCESS) { mem->done = 1; return; }

	sdk::util::c_log::Instance().duo(XorStr("[ (%04x) mem page: %04x, %04x ]\n"),
		mem->start,
		(uint32_t)m_info.AllocationBase + mem->start,
		((uint32_t)m_info.AllocationBase + mem->start + m_info.RegionSize)
	);

	auto fns = std::vector<std::pair<uint32_t, uint8_t*>>();

	for (auto a = (uint32_t)m_info.AllocationBase + mem->start; a < ((uint32_t)m_info.AllocationBase + mem->start + m_info.RegionSize); a++)
	{
		uint8_t b[0x10] = { };
		memcpy(b, (void*)a, 0x10);
		if (!b) continue;
		if (b[0] == 0x55 && b[1] == 0x8B && b[2] == 0xEC)
		{
			fns.push_back({ a, b });
		}
	}

	int count_had_pushes = 0;

	for (auto&& a : fns)
	{
		auto size_of_function = sdk::util::c_mem::Instance().find_size(a.first);
		if (!size_of_function) continue;
		auto asm_fn = sdk::util::c_disassembler::Instance().get_pushes(a.first, size_of_function, 0xFFF, mem->section_id);
		if (asm_fn.empty()) continue;

		count_had_pushes++;
		for (auto b : asm_fn)
		{
			if (!b || IsBadCodePtr((FARPROC)b)) continue;
			char cstring[0x256] = { };
			if (!ReadProcessMemory(GetCurrentProcess(), (void*)b, cstring, 0x128, nullptr)) continue;
			if (strstr(cstring, XorStr("FrameSkip"))) strcpy(cstring, "FrameSkip");			
			if (!cstring) continue;
			if (!sdk::util::c_fn_discover::Instance().is_ascii(cstring))
			{
				continue;
			}
			mem->listing[a.first].push_back(cstring);
		}
	}

	mem->done = 1;
	sdk::util::c_log::Instance().duo(XorStr("[ worker %04x to %04x has finished with %i fns and %i fns with strings (%i) ]\n"), mem->start, mem->end, fns.size(), mem->listing.size(), count_had_pushes);
}

int sdk::util::c_fn_discover::text_section()
{
	auto current_crc32 = this->get_crc();
	auto conf_crc32 = sdk::util::c_config::Instance().get_var(XorStr("dynamics"), XorStr("last_file_crc"));
	if (std::stoul(conf_crc32->container) != current_crc32)
	{
		sdk::util::c_log::Instance().duo(XorStr("[ new file detected, %04x to %04x ]\n[ running dynamics system, please wait ]\n"), std::stoul(conf_crc32->container), current_crc32);
	doit:
		DeleteFileA(XorStr("M2++_PY_DYNAMICS.DB"));
		if (this->text_run == 0)
		{
			this->fns.clear();

			auto base = GetModuleHandleA(0);
			auto text_section = sdk::util::c_mem::Instance().get_section(XorStr(".text"), base);
			auto per_block_size = (text_section.first + text_section.second);
			auto block_next = 0x1000;
			std::vector<sdk::util::s_mem*> worker_data;

			auto p = new sdk::util::s_mem(block_next, block_next + per_block_size);
			p->section_id = this->text_run;
			worker_data.push_back(p);
			sdk::util::worker_thread(p);

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
			sdk::util::c_log::Instance().duo(XorStr("[ found %i total dynamics ]\n"), this->fns.size());
			if (!this->fns.size()) return 0;
			return 1;
		}
		else
		{
			this->fns.clear();

			auto base = GetModuleHandleA(0);
			auto text_section = sdk::util::c_mem::Instance().get_section_idx(this->text_run, base);
			auto per_block_size = (text_section.first + text_section.second);
			auto block_next = 0x1000;
			std::vector<sdk::util::s_mem*> worker_data;

			auto p = new sdk::util::s_mem(block_next, block_next + per_block_size);
			p->section_id = this->text_run;

			worker_data.push_back(p);
			sdk::util::worker_thread(p);

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
			sdk::util::c_log::Instance().duo(XorStr("[ found %i total dynamics ]\n"), this->fns.size());
			if (!this->fns.size()) return 0;
		}
	}
	else
	{
		sdk::util::c_log::Instance().duo(XorStr("[ loading generated dynamics ]\n"));
		this->load_fn_db();
		if (this->fns.size() < 2) goto doit;
	}
	return 1;
}

int sdk::util::c_fn_discover::data_section()
{
	this->fstream.open(this->py_file_name);
	if (this->fstream.is_open())
	{
		this->fstream.close();
		this->load_py_fn_db();
		if (!this->fns_py.size()) goto yes;
		return 1;
	}
yes:
	this->fstream.close();

	sdk::util::c_log::Instance().duo(XorStr("[ py dynamics system started ]\n"));

	if (this->data_run == 0)
	{
		auto base = GetModuleHandleA(0);
		auto data_section = sdk::util::c_mem::Instance().get_section(XorStr(".data"), base);
		auto text_section = sdk::util::c_mem::Instance().get_section(XorStr(".text"), base);

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
			s_string_container*		str_ptr = 0;
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
		
		sdk::util::c_log::Instance().duo(XorStr("[ py dynamics system completed with %i results ]\n"), this->fns_py.size());
	}
	else
	{
		this->fns_py.clear();
		
		auto base = GetModuleHandleA(0);
		auto data_section = sdk::util::c_mem::Instance().get_section_idx(this->data_run, base);

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
			s_string_container*		str_ptr = 0;
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

		sdk::util::c_log::Instance().duo(XorStr("[ py dynamics system completed with %i results ]\n"), this->fns_py.size());
	}
	
	return 1;
}

