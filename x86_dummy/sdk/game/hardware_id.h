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
			//_DWORD *__usercall sub_551F60@<eax>(int a1@<ebp>, _DWORD *a2)
			typedef uint32_t* (__fastcall* t_SG_HWID)(uint32_t a1, uint32_t* a2);
			extern t_SG_HWID o_SG_HWID;
			extern uint32_t* __fastcall f_SG_HWID(uint32_t a1, uint32_t* a2);
		}
		
		class c_hwid : public s<c_hwid>
		{
		private:
			bool			GetVolumeInformationA_type();
		public:
			DWORD			session_hdd_serial = 0;
			BYTE			session_mac[9] = {0};
			void			setup();
			
		};
	}
}