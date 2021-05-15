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
#include <map>
#include <thread>
#include <map>
#include <functional>
#include <atomic>
#include <regex>
#include <sstream>
#include <math.h>
#include <concurrencysal.h>
#include <filesystem>
#include <io.h>
#include <fcntl.h>
#include <iphlpapi.h>

//libs
#include <d3d9.h>
#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\d3dx9.h"
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")

#include <zydis/Zydis/Zydis.h>
#include <zydis/Zycore/LibC.h>
#pragma comment(lib, "zydis/Zydis.lib")
#pragma comment(lib, "zydis/Zycore.lib")
#include <minhook/MinHook.h>
#pragma comment(lib, "minhook/libMinHook.lib")
#include <json/json.h>
#include "bytebuffer/bytebuffer.h"
//
#include "..\\lib\\imgui\\imgui_impl_win32.h"
#include "..\\lib\\imgui\\imgui_impl_dx9.h"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
//cheat predefs
#include <win_strct.h>
//cheat core
#include <util/structs.h>
#include <util/xor.h>
#include <singleton.h>
#include <util/maths.h>
#include <crc32/crc32.h>
#include <util/crc32.h>
#include <util/log.h>
#include <util/config.h>
#include <util/mem.h>
#include <util/disassembler.h>
#include <util/threading.h>
#include <game/offsets.h>
#include <game/functions.h>
#include <game/utils.h>
#include <game/char/utils.h>
#include <util/rtti.h>
#include <util/fn_discover.h>
#include <util/address_gathering.h>
#include <game/item/item_gather.h>
#include <game/item/item_manager.h>
#include <game/item/pickup.h>
#include <game/char/misc.h>
#include <game/accconnector/login.h>
#include <game/hardware_id.h>
#include <game/char/waithack.h>
#include <game/char/reducer.h>
#include <game/char/pull.h>
#include <game/char/tp_point.h>
#include <game/file/dump.h>
#include <game/map/map_files.h>
#include <ui.h>
#include <startup.h>