#include "threading.h"
using namespace std::chrono;

void sdk::util::c_thread::create_buffer(NtCreateThreadExBuffer* buffer)
{
	DWORD temp1 = 0;
	DWORD temp2 = 0;
	buffer->Size = sizeof(NtCreateThreadExBuffer);
	buffer->Unknown1 = 0x10003;
	buffer->Unknown2 = 0x8;
	buffer->Unknown3 = &temp2;
	buffer->Unknown4 = 0;
	buffer->Unknown5 = 0x10004;
	buffer->Unknown6 = 4;
	buffer->Unknown7 = &temp1;
	buffer->Unknown8 = 0;
}

HANDLE sdk::util::c_thread::create_ntthread(LPTHREAD_START_ROUTINE fn, void* param)
{
	auto ntdll_handle = GetModuleHandleA(XorStr("ntdll.dll"));
	if (!ntdll_handle) return 0;
	auto fn_nt_create_thread_ex = (NtCreateThreadExType)(GetProcAddress(ntdll_handle, XorStr("NtCreateThreadEx")));
	if (!fn_nt_create_thread_ex) { sdk::util::c_log::Instance().duo(XorStr("[ failed to get NtCreateThreadEx ]\n")); return 0; }
	auto buffer = NtCreateThreadExBuffer() = {};
	this->create_buffer(&buffer);
	HANDLE created_thread = 0;
	fn_nt_create_thread_ex(&created_thread, 0x1fffff, 0, GetCurrentProcess(), fn, param, 0, 0, 0, 0, &buffer);
	if (!created_thread) { sdk::util::c_log::Instance().duo(XorStr("[ failed creating ntthread! ]\n")); return 0; }
	hide_thread(created_thread);
	return created_thread;
}

void sdk::util::c_thread::hide_thread(HANDLE target)
{
	auto ntdll_handle = GetModuleHandleA(XorStr("ntdll.dll"));
	if (!ntdll_handle) return;
	auto fn_nt_set_information_thread = (_NtSetInformationThread)(GetProcAddress(ntdll_handle, XorStr("NtSetInformationThread")));
	if (!fn_nt_set_information_thread) { sdk::util::c_log::Instance().duo(XorStr("[ failed to get NtSetInformationThread ]\n")); return; }
	fn_nt_set_information_thread(target, (THREAD_INFORMATION_CLASS)0x11, 0, 0);
}

void sdk::util::c_thread::setup()
{
	std::thread a(sdk::util::init_worker);
	std::thread b(sdk::util::core_worker);
	a.detach();
	b.detach();
	//this->spawn((LPTHREAD_START_ROUTINE)sdk::util::init_worker);
	//this->spawn((LPTHREAD_START_ROUTINE)sdk::util::core_worker);
}

void __stdcall sdk::util::core_worker()
{
	while (1)
	{
		for (auto&& a : *sdk::util::c_thread::Instance().get_items())
		{
			if (a.last_execution > GetTickCount64()) continue;

			a.function();

			a.last_execution = a.interval + GetTickCount64();
		}
		std::this_thread::sleep_for(10ms);
	}
}

void __stdcall sdk::util::ui_worker()
{
	main::c_ui::Instance().setup();
	main::c_ui::Instance().work();
}

void patch_cshield(std::string a, char* buf, size_t size, uint32_t offset)
{
	if (!(uintptr_t)GetModuleHandleA(XorStr("CShield.dll"))) return;
	auto cs_checkheader = sdk::util::c_mem::Instance().find_pattern_directed((uintptr_t)GetModuleHandleA(XorStr("CShield.dll")), a.c_str()) + offset;
	if (!cs_checkheader || cs_checkheader == offset) return;
	auto ret_wpm = WriteProcessMemory(GetCurrentProcess(), (void*)cs_checkheader, &buf, size, nullptr);
}
uint32_t get_cshield_addr(std::string a)
{
	if (!(uintptr_t)GetModuleHandleA(XorStr("CShield.dll"))) return 0;
	auto cs_checkheader = sdk::util::c_mem::Instance().find_pattern_directed((uintptr_t)GetModuleHandleA(XorStr("CShield.dll")), a.c_str());
	return cs_checkheader;
}

