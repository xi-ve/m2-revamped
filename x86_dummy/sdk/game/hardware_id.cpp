#include "hardware_id.h"

bool sdk::game::c_hwid::GetVolumeInformationA_type()
{
	void* a;
	if (MH_CreateHook((void*)GetVolumeInformationA, (void*)sdk::game::fn_GetVolumeInformationA, (void**)&a) != MH_OK) return 0;
	if (MH_EnableHook((void*)GetVolumeInformationA) != MH_OK) return 0;

	if (MH_CreateHook((void*)GetAdaptersInfo, (void*)sdk::game::fn_GetAdaptersInfo, (void**)&sdk::game::hooks::o_GetAdaptersInfo) != MH_OK) return 0;
	if (MH_EnableHook((void*)GetAdaptersInfo) != MH_OK) return 0;
	
	return 1;
}

void sdk::game::c_hwid::setup()
{
	auto w = DWORD();
	srand((uint32_t)&w + GetTickCount());
	this->session_hdd_serial = rand() % MAXDWORD;
	for(int i =0;i< 8;i++)
	{
		this->session_mac[i] = rand() % 254;
	}
	this->GetVolumeInformationA_type();

	sdk::util::c_log::Instance().duo("[ using HWID bypass 1 ]\n");
}

void __stdcall sdk::game::fn_GetVolumeInformationA(LPCSTR lpRootPathName, LPSTR lpVolumeNameBuffer, DWORD nVolumeNameSize, LPDWORD lpVolumeSerialNumber, LPDWORD lpMaximumComponentLength, LPDWORD lpFileSystemFlags, LPSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize)
{
	sdk::util::c_log::Instance().print("[ request for hdd serial by client! ]\n");
	*lpVolumeSerialNumber = sdk::game::c_hwid::Instance().session_hdd_serial;
}

decltype(sdk::game::hooks::o_GetAdaptersInfo) sdk::game::hooks::o_GetAdaptersInfo = 0;
ULONG __stdcall sdk::game::fn_GetAdaptersInfo(PIP_ADAPTER_INFO AdapterInfo, PULONG SizePointer)
{
	sdk::util::c_log::Instance().print("[ request for mac by client! ]\n");
	auto ret = sdk::game::hooks::o_GetAdaptersInfo(AdapterInfo, SizePointer);
	PIP_ADAPTER_INFO pCurrAddresses = AdapterInfo;
	while (pCurrAddresses)
	{
		if (pCurrAddresses->AddressLength != 0)
		{
			for(int i = 0;i<8;i++)
			{
				pCurrAddresses->Address[i] = sdk::game::c_hwid::Instance().session_mac[i];
			}
			
		}

		pCurrAddresses = pCurrAddresses->Next;
	}
	return ret;
}