#include "dump.h"

void* sdk::game::file::dump_args::filename = 0;
void* sdk::game::file::dump_args::data = 0;
void* sdk::game::file::dump_args::mapped = 0;
void* sdk::game::file::dump_args::base = 0;
uint32_t sdk::game::file::dump_args::return_val = 0;
sdk::game::func::t::t_EterPackGet sdk::game::file::dump_args::o_EterPackGet = 0;

void sdk::game::file::rmv_str(std::string& clean_file_name, char c)
{
	while (1)
	{
		auto i = clean_file_name.find(c);
		if (i == std::string::npos) break;
		clean_file_name.erase(0, i + 1);
	}
}
void sdk::game::file::gate()
{
	sdk::game::file::c_dump::Instance().work();
}

__declspec(naked) bool __stdcall f_EterPack_Get()
{
	__asm
	{
		push eax
		mov eax, dword ptr[esp + 0x8]
		mov sdk::game::file::dump_args::data, eax
		mov eax, dword ptr[esp + 0xC]
		mov sdk::game::file::dump_args::filename, eax
		mov eax, dword ptr[esp + 0x10]
		mov sdk::game::file::dump_args::mapped, eax
		mov eax, dword ptr[esp + 0x14]
		mov sdk::game::file::dump_args::base, eax
		pop eax

		pop sdk::game::file::dump_args::return_val		
		push return_address

		jmp[sdk::game::file::dump_args::o_EterPackGet]

	return_address:

		push sdk::game::file::dump_args::return_val

		push ebp
		mov ebp, esp
		push eax

		call sdk::game::file::gate

		pop eax
		mov esp, ebp
		pop ebp

		ret
	}
}

bool sdk::game::file::c_dump::dump()
{
	auto file_size = *(uint32_t*)(((uint32_t)sdk::game::file::dump_args::data) + sdk::game::pack_offsets::off_CMAPPED_FILE_SIZE);
	if (!file_size) return 0;
	this->file_index++;

	auto file_name = std::string((const char*)sdk::game::file::dump_args::filename);
	sdk::game::file::rmv_str(file_name, '\\');
	sdk::game::file::rmv_str(file_name, '/');

	for (auto &&a : file_name) if ((wchar_t)a > 128) a = '_';
	
	sdk::util::c_log::Instance().duo(XorStr("[ (filesize: %i) %s ]\n"),
		file_size,
		(const char*)sdk::game::file::dump_args::filename);

	byte* buf = (byte*)malloc(file_size);
	memcpy(buf, (void*)(*(uint32_t*)((uint32_t)sdk::game::file::dump_args::mapped)), file_size);

	//dir recreation
	auto path = std::string((const char*)sdk::game::file::dump_args::filename);
	path = main::c_ui::Instance().to_lower(path);
	if (strstr(path.c_str(), "d:/"))
	{
		auto i = path.find_first_of('/');
		path.erase(0, i + 1);

		sdk::util::c_log::Instance().duo("(A d:/) %s\n", path.c_str());

		auto cpy = path;
		//del file part
		auto b = cpy.find_last_of('/');
		cpy.erase(b, cpy.size());

		sdk::util::c_log::Instance().duo("(B d:/) %s\n", cpy.c_str());

		std::filesystem::create_directories(std::string("client_dump_hsh/").append(cpy));
		
		sdk::util::c_log::Instance().duo("(B d:/) %s\n", std::string(XorStr("client_dump_hsh/")).append(cpy).append("/").append(file_name).c_str());

		std::ofstream fb(std::string(XorStr("client_dump_hsh/")).append(cpy).append("/").append(file_name), std::ios::binary);
		for (auto a = 0; a < file_size; a++) fb << buf[a];
		fb.close();
		free(buf);
	}
	else if (!strstr(path.c_str(), "/") && !strstr(path.c_str(), "\\"))
	{
		sdk::util::c_log::Instance().duo("(direct) %s\n", path.c_str());

		//pathless?
		std::ofstream fb(std::string(XorStr("client_dump_hsh/")).append(std::to_string(this->file_index)).append(file_name), std::ios::binary);
		for (auto a = 0; a < file_size; a++) fb << buf[a];
		fb.close();
		free(buf);
	}
	else if (strstr(path.c_str(), "d:\\"))
	{
		auto i = path.find_first_of('\\');
		path.erase(0, i + 1);

		sdk::util::c_log::Instance().duo("(A d:\\) %s\n", path.c_str());

		auto cpy = path;
		//del file part
		auto b = cpy.find_last_of('\\');
		cpy.erase(b, cpy.size());

		sdk::util::c_log::Instance().duo("(B d:\\) %s\n", cpy.c_str());

		std::filesystem::create_directories(std::string("client_dump_hsh\\").append(cpy));
		
		sdk::util::c_log::Instance().duo("(B d:\\) %s\n", std::string(XorStr("client_dump_hsh\\")).append(cpy).append("\\").append(file_name).c_str());

		std::ofstream fb(std::string(XorStr("client_dump_hsh\\")).append(cpy).append("\\").append(file_name), std::ios::binary);
		for (auto a = 0; a < file_size; a++) fb << buf[a];
		fb.close();
		free(buf);
	}
	else if (!strstr(path.c_str(), "d:") && strstr(path.c_str(), "/"))
	{
		auto cpy = path;
		//del file part
		auto b = cpy.find_last_of('/');
		cpy.erase(b, cpy.size());

		sdk::util::c_log::Instance().duo("(A /) %s\n", cpy.c_str());

		std::filesystem::create_directories(std::string("client_dump_hsh/").append(cpy));
		
		sdk::util::c_log::Instance().duo("(A /) %s\n", std::string(XorStr("client_dump_hsh/")).append(cpy).append("/").append(file_name).c_str());

		std::ofstream fb(std::string(XorStr("client_dump_hsh/")).append(cpy).append("/").append(file_name), std::ios::binary);
		for (auto a = 0; a < file_size; a++) fb << buf[a];
		fb.close();
		free(buf);
	}
	else if (!strstr(path.c_str(), "d:") && strstr(path.c_str(), "\\"))
	{
		auto cpy = path;
		//del file part
		auto b = cpy.find_last_of('\\');
		cpy.erase(b, cpy.size());

		sdk::util::c_log::Instance().duo("(A \\) %s\n", cpy.c_str());

		std::filesystem::create_directories(std::string("client_dump_hsh\\").append(cpy));

		std::ofstream fb(std::string(XorStr("client_dump_hsh\\")).append(cpy).append("\\").append(file_name), std::ios::binary);
		for (auto a = 0; a < file_size; a++) fb << buf[a];
		fb.close();
		free(buf);
	}
	else sdk::util::c_log::Instance().duo("weird path %s\n", (const char*)sdk::game::file::dump_args::filename);
	
    return 1;
}

void sdk::game::file::c_dump::work()
{
	this->dump();
}

void sdk::game::file::c_dump::set_ctx(void* f, void* d, void* m, void* b)
{
	sdk::game::file::dump_args::filename = f;
	sdk::game::file::dump_args::data = d;
	sdk::game::file::dump_args::mapped = m;
	sdk::game::file::dump_args::base = b;
}

void sdk::game::file::c_dump::setup()
{
	MH_CreateHook((void*)sdk::game::pack_offsets::off_CEterPackManager_GET, (void*)f_EterPack_Get, (void**)&sdk::game::file::dump_args::o_EterPackGet);
	MH_EnableHook((void*)sdk::game::pack_offsets::off_CEterPackManager_GET);
}