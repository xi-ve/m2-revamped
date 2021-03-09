#pragma once
#include <unordered_map>
namespace sdk
{
	namespace game
	{
		namespace map
		{
			class c_map_grabber
			{
			public:

				std::unordered_map<std::string, uint32_t>	existing_textures;
				void										work();
			};
		}
	}
}