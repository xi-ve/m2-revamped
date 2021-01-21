#pragma once
#include <inc.h>
namespace sdk
{
	namespace util
	{
		class c_address_gathering : public s<c_address_gathering>
		{
		private:
			bool			gather_connection_related();
		public:
			void			setup();
		};
	}
}