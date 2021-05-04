#include "ui.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x86\\d3dx9.lib")

#include "game/map/map_grabber.h"
#include "imgui/imgui_internal.h"

std::string main::c_ui::to_lower(std::string in)
{
	std::string r = "";
	for (auto a : in) r.push_back(std::tolower(a));
	return r;
}

// Data
LPDIRECT3D9              main::g_pD3D = NULL;
LPDIRECT3DDEVICE9        main::g_pd3dDevice = NULL;
D3DPRESENT_PARAMETERS    main::g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void main::c_ui::draw_point(ImVec2 main_map_pos, ImVec2 zoom_start, ImVec2 zoom_area_start, sdk::game::chr::vec act_pos_map, ImColor clr, byte w)
{
	ImGui::GetWindowDrawList()->AddCircleFilled(
		{
			main_map_pos.x + act_pos_map.x,
			main_map_pos.y + act_pos_map.y
		},
		w, clr);
	ImGui::GetWindowDrawList()->AddCircleFilled(
		{
			zoom_start.x + ((act_pos_map.x - zoom_area_start.x) * 5.f),
			zoom_start.y + ((act_pos_map.y - zoom_area_start.y) * 5.f)
		},
		w, clr);
}

void main::c_ui::render()
{
	auto viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	if (ImGui::Begin(XorStr("##main"), 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration))
	{
		auto idx = ImGui::GetID(XorStr("main_space_docker"));
		ImGui::DockSpace(idx);

		ImGui::SetNextWindowDockID(idx, ImGuiCond_::ImGuiCond_FirstUseEver);
		if (ImGui::Begin(XorStr("login"), 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize))
		{
			this->checkbox(XorStr("login"), XorStr("login"), XorStr("enable"));
			ImGui::SameLine(0, 25);
			ImGui::Checkbox(XorStr("log next login"), &sdk::game::accconnector::c_login::Instance().should_grab_details);

			if (sdk::game::accconnector::c_login::Instance().account_selected.username.size())
			{
				if (ImGui::Button(XorStr("login "))) sdk::game::accconnector::c_login::Instance().set_details();
				if (ImGui::Button(XorStr("select"))) sdk::game::accconnector::c_login::Instance().set_connect();
			}
			this->checkbox(XorStr("GPU reducer"), XorStr("reducer"), XorStr("toggle"));

			//TODO: selector for accounts, slot, ch

			ImGui::End();
		}
		ImGui::SetNextWindowDockID(idx, ImGuiCond_::ImGuiCond_FirstUseEver);
		if (ImGui::Begin(XorStr("loot"), 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize))
		{
			this->checkbox(XorStr("pickup"), XorStr("pickup"), XorStr("toggle"));
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("only blacklist"), &sdk::game::c_pickup::Instance().item_conf.only_pick_blacklist);
			this->slider(XorStr("range"), XorStr("pickup"), XorStr("range"), 300, 20000, 100.f);
			this->slider(XorStr("delay"), XorStr("pickup"), XorStr("delay"), 0, 1000, 5.f);

			ImGui::InputText(XorStr("##search item: "), this->input_search_item, 16);
			if (strlen(this->input_search_item))
			{
				struct s_item_view
				{
					std::string		name;
					uint32_t		vnum;
					bool			is_blacklisted;
				};

				std::vector<s_item_view> res = {};

				auto serach_term = this->to_lower(this->input_search_item);

				for (auto a : sdk::game::item::c_item_manager::Instance().item_names)
				{
					auto lower = this->to_lower(a.second);
					if (!strstr(lower.c_str(), serach_term.c_str())) continue;
					auto vr = s_item_view();
					vr.name = a.second;
					vr.vnum = a.first;
					vr.is_blacklisted = sdk::game::c_pickup::Instance().is_blacklisted(a.first);
					res.push_back(vr);
				}

				ImGui::BeginChild("##filter_results", { 0,0 }, true, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);

				if (!res.empty())
				{
					for (auto a : res)
					{
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, D3DCOLOR_RGBA(0, 0, 0, 0));
						if (a.is_blacklisted) ImGui::PushStyleColor(ImGuiCol_Text, { 255,0,0,255 });
						else ImGui::PushStyleColor(ImGuiCol_Text, { 0,255,0,255 });
						if (ImGui::Button(a.name.c_str(), { ImGui::GetWindowWidth() - 25, 0 }))
						{
							if (a.is_blacklisted)
							{
								sdk::game::c_pickup::Instance().unblacklist(a.vnum);
								sdk::game::c_pickup::Instance().add_whitelist(a.vnum);
							}
							else
							{
								sdk::game::c_pickup::Instance().unwhitelist(a.vnum);
								sdk::game::c_pickup::Instance().add_blacklist(a.vnum);
							}
						}
						ImGui::PopStyleColor(2);
					}
				}
				else ImGui::TextColored({ 255,0,0,255 }, "no results found");

				ImGui::EndChild();
			}

			ImGui::End();
		}
		ImGui::SetNextWindowDockID(idx, ImGuiCond_::ImGuiCond_FirstUseEver);
		if (ImGui::Begin(XorStr("waithack"), 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize))
		{
			this->checkbox(XorStr("waithack    "), XorStr("waithack"), XorStr("toggle"));
			ImGui::SameLine(0, 25);
			this->checkbox(XorStr("metins      "), XorStr("waithack"), XorStr("metins"));

			this->checkbox(XorStr("mobs        "), XorStr("waithack"), XorStr("mobs"));
			ImGui::SameLine(0, 25);
			this->checkbox(XorStr("player      "), XorStr("waithack"), XorStr("player"));

			this->checkbox(XorStr("on_attack   "), XorStr("waithack"), XorStr("on_attack"));
			ImGui::SameLine(0, 25);
			this->checkbox(XorStr("bp_on_attack"), XorStr("waithack"), XorStr("bp_on_attack"));

			this->checkbox(XorStr("dmg-boost   "), XorStr("waithack"), XorStr("boost"));
			ImGui::SameLine(0, 25);
			this->checkbox(XorStr("wallhack    "), XorStr("misc"), XorStr("wallhack"));

			this->checkbox(XorStr("pull        "), XorStr("pull"), XorStr("toggle"));
			ImGui::SameLine(0, 25);
			this->checkbox(XorStr("use_packets "), XorStr("pull"), XorStr("packet"));

			this->slider(XorStr("range  "), XorStr("waithack"), XorStr("range"), 300, 20000, 100.f);
			this->slider(XorStr("targets"), XorStr("waithack"), XorStr("targets"), 1, 25, 1.f);
			this->slider(XorStr("speed  "), XorStr("waithack"), XorStr("speed"), 1, 100, 1.f);
			this->slider(XorStr("anchor "), XorStr("waithack"), XorStr("anchor"), 100, 5000, 100.f);
			this->checkbox(XorStr("use_bow "), XorStr("waithack"), XorStr("bow_mode"));

			ImGui::End();
		}
		ImGui::SetNextWindowDockID(idx, ImGuiCond_::ImGuiCond_FirstUseEver);
		if (ImGui::Begin(XorStr("debug"), 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize))
		{
			if(std::filesystem::exists("Maps") && std::filesystem::is_directory("Maps")){
			auto background_base = sdk::game::c_utils::Instance().baseclass_background();
			if (background_base)
			{
				const auto bg_name = *(std::string*)(background_base + sdk::game::background::off_CUR_MAP_NAME);
				ImGui::Text("current map: %s", bg_name.c_str());

				const auto tex = sdk::game::map::c_map_files::Instance().find(bg_name);
				if (tex && tex->texture && ImGui::GetCurrentWindow()->DrawList)
				{
					ImGui::Text("map scale: x %i  y %i", tex->width, tex->height);

					ImGui::Image((void*)tex->texture, { (float)tex->width / 4, (float)tex->height / 4 });
					
					auto main_actor = sdk::game::chr::c_char::Instance().get_main_actor();
					if (main_actor)
					{
						auto self_pos = sdk::game::chr::c_char::Instance().get_pos(main_actor);
						self_pos.x /= 100.f;
						self_pos.y *= -1.f;
						self_pos.y /= 100.f;

						auto map_pos = self_pos;
						map_pos.x /= 4;
						map_pos.y /= 4;

						auto main_map_pos = ImGui::GetItemRectMin();

						ImGui::Image(
							(void*)tex->texture,
							{ tex->width / 4.f, tex->height / 4.f },
							{ (float)(map_pos.x / (tex->width / 4.f)) - 0.10f, (float)((map_pos.y / (tex->height / 4.f))) - 0.10f },
							{ (float)(map_pos.x / (tex->width / 4.f)) + 0.10f, (float)((map_pos.y / (tex->height / 4.f))) + 0.10f }
						);	

						auto zoom_start = ImGui::GetItemRectMin();//zoom image start imgui pos
					
						auto zoom_area_start = ImVec2(
							((float)(map_pos.x / (tex->width / 4.f)) - 0.10f) * (tex->width / 4.f),
							((float)((map_pos.y / (tex->height / 4.f))) - 0.10f) * (tex->height / 4.f));
						
						for (auto a : sdk::game::chr::c_char::Instance().get_alive())
						{
							auto p = (uint32_t)a.second;
							if (!p || p == main_actor) continue;
							if (sdk::game::chr::c_char::Instance().is_dead_actor(p)) continue;

							auto act_pos = sdk::game::chr::c_char::Instance().get_pos(p);
							act_pos.x /= 100.f;
							act_pos.y *= -1.f;
							act_pos.y /= 100.f;

							auto act_pos_map = act_pos;
							act_pos_map.x /= 4;
							act_pos_map.y /= 4;

							auto c = zoom_area_start;
							
							//draw on big map
							auto type = sdk::game::chr::c_char::Instance().get_type(p);
							switch (type)
							{
							case sdk::util::metin_structs::TYPE_ENEMY://mob
								draw_point(main_map_pos, zoom_start, c, act_pos_map, ImColor(255, 0, 0, 255), 2);
								break;
							case sdk::util::metin_structs::TYPE_NPC://npc
								draw_point(main_map_pos, zoom_start, c, act_pos_map, ImColor(255, 255, 0, 255), 2);
								break;
							case sdk::util::metin_structs::TYPE_STONE://metin
								draw_point(main_map_pos, zoom_start, c, act_pos_map, ImColor(255, 255, 255, 255), 2);
								break;
							case sdk::util::metin_structs::TYPE_PC://player
								draw_point(main_map_pos, zoom_start, c, act_pos_map, ImColor(0, 255, 0, 255), 2);
								break;
							default://anything else
								draw_point(main_map_pos, zoom_start, c, act_pos_map, ImColor(255, 0, 255, 255), 2);
								break;
							}
						}
						//self over all
						ImGui::GetWindowDrawList()->AddCircleFilled(
							{
							main_map_pos.x + map_pos.x,
							main_map_pos.y + map_pos.y
							},
							4, ImColor(0, 255, 0, 255));
						ImGui::GetWindowDrawList()->AddCircleFilled(
							{
							zoom_start.x + (map_pos.x - zoom_area_start.x) * 5.f,
							zoom_start.y + (map_pos.y - zoom_area_start.y) * 5.f
							},
							4, ImColor(0, 255, 0, 255));
					}

				}
				else
				{
					if (ImGui::Button("attempt dump map"))
					{
						auto n = new sdk::game::map::c_map_grabber();
						n->work();
					}
					/*auto n = new sdk::game::map::c_map_grabber();
					printf("A %04x\n", (uint32_t)D3DXSaveTextureToFileA);
					n->work();*/
				}
			}
				}else
				{
				ImGui::Text("missing Map folder");
				}
			ImGui::End();
		}
		ImGui::End();
	}
}
bool SliderFloatWithSteps(const char* label, float* v, float v_min, float v_max, float v_step, const char* display_format)
{
	if (!display_format)
		display_format = "%.3f";

	float v_f = *v;
	char value_buf[64] = {};
	const char* value_buf_end = value_buf + ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), display_format, *v);

	bool value_changed = ImGui::SliderFloat(label, &v_f, v_min, v_max, value_buf, 1.0f);
	float remain = fmodf((v_f - v_min), v_step);
	*v = (v_f - remain);
	return value_changed;
}
void main::c_ui::checkbox(std::string label, std::string varhead, std::string varbod, std::function<void()> fn)
{
	
	const auto var = sdk::util::c_config::Instance().get_var(varhead.c_str(), varbod.c_str());
	if (!var) return;
	bool b_var = false;
	if (var->container == "1") b_var = true;
	ImGui::Checkbox(label.c_str(), &b_var);
	if (b_var == true) var->container = "1";
	else var->container = "0";
}

