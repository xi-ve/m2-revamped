#include "crc32.h"

uint32_t sdk::util::c_crc32::get_crc_raw(const char* path)
{
    auto fhandle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (fhandle == INVALID_HANDLE_VALUE) return 0;
    auto file_size = GetFileSize(fhandle, 0);
	auto crc32 = uint32_t(0);
    SYSTEM_INFO sinfo;
    GetSystemInfo(&sinfo);
    auto alloc_gran = sinfo.dwAllocationGranularity;
    auto map_start = (0 / alloc_gran) * alloc_gran;
    auto view_size = (0 % alloc_gran) + map_start;
    auto file_mapping = CreateFileMapping(fhandle, 0, PAGE_READONLY, 0, map_start, 0);
    if (file_mapping)
    {
        auto map_data = MapViewOfFile(file_mapping, FILE_MAP_READ, 0, map_start, view_size);
		crc32 = this->get_crc32((const char*)map_data, file_size);
		if (map_data) UnmapViewOfFile(map_data);
		CloseHandle(file_mapping);
    }
    CloseHandle(fhandle);
	return crc32;
}

uint32_t sdk::util::c_crc32::get_crc32(const char* buf, size_t len)
{
    using namespace crc;
	DWORD crc = 0xffffffff;
	if (len >= 16)
	{
		do
		{
			DO16(buf, 0);

			buf += 16;
			len -= 16;
		} while (len >= 16);
	}
	if (len != 0)
	{
		do
		{
			DO1(buf, 0);
			++buf;
			--len;
		} while (len > 0);
	}
	crc ^= 0xffffffff;
	return crc;
}
