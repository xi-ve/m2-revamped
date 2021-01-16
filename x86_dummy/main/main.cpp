#include <inc.h>
BOOL APIENTRY DllMain(HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {               
        DisableThreadLibraryCalls(hModule);
        sdk::util::c_thread::Instance().setup();
        break;
    }
    }
    return TRUE;
}

