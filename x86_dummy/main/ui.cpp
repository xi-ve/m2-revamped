#include "ui.h"
#define INCLUDE_STYLE 1
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_D3D9_IMPLEMENTATION
#include <nuklear/nuklear.h>
#include <nuklear/nuklear_d3d9.h>

using namespace std::chrono;

static IDirect3DDevice9* device;
static IDirect3DDevice9Ex* deviceEx;
static D3DPRESENT_PARAMETERS present;
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

		case WM_SIZE:
			if (device)
			{
				UINT width = LOWORD(lparam);
				UINT height = HIWORD(lparam);
				if (width != 0 && height != 0 &&
					(width != present.BackBufferWidth || height != present.BackBufferHeight))
				{
					nk_d3d9_release();
					present.BackBufferWidth = width;
					present.BackBufferHeight = height;
					HRESULT hr = IDirect3DDevice9_Reset(device, &present);
					NK_ASSERT(SUCCEEDED(hr));
					nk_d3d9_resize(width, height);
				}
			}
			break;
	}

	if (nk_d3d9_handle_event(wnd, msg, wparam, lparam))
		return 0;

	return DefWindowProcW(wnd, msg, wparam, lparam);
}

void main::c_ui::create_device(HWND d)
{
	HRESULT hr;

	present.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	present.BackBufferWidth = 350;
	present.BackBufferHeight = 550;
	present.BackBufferFormat = D3DFMT_X8R8G8B8;
	present.BackBufferCount = 1;
	present.MultiSampleType = D3DMULTISAMPLE_NONE;
	present.SwapEffect = D3DSWAPEFFECT_DISCARD;
	present.hDeviceWindow = wnd;
	present.EnableAutoDepthStencil = TRUE;
	present.AutoDepthStencilFormat = D3DFMT_D24S8;
	present.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	present.Windowed = TRUE;

	{/* first try to create Direct3D9Ex device if possible (on Windows 7+) */
		typedef HRESULT WINAPI Direct3DCreate9ExPtr(UINT, IDirect3D9Ex**);
		Direct3DCreate9ExPtr* Direct3DCreate9Ex = (Direct3DCreate9ExPtr*)GetProcAddress(GetModuleHandleA("d3d9.dll"), "Direct3DCreate9Ex");
		if (Direct3DCreate9Ex) {
			IDirect3D9Ex* d3d9ex;
			if (SUCCEEDED(Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d9ex))) {
				hr = IDirect3D9Ex_CreateDeviceEx(d3d9ex, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd,
												 D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE | D3DCREATE_FPU_PRESERVE,
												 &present, NULL, &deviceEx);
				if (SUCCEEDED(hr)) {
					device = (IDirect3DDevice9*)deviceEx;
				}
				else {
				 /* hardware vertex processing not supported, no big deal
				 retry with software vertex processing */
					hr = IDirect3D9Ex_CreateDeviceEx(d3d9ex, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd,
													 D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE | D3DCREATE_FPU_PRESERVE,
													 &present, NULL, &deviceEx);
					if (SUCCEEDED(hr)) {
						device = (IDirect3DDevice9*)deviceEx;
					}
				}
				IDirect3D9Ex_Release(d3d9ex);
			}
		}
	}

	if (!device) {
		/* otherwise do regular D3D9 setup */
		IDirect3D9* d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

		hr = IDirect3D9_CreateDevice(d3d9, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd,
									 D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE | D3DCREATE_FPU_PRESERVE,
									 &present, &device);
		if (FAILED(hr)) {
			/* hardware vertex processing not supported, no big deal
			retry with software vertex processing */
			hr = IDirect3D9_CreateDevice(d3d9, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd,
										 D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE | D3DCREATE_FPU_PRESERVE,
										 &present, &device);
			NK_ASSERT(SUCCEEDED(hr));
		}
		IDirect3D9_Release(d3d9);
	}
}

