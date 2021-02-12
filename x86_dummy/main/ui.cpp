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


std::string main::c_ui::to_lower(std::string in)
{
	std::string r = "";
	for (auto a : in) r.push_back(std::tolower(a));
	return r;
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
	auto val = sdk::util::c_config::Instance().get_var(varhead.c_str(), varbod.c_str());
	if (!val) { sdk::util::c_log::Instance().duo("cant get var\n"); return; }
	if (strstr(val->container.c_str(), "1")) sw = 1;

	nk_checkbox_label(ctx, label.c_str(), &sw);

	if (sw) val->container = "1";
	else val->container = "0";
}

void main::c_ui::slider(std::string label, std::string varhead, std::string varbod, int min, int max, float steps)
{
	auto val = sdk::util::c_config::Instance().get_var(varhead.c_str(), varbod.c_str());
	if (!val) { sdk::util::c_log::Instance().duo("cant get var\n"); return; }
	float asflt = std::stof(val->container.c_str());

	nk_layout_row_dynamic(ctx, 16, 2);
	nk_label(ctx, sdk::util::c_log::Instance().string("%s - %.2f", label.c_str(), asflt), NK_TEXT_ALIGN_LEFT);
	nk_slider_float(ctx, (float)min, &asflt, (float)max, steps);

	val->container = std::to_string(asflt);
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
	int maxlen = 0;
	/* Win32 */
	memset(&wc, 0, sizeof(wc));
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandleW(0);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"e";
	atom = RegisterClassW(&wc);

	AdjustWindowRectEx(&rect, style, FALSE, exstyle);
	wnd = CreateWindowExW(exstyle, wc.lpszClassName, L"r",
						  style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
						  rect.right - rect.left, rect.bottom - rect.top,
						  NULL, NULL, wc.hInstance, NULL);

	dc = GetDC(wnd);
	/* GUI */
	font = nk_gdifont_create("Consolas", 16);
	ctx = nk_gdi_init(font, dc, WINDOW_WIDTH, WINDOW_HEIGHT);
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
				nk_layout_row_dynamic(ctx, 20, 2);
				nk_checkbox_label(ctx, XorStr("con-info"), &this->debug_serverdata);
				nk_checkbox_label(ctx, XorStr("actor-info"), &this->debug_actors);
				nk_checkbox_label(ctx, XorStr("item-info"), &this->debug_items);
				nk_checkbox_label(ctx, XorStr("item-conf"), &this->debug_item_edit);

				nk_checkbox_label(ctx, XorStr("log-next-login"), &sdk::game::accconnector::c_login::Instance().should_grab_details);
				this->checkbox(XorStr("login"), XorStr("login"), XorStr("enable"));

				this->checkbox(XorStr("waithack"), XorStr("waithack"), XorStr("toggle"));
				this->checkbox(XorStr("metins"), XorStr("waithack"), XorStr("metins"));
				this->checkbox(XorStr("mobs"), XorStr("waithack"), XorStr("mobs"));
				this->checkbox(XorStr("on_attack"), XorStr("waithack"), XorStr("on_attack"));
				this->checkbox(XorStr("bp_on_attack"), XorStr("waithack"), XorStr("bp_on_attack"));
				this->checkbox(XorStr("dmg-boost"), XorStr("waithack"), XorStr("boost"));
				this->checkbox(XorStr("wallhack"), XorStr("misc"), XorStr("wallhack"));

				this->checkbox(XorStr("pickup"), XorStr("pickup"), XorStr("toggle"));
				nk_checkbox_label(ctx, XorStr("only-blacklist"), &sdk::game::c_pickup::Instance().item_conf.only_pick_blacklist);
				this->slider(XorStr("pick-range"), XorStr("pickup"), XorStr("range"), 300, 20000, 100.f);
				this->slider(XorStr("pick-delay"), XorStr("pickup"), XorStr("delay"), 0, 1000, 5.f);

				this->slider(XorStr("range"), XorStr("waithack"), XorStr("range"), 300, 20000, 100.f);
				this->slider(XorStr("targets"), XorStr("waithack"), XorStr("targets"), 1, 25, 1.f);
				this->slider(XorStr("speed"), XorStr("waithack"), XorStr("speed"), 1, 100, 1.f);
				this->slider(XorStr("anchor"), XorStr("waithack"), XorStr("anchor"), 1000, 4000, 100.f);

				auto main_act = sdk::game::chr::c_char::Instance().get_main_actor();

				nk_layout_row_dynamic(ctx, 20, 2);
				nk_label(ctx, sdk::util::c_log::Instance().string(XorStr("alive : %i"), sdk::game::chr::c_char::Instance().get_alive().size()), NK_TEXT_LEFT);

				if (this->debug_item_edit && sdk::game::item::c_item_manager::Instance().did_grab)
				{
					nk_layout_row_dynamic(ctx, 28, 2);

					nk_label(ctx, "search:", NK_TEXT_LEFT);
					nk_edit_string_zero_terminated(ctx, NK_EDIT_SIMPLE, this->input_search_item, 26, nk_filter_default);

					if (this->input_search_item)
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

						if (res.size())
						{
							nk_layout_row_dynamic(ctx, 300, 1);

							if (nk_group_begin(ctx, "items", 0))
							{								
								nk_layout_row_dynamic(ctx, 20, 2);
								for (int i = 0; i < (int)res.size(); ++i)
								{
									int selected = 0;

									auto obj = res[i];
									if (obj.name.empty()) continue;

									auto cchar = obj.name.data();

									if (nk_select_label(ctx, (const char*)cchar, NK_TEXT_LEFT, selected))
									{
										if (obj.is_blacklisted)
										{
											sdk::game::c_pickup::Instance().unblacklist(obj.vnum);
											sdk::game::c_pickup::Instance().add_whitelist(obj.vnum);											
										}
										else
										{
											sdk::game::c_pickup::Instance().unwhitelist(obj.vnum); 
											sdk::game::c_pickup::Instance().add_blacklist(obj.vnum);											
										}
									}
									if (obj.is_blacklisted) nk_label(ctx, "blacklisted", NK_TEXT_RIGHT);
									else nk_label(ctx, "whitelisted", NK_TEXT_RIGHT);
								}
							} nk_group_end(ctx);

							//if (nk_list_view_begin(ctx, &view, "items", NK_WINDOW_BORDER | NK_WINDOW_SCROLL_AUTO_HIDE | NK_WINDOW_DYNAMIC, 20, 15))
							//{
							//	for (auto a = 0; a < (int)res.size(); a++)
							//	{
							//		auto b = res[a];
							//		if (b.name.empty()) continue;

							//		nk_layout_row_dynamic(ctx, 20, 2);

							//		int selected = false;
							//		if (nk_select_label(ctx, b.name.c_str(), NK_TEXT_LEFT, selected))
							//		{
							//			//swap pickmode
							//			sdk::util::c_log::Instance().duo("[ switching => vnum: %i, name: %s, blacklisted: %i ]\n", b.vnum, b.name.c_str(), b.is_blacklisted);

							//			if (b.is_blacklisted)
							//			{
							//				sdk::game::c_pickup::Instance().add_whitelist(b.vnum);
							//				sdk::game::c_pickup::Instance().unblacklist(b.vnum);
							//			}
							//			else
							//			{
							//				sdk::game::c_pickup::Instance().add_blacklist(b.vnum);
							//				sdk::game::c_pickup::Instance().unwhitelist(b.vnum);
							//			}
							//		}

							//		if (b.is_blacklisted) nk_label(ctx, "blacklisted", NK_TEXT_RIGHT);
							//		else nk_label(ctx, "whitelisted", NK_TEXT_RIGHT);
							//	}
							//	nk_list_view_end(&view);
							//}
						}
					}
				}

				if (main_act && this->debug_items)
				{
					auto it = sdk::game::item::c_item_gather::Instance().items;
					if (it.size())
					{
						auto main_pos = sdk::game::chr::c_char::Instance().get_pos(main_act);
						nk_layout_row_dynamic(ctx, 20, 1);
						for (auto&& a : it)
						{
							auto dst_to_item = 0.f; dst_to_item = sdk::game::chr::c_char::Instance().get_distance(main_pos, a.pos);
							nk_label(ctx, sdk::util::c_log::Instance().string("item vid: %04x, owner: %s, dst: %.2f", a.vid, a.owner.c_str(), dst_to_item), NK_TEXT_LEFT);
						}
					}
				}

				auto network_base = sdk::game::c_utils::Instance().baseclass_networking();
				auto account_connector_base = sdk::game::c_utils::Instance().baseclass_account_connector();
				if (network_base && account_connector_base)
				{
					if (this->debug_actors)
					{
						auto alive_actors = sdk::game::chr::c_char::Instance().get_alive();
						if (alive_actors.size())
						{
							nk_layout_row_dynamic(ctx, 20, 1);

							auto main_actor = sdk::game::chr::c_char::Instance().get_main_actor();
							auto lc_pos = sdk::game::chr::c_char::Instance().get_pos(main_actor);
							auto lc_name = sdk::game::chr::c_char::Instance().get_name(main_actor);
							auto lc_type = sdk::game::chr::c_char::Instance().get_type(main_actor);
							auto dead = sdk::game::chr::c_char::Instance().is_dead_actor(main_actor);

							nk_label(ctx, sdk::util::c_log::Instance().string(XorStr("self %04x %i, %s, %i, %i %i %i"), main_actor, dead, lc_name.c_str(), lc_type, (int)lc_pos.x, (int)lc_pos.y, (int)lc_pos.z), NK_TEXT_LEFT);

							for (auto a : alive_actors)
							{
								auto p = (uint32_t)a.second;
								if (!p) continue;
								auto name = sdk::game::chr::c_char::Instance().get_name(p);
								auto type = sdk::game::chr::c_char::Instance().get_type(p);
								dead = sdk::game::chr::c_char::Instance().is_dead_actor(main_actor);
								nk_label(ctx, sdk::util::c_log::Instance().string(XorStr("actor %04x %i, %s, %i"), p, dead, name.c_str(), type), NK_TEXT_LEFT);
							}
						}
						else
						{
							nk_layout_row_dynamic(ctx, 20, 1);
							nk_label(ctx, sdk::util::c_log::Instance().string(XorStr("no alive actors!")), NK_TEXT_LEFT);
						}
					}
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

				nk_layout_row_dynamic(ctx, 30, 3);
				if (nk_button_label(ctx, XorStr("set&connect")))
				{
					sdk::game::accconnector::c_login::Instance().set_details();
				}
				if (nk_button_label(ctx, XorStr("connect")))
				{
					sdk::game::accconnector::c_login::Instance().set_connect();
				}
				if (nk_button_label(ctx, XorStr("attack-close")))
				{
					float last_dst = 9999.f; uint32_t target = 0;
					auto alive = sdk::game::chr::c_char::Instance().get_alive();

					auto main_actor = sdk::game::chr::c_char::Instance().get_main_actor();
					auto lc_pos = sdk::game::chr::c_char::Instance().get_pos(main_actor);

					for (auto a : alive)
					{
						auto p = (uint32_t)a.second;
						if (!p) continue;
						if (p == main_actor) continue;
						auto pos = sdk::game::chr::c_char::Instance().get_pos(p);
						if (!pos.valid()) continue;
						if (sdk::game::chr::c_char::Instance().is_dead_actor(p)) continue;
						if (!sdk::game::chr::c_char::Instance().is_attackable(p)) continue;
						auto dst_to_us = sdk::game::chr::c_char::Instance().get_distance(lc_pos, pos);
						if (dst_to_us >= 300) continue;
						if (dst_to_us < last_dst)
						{
							last_dst = dst_to_us;
							target = sdk::game::chr::c_char::Instance().get_vid(p);
						}
					}
					if (target)
					{
						sdk::game::func::c_funcs::Instance().f_SendAttackPacket(network_base, 0, target);
					}
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
