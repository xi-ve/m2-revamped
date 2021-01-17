#pragma once
#include <inc.h>
namespace crypto
{
	class c_crc32 : public s<c_crc32>
	{
	public:
		DWORD getcrc32(const char* buffer, size_t count);
		DWORD getcasecrc32(const char* buf, size_t len);

		DWORD gethfilecrc32(HANDLE hFile);

		DWORD getfilecrc32(const char* c_szFileName);
		DWORD getfilesize(const char* c_szFileName);
	};
	extern c_crc32* crc32;
}