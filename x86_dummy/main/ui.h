#pragma once
#include <inc.h>

namespace main
{
	class c_ui : public s<c_ui>
	{
	private:		
		WNDCLASSW wc;
		RECT rect = { 0, 0, 350, 550 };
		DWORD style = WS_OVERLAPPEDWINDOW;
		DWORD exstyle = WS_EX_APPWINDOW;
		HWND wnd;
		int running = 1;
	private:
		void create_device(HWND wnd);
	public:
		void setup();
		void work();
	};
}