void main::c_ui::slider(std::string label, std::string varhead, std::string varbod, int min, int max, float steps)
{
	const auto var = sdk::util::c_config::Instance().get_var(varhead.c_str(), varbod.c_str());
	if (!var) return;
	float value = 0.f;
	value = std::stof(var->container);
	ImGui::Text(label.c_str()); ImGui::SameLine(0, 15);
	SliderFloatWithSteps(std::string("##").append(label).c_str(), &value, (float)min, (float)max, steps, "%.3f");
	var->container = std::string(std::to_string(value));
}
bool GetImageSize(const char* fn, int* x, int* y)
{
	FILE* f = fopen(fn, "rb");
	if (f == 0) return false;
	fseek(f, 0, SEEK_END);
	long len = ftell(f);
	fseek(f, 0, SEEK_SET);
	if (len < 24) {
		fclose(f);
		return false;
	}
	// Strategy:
	// reading GIF dimensions requires the first 10 bytes of the file
	// reading PNG dimensions requires the first 24 bytes of the file
	// reading JPEG dimensions requires scanning through jpeg chunks
	// In all formats, the file is at least 24 bytes big, so we'll read that always
	unsigned char buf[24]; fread(buf, 1, 24, f);

	// For JPEGs, we need to read the first 12 bytes of each chunk.
	// We'll read those 12 bytes at buf+2...buf+14, i.e. overwriting the existing buf.
	if (buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF && buf[3] == 0xE0 && buf[6] == 'J' && buf[7] == 'F' && buf[8] == 'I' && buf[9] == 'F')
	{
		long pos = 2;
		while (buf[2] == 0xFF)
		{
			if (buf[3] == 0xC0 || buf[3] == 0xC1 || buf[3] == 0xC2 || buf[3] == 0xC3 || buf[3] == 0xC9 || buf[3] == 0xCA || buf[3] == 0xCB) break;
			pos += 2 + (buf[4] << 8) + buf[5];
			if (pos + 12 > len) break;
			fseek(f, pos, SEEK_SET); fread(buf + 2, 1, 12, f);
		}
	}

	fclose(f);

	// JPEG: (first two bytes of buf are first two bytes of the jpeg file; rest of buf is the DCT frame
	if (buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF)
	{
		*y = (buf[7] << 8) + buf[8];
		*x = (buf[9] << 8) + buf[10];
		//cout << *x << endl;
		return true;
	}

	// GIF: first three bytes say "GIF", next three give version number. Then dimensions
	if (buf[0] == 'G' && buf[1] == 'I' && buf[2] == 'F')
	{
		*x = buf[6] + (buf[7] << 8);
		*y = buf[8] + (buf[9] << 8);
		return true;
	}

	// PNG: the first frame is by definition an IHDR frame, which gives dimensions
	if (buf[0] == 0x89 && buf[1] == 'P' && buf[2] == 'N' && buf[3] == 'G' && buf[4] == 0x0D && buf[5] == 0x0A && buf[6] == 0x1A && buf[7] == 0x0A
		&& buf[12] == 'I' && buf[13] == 'H' && buf[14] == 'D' && buf[15] == 'R')
	{
		*x = (buf[16] << 24) + (buf[17] << 16) + (buf[18] << 8) + (buf[19] << 0);
		*y = (buf[20] << 24) + (buf[21] << 16) + (buf[22] << 8) + (buf[23] << 0);
		return true;
	}

	return false;
}
bool main::c_ui::load_texture(const char* filename, PDIRECT3DTEXTURE9* out_texture, int* out_width, int* out_height)
{
	PDIRECT3DTEXTURE9 texture;
	HRESULT hr = D3DXCreateTextureFromFileA(g_pd3dDevice, filename, &texture);
	if (hr != S_OK) return false;
	D3DSURFACE_DESC my_image_desc;
	texture->GetLevelDesc(0, &my_image_desc);
	*out_texture = texture;

	int w, h;
	GetImageSize(filename, &w, &h);

	*out_width = w;
	*out_height = h;

	return true;
}

void main::c_ui::setup()
{
	wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("   "), NULL };
	::RegisterClassEx(&wc);
	hwnd = ::CreateWindow(wc.lpszClassName, _T("     "), WS_OVERLAPPEDWINDOW, 0, 0, 450, 550, NULL, NULL, wc.hInstance, NULL);

	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return;
	}
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);
	if (std::filesystem::exists("Maps") && std::filesystem::is_directory("Maps")) 
		sdk::game::map::c_map_files::Instance().setup();
	
	for (auto a : sdk::game::map::c_map_files::Instance().files)
	{
		sdk::game::map::s_img_data i;
		i.path = a;
		if (!this->load_texture(a.c_str(), &i.texture, &i.width, &i.height)) continue;
		sdk::game::map::c_map_files::Instance().textures.push_back(i);
	}
}

void main::c_ui::work()
{
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		this->render();

		// Rendering
		ImGui::EndFrame();
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

		// Handle loss of D3D9 device
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			ResetDevice();
	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);
}

// Helper functions
using namespace main;
bool CreateDeviceD3D(HWND hWnd)
{
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return false;

	// Create the D3DDevice
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
	//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		return false;

	return true;
}

void CleanupDeviceD3D()
{
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			ResetDevice();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}