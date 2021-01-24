#pragma once
#define NANOGUI_USE_OPENGL 1
#define _GLFW_WIN32 1
#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 1
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <TlHelp32.h>
#include <Psapi.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unordered_map>
#include <thread>
#include <map>
#include <functional>
#include <atomic>
#include <regex>
#include <sstream>
#include <d3d9.h>
//libs
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "d3d9.lib")
#include <zydis/Zydis/Zydis.h>
#include <zydis/Zycore/LibC.h>
#pragma comment(lib, "zydis/Zydis.lib")
#pragma comment(lib, "zydis/Zycore.lib")
#include <minhook/MinHook.h>
#pragma comment(lib, "minhook/libMinHook.lib")
#include <json/json.h>
//cheat predefs
#include <win_strct.h>
//cheat core
#include <util/xor.h>
#include <singleton.h>
#include <crc32/crc32.h>
#include <util/crc32.h>
#include <util/log.h>
#include <util/mem.h>
#include <util/disassembler.h>
#include <util/threading.h>
#include <util/config.h>
#include <game/offsets.h>
#include <game/functions.h>
#include <game/utils.h>
#include <util/rtti.h>
#include <util/fn_discover.h>
#include <util/address_gathering.h>
#include <ui.h>
#include <startup.h>