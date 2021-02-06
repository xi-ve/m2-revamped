#pragma once
#include <inc.h>
namespace sdk
{
	namespace game
	{
		namespace item
		{
			class c_item_manager : public s<c_item_manager>
			{
			public:
				std::map<uint32_t, uint32_t*>	item_map;
				std::map<uint32_t, std::string> item_names;
			public:
				bool	did_grab = false;
				void	setup();
				bool	grab();
			};
		}
	}
}