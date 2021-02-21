#pragma once
#include <inc.h>
namespace sdk
{
	namespace game
	{
		//actor related things
		class c_utils : public s<c_utils>
		{
		public:
			uint32_t	baseclass_networking();
			uint32_t	baseclass_account_connector();
			uint32_t	baseclass_character_manager();
			uint32_t	baseclass_python_player();
			uint32_t	baseclass_event_handler();
			uint32_t	baseclass_python_item();
			uint32_t	baseclass_item_manager();
			uint32_t	baseclass_background();
		};
	}
}