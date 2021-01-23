#pragma once
#include <inc.h>
namespace sdk
{
	namespace util
	{
		class c_address_gathering : public s<c_address_gathering>
		{
		private:
			bool			error_out(int line);
		private:
			uint32_t		find_singleton_or_instance(uint32_t f);
			bool			gather_connection_related();
		public:
			void			setup();
		};
	}
}