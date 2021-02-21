#pragma once
#include <inc.h>

namespace main
{
	class c_ui : public s<c_ui>
	{
	private:
		HWND		hwnd = 0;
		WNDCLASSEX	wc = {};
	private:
		int		debug_serverdata = false;
		int		debug_actors = false;
		int		debug_items = false;
		int		debug_item_edit = false;
		int*	selected_element_item = {};
	private:
		char input_search_item[26] = "";
		char input_point_name[26] = "";
	private:
		int running = 1;
	private:
		std::string	to_lower(std::string in);
	private:
		void render();
		void create_device(HWND wnd);
	private:
		void checkbox(std::string label, std::string varhead, std::string varbod, std::function<void()> fn = NULL);
		void slider(std::string label, std::string varhead, std::string varbod, int min, int max, float steps = 0.1f);
	public:
		void setup();
		void work();
	};
}