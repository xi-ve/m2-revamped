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
	this->spawn((LPTHREAD_START_ROUTINE)sdk::util::init_worker);
	this->spawn((LPTHREAD_START_ROUTINE)sdk::util::core_worker);
}

void __stdcall sdk::util::core_worker()
{
	while (1)
	{
		for (auto&& a : sdk::util::c_thread::Instance().get_items())
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
	if (!(uintptr_t)GetModuleHandleA("CShield.dll")) return;
	auto cs_checkheader = sdk::util::c_mem::Instance().find_pattern_directed((uintptr_t)GetModuleHandleA("CShield.dll"), a.c_str()) + offset;
	sdk::util::c_log::Instance().duo("[ %s => %04x ]\n", a.c_str(), cs_checkheader);
	if (!cs_checkheader || cs_checkheader == offset) return;
	auto ret_wpm = WriteProcessMemory(GetCurrentProcess(), (void*)cs_checkheader, &buf, size, nullptr);
}

void __stdcall sdk::util::init_worker()
{
	MH_Initialize();
	sdk::util::c_log::Instance().setup();
	main::s_startup::Instance().setup();
	sdk::util::c_mem::Instance().setup();
	sdk::util::c_config::Instance().setup();
	sdk::util::c_fn_discover::Instance().setup();

	if (strstr(sdk::util::c_fn_discover::Instance().server_name.c_str(), "Arithra2"))
	{
		char patch2[2] = { (char)0x90,(char)0x90 };//.text:73EC946B 72 BA                                   jb      short loc_73EC9427
		patch_cshield("72 ? 8B 75 ? 83 C6 ?", patch2, sizeof(patch2), 0);
		patch_cshield("75 ? 33 C0 EB ? 1B C0 83 C8 ? 85 C0 74 ? 8D 4D ? 8D 51 ? 90", patch2, sizeof(patch2), 0);

		sdk::game::c_hwid::Instance().setup();
	}

	sdk::util::c_address_gathering::Instance().setup();
	sdk::game::accconnector::c_login::Instance().setup();
	sdk::util::c_thread::Instance().append([]() 
	{
		sdk::game::accconnector::c_login::Instance().work();
	}, 15);
	sdk::util::ui_worker();
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

sdk::util::t_queue sdk::util::c_thread::get_items()
{
	return this->workers;
}
