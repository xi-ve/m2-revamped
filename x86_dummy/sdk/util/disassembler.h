#pragma once
#include <inc.h>
namespace sdk
{
	namespace util
	{
		typedef std::vector<uint32_t>		t_asm_res;
		typedef std::vector<std::string>	t_asm_raw;
		class c_disassembler : public s<c_disassembler>
		{
		private:
			t_asm_raw	get_asm(uint32_t address, size_t size);
		public:
			void		setup();
			t_asm_res	get_pushes(uint32_t address, size_t size = 0, size_t min = 0xFFF);
		};
	}
}