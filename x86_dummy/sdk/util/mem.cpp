#include "mem.h"
void sdk::util::c_mem::setup()
{
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
        char string[(sizeof image_section[a].Name)+1];
        memcpy(string, image_section[a].Name, sizeof image_section[a].Name);
        string[sizeof image_section[a].Name] = 0;
        if (strstr(string, section)) return { image_section[a].VirtualAddress, image_section[a].SizeOfRawData };
    }
    return { 0,0 };
}

size_t sdk::util::c_mem::find_size(uint32_t address)
{
    if (!this->is_valid(address, 0x1256)) return 0;
    for (auto a = 0; a < 0x1256; a += 1)
    {
        auto cur = address + a;
        auto b = *(byte*)(cur);
        auto b2 = *(byte*)(cur+1);
        if (b == 0xc3 && b2 == 0x55) return a - address;
        if (b == 0xc3 && b2 == 0xcc) return a - address;
        if (b == 0xc2 && b2 == 0x04) return a - address;
        if (b == 0xc2 && *(uint8_t*)(a + 3) == 0xCC && *(uint8_t*)(a + 4) == 0xCC) return a - address;
        if (b == 0xe9 && *(uint8_t*)(a + 5) == 0x55) return a - address;
        if (b == 0xc3 && b2 == 0x55) return a - address;
    }
    return 0;
}

