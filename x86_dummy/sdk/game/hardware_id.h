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
		class c_hwid : public s<c_hwid>
		{
		private:
			bool			GetVolumeInformationA_type();
		public:
			DWORD			session_hwid = 0;
			void			setup();
		};
	}
}