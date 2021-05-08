#pragma once
#include <inc.h>
namespace sdk
{
	namespace util
	{
		typedef std::pair<uint32_t, uint32_t> t_size;
		class c_mem : public s<c_mem>
		{
		private:
			
		public:
			size_t		find_size(uint32_t address);
			bool		is_valid(uint32_t address, size_t range);
			t_size		get_section(const char* section, HMODULE base_module);
			t_size		get_section_idx(byte i, HMODULE base_module);
			size_t		get_section_count(HMODULE base_module);
			uint32_t	find_pattern(uint32_t start, const char* sig);
			uint32_t	find_pattern_directed(uintptr_t moduleAdress, const char* signature);
			void		setup();
		};
	}
}