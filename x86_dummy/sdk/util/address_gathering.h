#pragma once
#include <inc.h>
namespace sdk
{
	namespace util
	{
		class c_address_gathering : public s<c_address_gathering>
		{
		private:
			uint32_t		get_func(const char* string_ref);
		public:
			void			setup();
		};
	}
}