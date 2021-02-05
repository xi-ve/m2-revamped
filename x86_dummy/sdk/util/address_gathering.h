#pragma once
#include <inc.h>
namespace sdk
{
	namespace util
	{
		class c_address_gathering : public s<c_address_gathering>
		{
		private:
			bool					error_out(int line);
		private:
			std::vector<uint32_t>	find_custom_range(uint32_t adr, uint32_t min, uint32_t max, uint32_t steps, std::vector<std::string> instructions, bool from_max = false);
			uint32_t				find_singleton_or_instance(uint32_t f);
			bool					gather_connection_related();
			bool					gather_actor_related();
			bool					gather_item_related();
		public:
			bool					done = false;
			void					setup();
			bool					check_baseclasses();
		};
	}
}