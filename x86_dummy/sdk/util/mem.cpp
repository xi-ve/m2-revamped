#include "mem.h"
void sdk::util::c_mem::setup()
{
	sdk::util::c_log::Instance().duo(XorStr("[ c_mem::setup completed ]\n"));
}

bool sdk::util::c_mem::is_valid(uint32_t adr, size_t field_mem_size)
{
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	if (VirtualQuery((void*)adr, &mbi, sizeof(decltype(mbi))) == 0) return false;
	if (mbi.State != MEM_COMMIT) return false;
	if (mbi.Protect & PAGE_NOACCESS) return false;
	const std::vector<DWORD> allowed({
		PAGE_READONLY,
		PAGE_READWRITE,
		PAGE_WRITECOPY,
		PAGE_EXECUTE_READ,
		PAGE_EXECUTE_READWRITE,
		PAGE_EXECUTE_WRITECOPY
									 });
	if (std::find(allowed.begin(), allowed.end(), mbi.Protect) == allowed.end())  return false;
	size_t blockBytesPostPtr = mbi.RegionSize - (adr - (uint32_t)mbi.AllocationBase);
	if (blockBytesPostPtr < field_mem_size)  return this->is_valid(adr + blockBytesPostPtr, field_mem_size - blockBytesPostPtr);
	return true;
}

sdk::util::t_size sdk::util::c_mem::get_section(const char* section, HMODULE base_module)
{
	auto dos_header = (IMAGE_DOS_HEADER*)(base_module);
	auto nt_headers = (IMAGE_NT_HEADERS*)((BYTE*)dos_header + dos_header->e_lfanew);
	auto image_section = (PIMAGE_SECTION_HEADER)(nt_headers + 1);
	for (auto a = 0; a < nt_headers->FileHeader.NumberOfSections; a++)
	{
		char string[(sizeof image_section[a].Name) + 1];
		memcpy(string, image_section[a].Name, sizeof image_section[a].Name);
		string[sizeof image_section[a].Name] = 0;
		if (strstr(string, section)) return { image_section[a].VirtualAddress, image_section[a].SizeOfRawData };
	}
	return { 0,0 };
}

uint32_t sdk::util::c_mem::find_pattern(uint32_t start, const char* sig)
{
	static auto patternToByte = [](const char* pattern)
	{
		auto       bytes = std::vector<int>{};
		const auto start = const_cast<char*>(pattern);
		const auto end = const_cast<char*>(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current)
		{
			if (*current == '?')
			{
				++current;
				if (*current == '?')
					++current;
				bytes.push_back(-1);
			}
			else { bytes.push_back(strtoul(current, &current, 16)); }
		}
		return bytes;
	};

	const auto dosHeader = (PIMAGE_DOS_HEADER)(uintptr_t)GetModuleHandleA(0);
	const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)(uintptr_t)GetModuleHandleA(0) + dosHeader->e_lfanew);

	const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
	auto       patternBytes = patternToByte(sig);
	const auto scanBytes = reinterpret_cast<std::uint8_t*>((uintptr_t)GetModuleHandleA(0));

	const auto s = patternBytes.size();
	const auto d = patternBytes.data();

	if (start) start -= (uintptr_t)GetModuleHandleA(0);

	for (auto i = start; i < sizeOfImage - s; ++i)
	{
		bool found = true;
		for (auto j = 0ul; j < s; ++j)
		{
			if (scanBytes[i + j] != d[j] && d[j] != -1)
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			auto res = reinterpret_cast<uintptr_t>(&scanBytes[i]);
			return res;
		}
	}
	return NULL;
}

uint32_t sdk::util::c_mem::find_pattern_directed(uintptr_t moduleAdress, const char* signature)
{
	static auto patternToByte = [](const char* pattern)
	{
		auto       bytes = std::vector<int>{};
		const auto start = const_cast<char*>(pattern);
		const auto end = const_cast<char*>(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current)
		{
			if (*current == '?')
			{
				++current;
				if (*current == '?')
					++current;
				bytes.push_back(-1);
			}
			else { bytes.push_back(strtoul(current, &current, 16)); }
		}
		return bytes;
	};

	const auto dosHeader = (PIMAGE_DOS_HEADER)moduleAdress;
	const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)moduleAdress + dosHeader->e_lfanew);

	const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
	auto       patternBytes = patternToByte(signature);
	const auto scanBytes = reinterpret_cast<std::uint8_t*>(moduleAdress);

	const auto s = patternBytes.size();
	const auto d = patternBytes.data();

	for (auto i = 0ul; i < sizeOfImage - s; ++i)
	{
		bool found = true;
		for (auto j = 0ul; j < s; ++j)
		{
			if (scanBytes[i + j] != d[j] && d[j] != -1)
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			auto res = reinterpret_cast<uintptr_t>(&scanBytes[i]);
			//printf("found address %04x @ %01x,%01x,%01x,%01x\n", res, *(BYTE*)(res), *(BYTE*)(res+1), *(BYTE*)(res+2), *(BYTE*)(res+3));
			return res;
		}
	}
	return NULL;
}

size_t sdk::util::c_mem::find_size(uint32_t address)
{
	for (auto a = address; a < address + 0x1256; a += 1)
	{
		auto cur = address + a;
		uint8_t* b = (uint8_t*)malloc(10);
		if (!b) return 0;
		memcpy(b, (void*)a, 8);
		if (!b) return 0;
		if (b[0] == 0xc3 && b[1] == 0x55) return a - address;
		if (b[0] == 0xc3 && b[1] == 0xcc) return a - address;
		if (b[0] == 0xc2 && b[1] == 0x04) return a - address;
		if (b[0] == 0xc2 && b[3] == 0xCC && b[4] == 0xCC) return a - address;
		if (b[0] == 0xc2 && b[2] == 0x00 && b[5] == 0xCC) return a;
		if (b[0] == 0xe9 && b[5] == 0x55) return a - address;
		if (b[0] == 0xc3 && b[1] == 0x55) return a - address;
	}
	return 0;
}

