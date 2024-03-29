#pragma once
#include <inc.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x86\\d3dx9.lib")
namespace main
{
	extern LPDIRECT3D9              g_pD3D;
	extern LPDIRECT3DDEVICE9        g_pd3dDevice;
	extern D3DPRESENT_PARAMETERS    g_d3dpp;
	
	class c_ui : public s<c_ui>
	{
	private:
		HWND		hwnd = 0;
		WNDCLASSEX	wc = {};
	private:
		bool		login_tab_enabled = false;
		bool		looting_tab_enabled = false;
		bool		waithack_tab_enabled = false;
	private:
		char		input_search_item[26] = "";
		char		input_point_name[26] = "";
	private:
		int			running = 1;
	public:
		std::string	to_lower(std::string in);
	private:
		void		draw_point(ImVec2 main_map_pos, ImVec2 zoom_start, ImVec2 zoom_area_start,
		                sdk::game::chr::vec act_pos_map, ImColor clr, byte w);
		void		render();
		void		create_device(HWND wnd);
	private:
		void		checkbox(std::string label, std::string varhead, std::string varbod, std::function<void()> fn = NULL);
		void		slider(std::string label, std::string varhead, std::string varbod, int min, int max, float steps = 0.1f);
	public:
		bool		load_texture(const char* filename, PDIRECT3DTEXTURE9* out_texture, int* out_width, int* out_height);		
		void		setup();
		void		work();
	};
}