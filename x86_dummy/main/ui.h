#pragma once
#include <inc.h>

namespace main
{
	class c_ui : public s<c_ui>
	{
	private:
		int		debug_serverdata = false;
		int		debug_actors = false;
		int		debug_items = false;
	private:		
		WNDCLASSW wc;
		RECT rect = { 0, 0, 350, 550 };
		DWORD style = WS_OVERLAPPEDWINDOW;
		DWORD exstyle = WS_EX_APPWINDOW;
		HWND wnd;
		int running = 1;
	private:
		void create_device(HWND wnd);
	private:
		void checkbox(std::string label, std::string varhead, std::string varbod, std::function<void()> fn = NULL);
		void slider(std::string label, std::string varhead, std::string varbod, int min, int max, float steps = 0.1f);
	public:
		void setup();
		void work();
	};
}