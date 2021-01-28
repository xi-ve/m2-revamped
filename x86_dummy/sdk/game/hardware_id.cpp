#include "hardware_id.h"

bool sdk::game::c_hwid::GetVolumeInformationA_type()
{
	void* a;
	if (MH_CreateHook((void*)GetVolumeInformationA, (void*)sdk::game::fn_GetVolumeInformationA, (void**)&a) != MH_OK) return 0;;
	if (MH_EnableHook((void*)GetVolumeInformationA) != MH_OK) return 0;;
	return 1;
}

void sdk::game::c_hwid::setup()
{
	auto w = DWORD();
	srand((uint32_t)&w + GetTickCount());
	this->session_hwid = rand() % MAXDWORD;
	this->GetVolumeInformationA_type();

	sdk::util::c_log::Instance().duo("[ using HWID bypass 1 ]\n");
}

void __stdcall sdk::game::fn_GetVolumeInformationA(LPCSTR lpRootPathName, LPSTR lpVolumeNameBuffer, DWORD nVolumeNameSize, LPDWORD lpVolumeSerialNumber, LPDWORD lpMaximumComponentLength, LPDWORD lpFileSystemFlags, LPSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize)
{
	sdk::util::c_log::Instance().print("[ request for serial by client! ]\n");
	*lpVolumeSerialNumber = sdk::game::c_hwid::Instance().session_hwid;
}
