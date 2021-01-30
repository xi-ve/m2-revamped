#include "ui.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define WINDOW_WIDTH 350
#define WINDOW_HEIGHT 550

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_GDI_IMPLEMENTATION

#pragma comment(lib, "Msimg32.lib")

#include <nuklear/nuklear.h>
#include <nuklear/nuklear_d3d9.h>

using namespace std::chrono;

static nk_context* ctx;
static nk_colorf bg;

static LRESULT CALLBACK
WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	if (nk_gdi_handle_event(wnd, msg, wparam, lparam))
		return 0;

	return DefWindowProcW(wnd, msg, wparam, lparam);
}


void main::c_ui::create_device(HWND d)
{
	GdiFont* font;
	struct nk_context* ctx;

	WNDCLASSW wc;
	ATOM atom;
	RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	DWORD style = WS_OVERLAPPEDWINDOW;
	DWORD exstyle = WS_EX_APPWINDOW;
	HWND wnd;
	HDC dc;
	int running = 1;
	int needs_refresh = 1;

	/* Win32 */
	memset(&wc, 0, sizeof(wc));
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandleW(0);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"   ";
	atom = RegisterClassW(&wc);

	AdjustWindowRectEx(&rect, style, FALSE, exstyle);
	wnd = CreateWindowExW(exstyle, wc.lpszClassName, L"  ",
						  style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
						  rect.right - rect.left, rect.bottom - rect.top,
						  NULL, NULL, wc.hInstance, NULL);
	dc = GetDC(wnd);

	/* GUI */
	font = nk_gdifont_create("Consolas", 14);
	ctx = nk_gdi_init(font, dc, WINDOW_WIDTH, WINDOW_HEIGHT);

	/* style.c */
#ifdef INCLUDE_STYLE
/*set_style(ctx, THEME_WHITE);*/
/*set_style(ctx, THEME_RED);*/
/*set_style(ctx, THEME_BLUE);*/
/*set_style(ctx, THEME_DARK);*/
#endif
}

void main::c_ui::checkbox(std::string label, std::string varhead, std::string varbod, std::function<void()> fn)
{
	int sw = false;
	auto val = sdk::util::c_config::Instance().get_var(label.c_str(), varhead.c_str());
	if (!val) { sdk::util::c_log::Instance().duo("cant get var\n"); return; }
	if (strstr(val->container.c_str(), "1")) sw = 1;

	nk_checkbox_label(ctx, label.c_str(), &sw);

	if (sw) val->container = "1";
	else val->container = "0";
}

void main::c_ui::setup()
{

}

