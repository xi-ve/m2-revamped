#pragma once
#include <inc.h>
namespace sdk
{
	namespace game
	{
		namespace item
		{
			struct s_item
			{
				std::string		owner = "";
				uint32_t		address = 0;
				uint32_t		vid = 0;
				uint32_t		vnum = 0;
				chr::vec		pos = {};
				ULONGLONG		drop_time = 0;
			};
			class c_item_gather : public s<c_item_gather>
			{
			public:
				std::vector<s_item>		items; 
				void					setup();
				void					work();
			};
		}
	}
}