void empt_func()
{

}
void __fastcall ac_shit(uint32_t self, int a2, int a3)
{

}
typedef void(__fastcall* cshield_worker)(uint32_t);
cshield_worker o_cshield_worker = 0;
uint32_t __fastcall ac_cshield_worker(uint32_t base)
{
	printf(XorStr("[ CShield attempts to do bad thing, BONK! ]\n"));
	return 1;
}
void __stdcall ac_cshield_worker2()
{
	printf("[ 2 CShield attempts to do bad thing, BONK! ]\n");
}
void __stdcall sdk::util::init_worker()
{
	void* rr; 
	MH_Initialize();
	sdk::util::c_log::Instance().setup();
	main::s_startup::Instance().setup();
	sdk::util::c_mem::Instance().setup();
	sdk::util::c_config::Instance().setup();
	sdk::util::c_fn_discover::Instance().setup();
	sdk::util::c_address_gathering::Instance().setup();

	if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), XorStr("Kuba2")) || strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), XorStr("Anoria2")) || strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), XorStr("Arithra2")) || strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), XorStr("SunshineMt2")) || strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), XorStr("Akeno2")) || strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), XorStr("Yumano3")))
	{
		auto _code_55_fun = get_cshield_addr(XorStr("55 8B EC 6A ? 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC ? A1 ? ? ? ? 33 C5 89 45 ? 53 56 57 50 8D 45 ? 64 A3 ? ? ? ? 89 55 ?"));

		MH_CreateHook((void*)_code_55_fun, (void*)ac_cshield_worker, (void**)&rr);
		MH_EnableHook((void*)_code_55_fun);

		sdk::util::c_log::Instance().duo(XorStr("[ CShield bonk executed ]\n"));
	}
	if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), XorStr("Arithra2")) || strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), XorStr("Realm2")) || strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), XorStr("xaleas")))
	{
		sdk::game::c_hwid::Instance().setup();
	}

	sdk::game::accconnector::c_login::Instance().setup();
	sdk::game::chr::c_char::Instance().setup();
	sdk::game::c_waithack::Instance().setup();
	sdk::game::item::c_item_gather::Instance().setup();
	sdk::game::c_pickup::Instance().setup();
	sdk::game::chr::c_misc::Instance().setup();
	sdk::game::chr::c_pull::Instance().setup();
	sdk::game::chr::c_tp_point::Instance().setup();

	main::c_ui::Instance().setup();
	
	sdk::util::c_thread::Instance().append([]()
	{
		sdk::util::c_config::Instance().save();
	}, 5000);

	sdk::util::c_thread::Instance().append([]()
	{
		if (sdk::util::c_address_gathering::Instance().done)
		{
			sdk::game::accconnector::c_login::Instance().work();

			if (sdk::game::item::c_item_manager::Instance().did_grab)
			{
				sdk::game::c_waithack::Instance().work();
				sdk::game::item::c_item_gather::Instance().work();
				sdk::game::c_pickup::Instance().work();
				sdk::game::chr::c_misc::Instance().work();
				sdk::game::chr::c_pull::Instance().work();
			}
			else sdk::game::item::c_item_manager::Instance().grab();
		}
	}, 15);

	main::c_ui::Instance().work();
}

bool sdk::util::c_thread::spawn(LPTHREAD_START_ROUTINE fn, void* param)
{
	this->create_ntthread(fn, param);
	return false;
}

bool sdk::util::c_thread::append(std::function<void()> lambda, uint32_t interval)
{
	this->workers.emplace_back(interval, lambda);
	return false;
}

sdk::util::t_queue* sdk::util::c_thread::get_items()
{
	return &this->workers;
}
