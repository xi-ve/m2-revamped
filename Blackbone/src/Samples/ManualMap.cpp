#include <BlackBone/Process/Process.h>
#pragma comment(lib, "BlackBone.lib")

#include <iostream>
#include <set>
using namespace blackbone;
using namespace std::chrono;

std::set<std::wstring> nativeMods, modList;
DWORD lastpid;

std::vector<DWORD> getpids() {
    std::vector<DWORD> pids;
    std::vector<std::string> Names = { "Olympia2.exe", "metin2client.bin", "metin2client.exe", "Anoria2.exe", "Kevra.exe", "Arithra2-Client.exe", "Akeno2_new.exe", "SunshineMt2.exe" };
    for (auto a : Names) {
        std::vector<DWORD> buf = Process::EnumByName(std::wstring(a.begin(),a.end()));
        for (auto i : buf) {
            pids.push_back(i);
        }
    }

    return pids;
}

void MapCmdFromMem()
{
    std::vector<DWORD> pids = getpids();

    while (!pids.size()) { pids = getpids(); }
    if (pids.front() == lastpid) return;
    lastpid = pids.front();

    lastpid = pids.front();
    printf("pid: %i\n",pids.front());

    std::this_thread::sleep_for(250ms);

    void* buf = nullptr;
    auto size = 0;

    // Get image context
    HANDLE hFile = CreateFileW( L"m2++.dll", FILE_GENERIC_READ, 0x7, 0, OPEN_EXISTING, 0, 0 );
    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD bytes = 0;
        size = GetFileSize( hFile, NULL );
        buf = VirtualAlloc( NULL, size, MEM_COMMIT, PAGE_READWRITE );
        ReadFile( hFile, buf, size, &bytes, NULL );
        CloseHandle( hFile );
    }

	Process thisProc;
	thisProc.Attach(pids.front());

    auto image = thisProc.mmap().MapImage( size, buf, false, NoThreads | WipeHeader );
    if (!image)
    {
        std::wcout << L"Mapping failed with error 0x" << std::hex << image.status
                   << L". " << Utils::GetErrorDescription( image.status ) << std::endl << std::endl;
    }
    else
        std::wcout << L"Successfully mapped, unmapping\n";

    thisProc.Detach();

    VirtualFree( buf, 0, MEM_RELEASE );
}
