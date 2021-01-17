#include "disassembler.h"

void sdk::util::c_disassembler::setup()
{
}

sdk::util::t_asm_raw sdk::util::c_disassembler::get_asm(uint32_t address, size_t size)
{
	auto ret = sdk::util::t_asm_raw();

	uint8_t cbytes[0x2048];
	memcpy((void*)cbytes, (void*)(address), size);
	if (!cbytes) return ret;

	csh handle; cs_insn* insn; int count;
	if (cs_open(CS_ARCH_X86, CS_MODE_32, &handle) != CS_ERR_OK) return ret;
	count = cs_disasm(handle, cbytes, size - 1, address, 0, &insn);
	if (count > 0)
	{
		for (auto h = 0; h < count; h++) ret.push_back(sdk::util::c_log::Instance().string("%s , %s", insn[h].mnemonic, insn[h].op_str));
		cs_free(insn, count);
	}
	cs_close(&handle);

	return ret;
}

sdk::util::t_asm_res sdk::util::c_disassembler::get_pushes(uint32_t address, size_t size, size_t min)
{
	if (!size) size = sdk::util::c_mem::Instance().find_size(address);
	auto raw_asm = this->get_asm(address, size);
	auto base = GetModuleHandleA(0);
	auto data1_sec = sdk::util::c_mem::Instance().get_section(".data1", base);
	auto data1_max = (uintptr_t)base + data1_sec.first + data1_sec.second;
	auto ret = sdk::util::t_asm_res();
	for (auto a : raw_asm)
	{
		if (a.empty()) continue;
		if (a.find("push ,") != std::string::npos)
		{
			std::regex rex("0[xX][0-9a-fA-F]+"); std::smatch match;
			auto has = std::regex_search(a, match, rex);
			if (!has) continue;
			for (auto b : match)
			{
				auto hex = std::stoull(b, nullptr, 16);
				if (hex > data1_max || hex < min) continue;
				ret.push_back((uint32_t)hex);
			}
		}
	}
	return ret;
}
