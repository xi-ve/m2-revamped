#pragma once
#include <inc.h>
namespace sdk
{
	namespace util
	{
		typedef std::pair<uint32_t, uint32_t> t_size;
		class c_mem : s<c_mem>
		{
		private:
			size_t	find_size(uint32_t address);
		public:
			bool	is_valid(uint32_t address, size_t range);
			t_size	get_section(const char* section, HMODULE base_module);
			void	setup();
		};
	}
}