#include "ui.h"

#include "imgui/imgui_internal.h"

std::string main::c_ui::to_lower(std::string in)
{
	std::string r = "";
	for (auto a : in) r.push_back(std::tolower(a));
	return r;
}

// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


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
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("log next login"), &sdk::game::accconnector::c_login::Instance().should_grab_details);

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

			ImGui::InputText(XorStr("search item: "), this->input_search_item, 16);
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

				ImGui::BeginChild("##filter_results", {0,0}, true, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);
				
				if (!res.empty())
				{
					for (auto a : res)
					{
						if (a.is_blacklisted) ImGui::PushStyleColor(ImGuiCol_Text, { 255,0,0,255 });
						else ImGui::PushStyleColor(ImGuiCol_Text, { 0,255,0,255 });						
						if (ImGui::Button(a.name.c_str(), {ImGui::GetWindowWidth() - 25, 0}))
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
						ImGui::PopStyleColor(1);
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
			
			ImGui::End();
		}
		ImGui::SetNextWindowDockID(idx, ImGuiCond_::ImGuiCond_FirstUseEver);
		if (ImGui::Begin(XorStr("debug"), 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize))
		{

			ImGui::End();
		}
		ImGui::End();
	}

	ImGui::ShowDemoWindow();
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
	ImGui::SliderFloat(label.c_str(), &value, min, max, "%.2f", steps);
	var->container = std::string(std::to_string(value));
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