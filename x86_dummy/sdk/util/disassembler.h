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
			t_asm_res	get_movs(uint32_t address, size_t size = 0, size_t min = 0, size_t max = 0);
			t_asm_res	get_pushes(uint32_t address, size_t size = 0, size_t min = 0xFFF);
			t_asm_res	get_addrs(uint32_t address, size_t size = 0, size_t min = 0xFFF);
			t_asm_res	get_calls(uint32_t address, size_t size = 0, size_t min = 0xFFF, BOOL skip_py_exports = false);
			t_asm_res	get_jumps(uint32_t address, size_t size = 0, size_t min = 0xFFF);
			t_asm_res	get_offsets(uint32_t address, size_t size = 0, size_t min = 0x10, size_t max = 0xFFFF);
			t_asm_res	get_custom(uint32_t address, size_t size = 0, size_t min = 0x10, size_t max = 0xFFFF, std::vector<std::string> opcodes = {"push", "lea"});
			t_asm_raw	dump_asm(uint32_t address, size_t size = 0);
			uint32_t	convert_rip(uint32_t start, int offset);
		};
	}
}