void main::c_ui::work()
{
	GdiFont* font;

	WNDCLASSW wc;
	ATOM atom;
	RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	DWORD style = WS_OVERLAPPEDWINDOW;
	DWORD exstyle = WS_EX_APPWINDOW;
	HWND wnd;
	HDC dc;
	int running = 1;
	int needs_refresh = 1;

	/* Win32 */
	memset(&wc, 0, sizeof(wc));
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandleW(0);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"   ";
	atom = RegisterClassW(&wc);

	AdjustWindowRectEx(&rect, style, FALSE, exstyle);
	wnd = CreateWindowExW(exstyle, wc.lpszClassName, L"  ",
						  style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
						  rect.right - rect.left, rect.bottom - rect.top,
						  NULL, NULL, wc.hInstance, NULL);
	dc = GetDC(wnd);

	/* GUI */
	font = nk_gdifont_create("Consolas", 16);
	ctx = nk_gdi_init(font, dc, WINDOW_WIDTH, WINDOW_HEIGHT);

	/* style.c */
#ifdef INCLUDE_STYLE
/*set_style(ctx, THEME_WHITE);*/
/*set_style(ctx, THEME_RED);*/
/*set_style(ctx, THEME_BLUE);*/
/*set_style(ctx, THEME_DARK);*/
#endif

	sdk::util::c_log::Instance().duo(XorStr("[ ui setup complete ]\n"));

	while (running)
	{
		bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
		/* Input */
		MSG msg;
		nk_input_begin(ctx);
		if (needs_refresh == 0) {
			if (GetMessageW(&msg, NULL, 0, 0) <= 0)
				running = 0;
			else {
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
			needs_refresh = 1;
		}
		else needs_refresh = 0;

		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				running = 0;
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
			needs_refresh = 1;
		}
		nk_input_end(ctx);

		/* GUI */
		if (nk_begin(ctx, XorStr("M2++"), nk_rect(0, 0, 350, 550), 0))
		{
			if (sdk::util::c_address_gathering::Instance().done)
			{
				nk_layout_row_dynamic(ctx, 20, 1);
				nk_checkbox_label(ctx, XorStr("debug-con"), (nk_bool*)&this->debug_serverdata);
				nk_layout_row_dynamic(ctx, 20, 1);
				nk_checkbox_label(ctx, XorStr("log-next-login"), (nk_bool*)&sdk::game::accconnector::c_login::Instance().should_grab_details);
				nk_layout_row_dynamic(ctx, 20, 1);
				this->checkbox(XorStr("login"), XorStr("enable"), XorStr("enable"));
				nk_layout_row_dynamic(ctx, 20, 1);
				nk_label(ctx, sdk::util::c_log::Instance().string(XorStr("alive : %i"), sdk::game::chr::c_char::Instance().get_alive().size()), NK_TEXT_LEFT);
				nk_layout_row_dynamic(ctx, 20, 1);
				nk_label(ctx, sdk::util::c_log::Instance().string(XorStr("dead : %i"), sdk::game::chr::c_char::Instance().get_dead().size()), NK_TEXT_LEFT);

				auto network_base = sdk::game::c_utils::Instance().baseclass_networking();
				auto account_connector_base = sdk::game::c_utils::Instance().baseclass_account_connector();
				if (network_base && account_connector_base)
				{
					if (this->debug_serverdata)
					{
						char net_phase[16] = "\0";
						memcpy(net_phase, (void*)(network_base + sdk::game::connection_offsets::off_NETWORKING_PHASE), 16);
						char cur_username[32] = "\0";
						memcpy(cur_username, (void*)(account_connector_base + sdk::game::connection_offsets::off_USERNAME), 32);
						char cur_password[32] = "\0";
						memcpy(cur_password, (void*)(account_connector_base + sdk::game::connection_offsets::off_PASSWORD), 32);
						char cur_passcode[32] = "\0";
						if (sdk::game::connection_offsets::off_PASSCODE) memcpy(cur_passcode, (void*)(account_connector_base + sdk::game::connection_offsets::off_PASSCODE), 32);
						char cur_ip[32] = "\0";
						memcpy(cur_ip, (void*)(account_connector_base + sdk::game::connection_offsets::off_IP), 32);

						if (!cur_ip || !sdk::util::c_fn_discover::Instance().is_ascii(cur_ip))
						{
							auto r = *(std::string*)(account_connector_base + sdk::game::connection_offsets::off_IP);
							strcpy(cur_ip, r.c_str());
						}

						auto cur_port = *(uint32_t*)(account_connector_base + sdk::game::connection_offsets::off_PORT);

						auto sock_auth_srv = *(sockaddr_in*)(account_connector_base + sdk::game::connection_offsets::off_SOCKET);
						auto net_ip = inet_ntoa(sock_auth_srv.sin_addr);
						auto port = htons(sock_auth_srv.sin_port);

						if (net_phase)
						{
							auto self_ptr = sdk::game::chr::c_char::Instance().get_main_actor();
							nk_layout_row_dynamic(ctx, 26, 1);
							nk_label(ctx, sdk::util::c_log::Instance().string(XorStr("self    ptr  : %04x"), self_ptr), NK_TEXT_LEFT);
							nk_label(ctx, sdk::util::c_log::Instance().string(XorStr("network ptr  : %04x"), network_base), NK_TEXT_LEFT);
							nk_label(ctx, sdk::util::c_log::Instance().string(XorStr("network phase: %s"), net_phase), NK_TEXT_LEFT);
							nk_label(ctx, sdk::util::c_log::Instance().string(XorStr("username     : %s"), cur_username), NK_TEXT_LEFT);
							nk_label(ctx, sdk::util::c_log::Instance().string(XorStr("password     : %s"), cur_password), NK_TEXT_LEFT);
							if (sdk::game::connection_offsets::off_PASSCODE) nk_label(ctx, sdk::util::c_log::Instance().string(XorStr("passcode     : %s"), cur_passcode), NK_TEXT_LEFT);
							nk_label(ctx, sdk::util::c_log::Instance().string(XorStr("GAME IP      : %s"), cur_ip), NK_TEXT_LEFT);
							nk_label(ctx, sdk::util::c_log::Instance().string(XorStr("GAME PORT    : %i"), cur_port), NK_TEXT_LEFT);
							nk_label(ctx, sdk::util::c_log::Instance().string(XorStr("CH IP        : %s"), net_ip), NK_TEXT_LEFT);
							nk_label(ctx, sdk::util::c_log::Instance().string(XorStr("CH PORT      : %i"), port), NK_TEXT_LEFT);
						}
					}
				}

				nk_layout_row_dynamic(ctx, 30, 2);
				if (nk_button_label(ctx, XorStr("set&connect")))
				{
					sdk::game::accconnector::c_login::Instance().set_details();
				}
				if (nk_button_label(ctx, XorStr("connect")))
				{
					sdk::game::accconnector::c_login::Instance().set_connect();
				}
			}
		}
		nk_end(ctx);
		/* Draw */
		nk_gdi_render(nk_rgb(30, 30, 30));
	}

	nk_gdifont_del(font);
	ReleaseDC(wnd, dc);
	UnregisterClassW(wc.lpszClassName, wc.hInstance);
}
