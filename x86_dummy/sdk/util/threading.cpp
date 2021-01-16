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
	auto ntdll_handle = GetModuleHandleA("ntdll.dll");
	if (!ntdll_handle) return 0;
	auto fn_nt_create_thread_ex = (NtCreateThreadExType)(GetProcAddress(ntdll_handle, "NtCreateThreadEx"));
	if (!fn_nt_create_thread_ex) { sdk::util::c_log::Instance().duo("[ failed to get NtCreateThreadEx ]\n"); return 0; }
	auto buffer = NtCreateThreadExBuffer() = {};
	this->create_buffer(&buffer);
	HANDLE created_thread = 0;
	fn_nt_create_thread_ex(&created_thread, 0x1fffff, 0, GetCurrentProcess(), fn, param, 0, 0, 0, 0, &buffer);
	if (!created_thread) { sdk::util::c_log::Instance().duo("[ failed creating ntthread! ]\n"); return 0; }
	hide_thread(created_thread);
	return created_thread;
}

void sdk::util::c_thread::hide_thread(HANDLE target)
{
	auto ntdll_handle = GetModuleHandleA("ntdll.dll");
	if (!ntdll_handle) return;
	auto fn_nt_set_information_thread = (_NtSetInformationThread)(GetProcAddress(ntdll_handle, "NtSetInformationThread"));
	if (!fn_nt_set_information_thread) { sdk::util::c_log::Instance().duo("[ failed to get NtSetInformationThread ]\n"); return; }
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

void __stdcall sdk::util::init_worker()
{
	main::s_startup::Instance().setup();
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