void main::c_ui::setup()
{
	memset(&wc, 0, sizeof(wc));
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandleW(0);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"      ";
	RegisterClassW(&wc);

	sdk::util::c_log::Instance().duo("[ starting ui ]\n");

	AdjustWindowRectEx(&rect, style, FALSE, exstyle);

	wnd = CreateWindowExW(exstyle, wc.lpszClassName, L"",
						  style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
						  rect.right - rect.left, rect.bottom - rect.top,
						  NULL, NULL, wc.hInstance, NULL);

	this->create_device(wnd);

	ctx = nk_d3d9_init(device, 350, 550);
	{struct nk_font_atlas* atlas;
	nk_d3d9_font_stash_begin(&atlas);
	nk_d3d9_font_stash_end(); }

#ifdef INCLUDE_STYLE
	struct nk_color table[NK_COLOR_COUNT];
	table[NK_COLOR_TEXT] = nk_rgba(210, 210, 210, 255);
	table[NK_COLOR_WINDOW] = nk_rgba(57, 67, 71, 215);
	table[NK_COLOR_HEADER] = nk_rgba(51, 51, 56, 220);
	table[NK_COLOR_BORDER] = nk_rgba(46, 46, 46, 255);
	table[NK_COLOR_BUTTON] = nk_rgba(48, 83, 111, 255);
	table[NK_COLOR_BUTTON_HOVER] = nk_rgba(58, 93, 121, 255);
	table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(63, 98, 126, 255);
	table[NK_COLOR_TOGGLE] = nk_rgba(50, 58, 61, 255);
	table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 53, 56, 255);
	table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(48, 83, 111, 255);
	table[NK_COLOR_SELECT] = nk_rgba(57, 67, 61, 255);
	table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(48, 83, 111, 255);
	table[NK_COLOR_SLIDER] = nk_rgba(50, 58, 61, 255);
	table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(48, 83, 111, 245);
	table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
	table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
	table[NK_COLOR_PROPERTY] = nk_rgba(50, 58, 61, 255);
	table[NK_COLOR_EDIT] = nk_rgba(50, 58, 61, 225);
	table[NK_COLOR_EDIT_CURSOR] = nk_rgba(210, 210, 210, 255);
	table[NK_COLOR_COMBO] = nk_rgba(50, 58, 61, 255);
	table[NK_COLOR_CHART] = nk_rgba(50, 58, 61, 255);
	table[NK_COLOR_CHART_COLOR] = nk_rgba(48, 83, 111, 255);
	table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
	table[NK_COLOR_SCROLLBAR] = nk_rgba(50, 58, 61, 255);
	table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(48, 83, 111, 255);
	table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
	table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
	table[NK_COLOR_TAB_HEADER] = nk_rgba(48, 83, 111, 255);
	nk_style_from_table(ctx, table);
#endif
	sdk::util::c_log::Instance().duo("[ ui setup complete ]\n");
}

void main::c_ui::work()
{
	bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
	while (running)
	{
		/* Input */
		MSG msg;
		nk_input_begin(ctx);
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				running = 0;
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		nk_input_end(ctx);

		/* GUI */
		if (nk_begin(ctx, "M2++", nk_rect(50, 50, 350, 550),
			NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
			NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
		{
			enum { EASY, HARD };

			nk_layout_row_static(ctx, 30, 80, 1);
			if (nk_button_label(ctx, "button"))
				fprintf(stdout, "button pressed\n");
			nk_layout_row_dynamic(ctx, 30, 2);
			nk_layout_row_dynamic(ctx, 22, 1);

			nk_layout_row_dynamic(ctx, 20, 1);
			nk_label(ctx, "background:", NK_TEXT_LEFT);
			nk_layout_row_dynamic(ctx, 25, 1);
			if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx), 400))) 
			{
				nk_layout_row_dynamic(ctx, 120, 1);
				bg = nk_color_picker(ctx, bg, NK_RGBA);
				nk_layout_row_dynamic(ctx, 25, 1);
				bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
				bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
				bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
				bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
				nk_combo_end(ctx);
			}
		}
		nk_end(ctx);
		{
			HRESULT hr;
			hr = IDirect3DDevice9_Clear(device, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
										D3DCOLOR_COLORVALUE(bg.r, bg.g, bg.b, bg.a), 0.0f, 0);
			NK_ASSERT(SUCCEEDED(hr));

			hr = IDirect3DDevice9_BeginScene(device);
			NK_ASSERT(SUCCEEDED(hr));
			nk_d3d9_render(NK_ANTI_ALIASING_ON);
			hr = IDirect3DDevice9_EndScene(device);
			NK_ASSERT(SUCCEEDED(hr));

			if (deviceEx) {
				hr = IDirect3DDevice9Ex_PresentEx(deviceEx, NULL, NULL, NULL, NULL, 0);
			}
			else {
				hr = IDirect3DDevice9_Present(device, NULL, NULL, NULL, NULL);
			}
			if (hr == D3DERR_DEVICELOST || hr == D3DERR_DEVICEHUNG || hr == D3DERR_DEVICEREMOVED)
			{
				break;
			}
			else if (hr == S_PRESENT_OCCLUDED) {
			 /* window is not visible, so vsync won't work. Let's sleep a bit to reduce CPU usage */
				std::this_thread::sleep_for(10ms);
			}
			NK_ASSERT(SUCCEEDED(hr));
		}
	}
	nk_d3d9_shutdown();
	if (deviceEx) IDirect3DDevice9Ex_Release(deviceEx);
	else IDirect3DDevice9_Release(device);
	UnregisterClassW(wc.lpszClassName, wc.hInstance);
}
