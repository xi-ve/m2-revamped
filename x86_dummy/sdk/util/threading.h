#pragma once
#include <inc.h>
namespace sdk
{
	namespace util
	{
		typedef int(__stdcall* _NtQueryInformationProcess)(_In_ HANDLE, _In_  unsigned int, _Out_ PVOID, _In_ ULONG, _Out_ PULONG);
		typedef int(__stdcall* _NtSetInformationThread)(_In_ HANDLE, _In_ THREAD_INFORMATION_CLASS, _In_ PVOID, _In_ ULONG);

		extern void __stdcall core_worker();
		extern void __stdcall ui_worker();
		extern void __stdcall init_worker();

		typedef void(__fastcall* t_generic_function)();
		struct s_worker_info
		{
			s_worker_info(uint32_t ival, std::function<void()> fn)
			{
				interval = ival;
				function = fn;
			}
			uint32_t				interval = 0;
			ULONGLONG				last_execution = 0;
			std::function<void()>	function = 0;
		};
		typedef std::vector<s_worker_info> t_queue;

		class c_thread : public s<c_thread>
		{
		private:
			t_queue	workers = {};
			HANDLE	core_worker = 0;
		private:
			void	create_buffer(NtCreateThreadExBuffer* buffer);
			HANDLE	create_ntthread(LPTHREAD_START_ROUTINE fn, void* param = nullptr);
			void	hide_thread(HANDLE target);
		public:
			void	setup();
			bool	spawn(LPTHREAD_START_ROUTINE fn, void* param = nullptr);
			bool	append(std::function<void()> lambda, uint32_t interval);
		public:
			t_queue*	get_items();
		};
	}
}