#pragma once
#include <inc.h>
namespace sdk
{
	namespace game
	{
		namespace map
		{
			struct s_img_data
			{
				std::string			path = "";
				PDIRECT3DTEXTURE9	texture = {};
				int					width = 0;
				int					height = 0;
			};
			class c_map_files : public s<c_map_files>
			{
			public:
				s_img_data*						find(std::string name);
			public:
				std::vector<std::string>		files;
				std::vector<s_img_data>			textures;
				void							setup();				
			};
		}
	}
}