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
		};
	}
}