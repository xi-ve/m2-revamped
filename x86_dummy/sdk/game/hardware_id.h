#pragma once
#include <inc.h>
namespace sdk
{
	namespace game
	{
		void __stdcall fn_GetVolumeInformationA(LPCSTR lpRootPathName,
												_Out_writes_opt_(nVolumeNameSize) LPSTR lpVolumeNameBuffer,
												_In_ DWORD nVolumeNameSize,
												_Out_opt_ LPDWORD lpVolumeSerialNumber,
												_Out_opt_ LPDWORD lpMaximumComponentLength,
												_Out_opt_ LPDWORD lpFileSystemFlags,
												_Out_writes_opt_(nFileSystemNameSize) LPSTR lpFileSystemNameBuffer,
												_In_ DWORD nFileSystemNameSize);
		
		DWORD __stdcall fn_GetAdaptersInfo(PIP_ADAPTER_INFO AdapterInfo, PULONG SizePointer);


		namespace hooks
		{
			typedef ULONG(__stdcall* t_GetAdaptersInfo)(PIP_ADAPTER_INFO AdapterInfo, PULONG SizePointer);
			extern t_GetAdaptersInfo o_GetAdaptersInfo;
			extern bool __stdcall f_GetAdaptersInfo(uint32_t base);
		}
		
		class c_hwid : public s<c_hwid>
		{
		private:
			bool			GetVolumeInformationA_type();
		public:
			DWORD			session_hdd_serial = 0;
			BYTE			session_mac[8] = {0};
			void			setup();
			
		};
	}
}