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

decltype(sdk::game::hooks::o_SG_HWID) sdk::game::hooks::o_SG_HWID = 0;
uint32_t* __fastcall sdk::game::hooks::f_SG_HWID(uint32_t a1, uint32_t* a2)
{
	auto orig = sdk::game::hooks::o_SG_HWID(a1, a2);


	
	return nullptr;
}

void PrintMACaddress(BYTE* addr)
{
	for (int i = 0; i < 8; i++)
	{
		printf("%x ", *addr++);
	}
}

static void GetMACaddress(void)
{
	IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information for up to 16 NICs
	DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

	DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);                  // [in] size of receive data buffer
	if (dwStatus != ERROR_SUCCESS)
	{
		printf("GetAdaptersInfo failed. err=%d\n", GetLastError());
		return;
	}

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // Contains pointer to  current adapter info
	do
	{
		PrintMACaddress(pAdapterInfo->Address); // Print MAC address
		pAdapterInfo = pAdapterInfo->Next;    // Progress through linked list
	} while (pAdapterInfo);                    // Terminate if last adapter
}
	
void sdk::game::c_hwid::setup()
{
	GetMACaddress();
	sdk::util::c_log::Instance().duo("[ ~~~ ]\n");

	auto w = DWORD();
	srand((uint32_t)&w + GetTickCount());
	this->session_hdd_serial = rand() % MAXDWORD;
	for(int i =0;i< 8;i++)
	{
		this->session_mac[i] = rand() % 0xFF;
	}
	this->GetVolumeInformationA_type();

	sdk::util::c_log::Instance().duo("[ using HWID bypass 1 ]\n");
	
	GetMACaddress();
	sdk::util::c_log::Instance().duo("[ ~~~ ]\n");


	/*auto SG_method = sdk::util::c_mem::Instance().find_pattern((uintptr_t)GetModuleHandleA(0), XorStr("53 8B DC 83 EC ? 83 E4 ? 83 C4 ? 55 8B 6B ? 89 6C 24 ? 8B EC 6A ? 68 ? ? ? ? 64 A1 ? ? ? ? 50 53 B8 ? ? ? ? E8 ? ? ? ? A1 ? ? ? ? 33 C5 89 45 ? 56 57 50 8D 45 ? 64 A3 ? ? ? ? 8B 7B ?"));
	if (SG_method)
	{
		sdk::util::c_log::Instance().duo("[ using SG HWID bypass ]\n");

	}*/
}

void __stdcall sdk::game::fn_GetVolumeInformationA(LPCSTR lpRootPathName, LPSTR lpVolumeNameBuffer, DWORD nVolumeNameSize, LPDWORD lpVolumeSerialNumber, LPDWORD lpMaximumComponentLength, LPDWORD lpFileSystemFlags, LPSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize)
{
	sdk::util::c_log::Instance().print("[ request for hdd serial by client! ]\n");
	*lpVolumeSerialNumber = sdk::game::c_hwid::Instance().session_hdd_serial;
}

decltype(sdk::game::hooks::o_GetAdaptersInfo) sdk::game::hooks::o_GetAdaptersInfo = 0;
ULONG __stdcall sdk::game::fn_GetAdaptersInfo(PIP_ADAPTER_INFO AdapterInfo, PULONG SizePointer)
{
	srand((uint32_t)&AdapterInfo + rand() % 0xFFFFF);
	
	sdk::util::c_log::Instance().print("[ request for mac by client! ]\n");
	
	ULONG l = sdk::game::hooks::o_GetAdaptersInfo(AdapterInfo, SizePointer);

	if (l == NO_ERROR)
	{
		PIP_ADAPTER_INFO pCurrAddresses = AdapterInfo;
		while (pCurrAddresses)
		{
			if (pCurrAddresses->AddressLength != 0)
			{
				pCurrAddresses->Address[0] = rand() % 0xff;
				pCurrAddresses->Address[1] = rand() % 0xff;
				pCurrAddresses->Address[2] = rand() % 0xff;
				pCurrAddresses->Address[3] = rand() % 0xff;
				pCurrAddresses->Address[4] = rand() % 0xff;
				pCurrAddresses->Address[5] = rand() % 0xff;
			}

			pCurrAddresses = pCurrAddresses->Next;
		}
	}
	return l;
}
