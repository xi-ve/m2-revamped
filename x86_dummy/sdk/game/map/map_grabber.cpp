#include "map_grabber.h"

#include <comcat.h>
#include <game/offsets.h>

namespace d3d8_x
{
#include <Windows.h>
#include <cstdlib>
	
#include "d3d8/d3d8.h"
#pragma comment(lib, "d3d8/d3d8.lib")
#include "d3d8/d3dx8.h"
#pragma comment(lib, "d3d8/d3dx8.lib")
	extern void trickery(DWORD t);
}

void do_work(DWORD t)
{
	auto res = d3d8_x::D3DXSaveTextureToFileA("map_cur.dds", d3d8_x::D3DXIFF_DDS, (d3d8_x::LPDIRECT3DTEXTURE8)t, 0);
	printf("res: %04x\n", res);
}

void d3d8_x::trickery(DWORD t)
{
	auto py_map = *(uint32_t*)(sdk::game::pointer_offsets::off_CPythonMiniMap);
	if (py_map)
	{
		auto graphic_container = *(uint32_t*)(py_map + 0x2c0);
		if (graphic_container)
		{
			auto tex_ = *(uint32_t*)(graphic_container + 0x3c);
			if (tex_)
			{
				printf("saved %i %i %s\n", *(int*)(tex_ + 0x24), *(int*)(tex_ + 0x28), *(const char**)(graphic_container + 0xc));
				::do_work(t);
			}
			else printf("no has map sadge\n");
		}
	}
}

void sdk::game::map::c_map_grabber::work()
{
	d3d8_x::trickery(0);
}
