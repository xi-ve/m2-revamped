#pragma once
#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
#define _CRT_SECURE_NO_WARNINGS
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

//libs
#include <capstone/include/capstone/capstone.h>
#pragma comment(lib, "capstone/capstone.lib")
#include <minhook/MinHook.h>
#pragma comment(lib, "minhook/libMinHook.lib")
#include <json/json.h>
//cheat predefs
#include <win_strct.h>
//cheat core
#include <singleton.h>
#include <crc32/crc32.h>
#include <util/crc32.h>
#include <util/log.h>
#include <util/mem.h>
#include <util/disassembler.h>
#include <util/threading.h>
#include <util/config.h>
#include <util/fn_discover.h>
#include <startup.h>