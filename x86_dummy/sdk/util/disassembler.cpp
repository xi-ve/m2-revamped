#include "disassembler.h"

void sdk::util::c_disassembler::setup()
{
}

sdk::util::t_asm_raw sdk::util::c_disassembler::get_asm(uint32_t address, size_t size)
{
	auto ret = sdk::util::t_asm_raw();

	if (!size || size >= 0x1256) return ret;

	uint8_t cbytes[0x1598] = { };
	memcpy(cbytes, (void*)(address), size);
	if (!cbytes) return ret;

	ZydisDecoder decoder;
	ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_COMPAT_32, ZYDIS_ADDRESS_WIDTH_32);

	ZydisFormatter formatter;
	if (!ZYAN_SUCCESS(ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL))) return ret;

	/*
	ZYDIS_FORMATTER_PROP_FORCE_RELATIVE_RIPREL
	*/
	
	ZyanU64 runtime_address = address;
	ZyanUSize offset = 0;
	const ZyanUSize length = size - 1;
	ZydisDecodedInstruction instruction;
	while (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&decoder, cbytes + offset, length - offset,
		   &instruction)))
	{
		// Format & print the binary instruction structure to human readable format
		char buffer[256];
		ZydisFormatterFormatInstruction(&formatter, &instruction, buffer, sizeof(buffer),
										runtime_address);

		ret.push_back(buffer);

		offset += instruction.length;
		runtime_address += instruction.length;
	}

	return ret;
}

sdk::util::t_asm_res sdk::util::c_disassembler::get_pushes(uint32_t address, size_t size, size_t min)
{
	if (!size) size = sdk::util::c_mem::Instance().find_size(address);
	auto raw_asm = this->get_asm(address, size);
	if (!raw_asm.size()) return {};
	auto base = GetModuleHandleA(0);
	auto data1_sec = sdk::util::c_mem::Instance().get_section(".data1", base);
	auto data1_max = (uintptr_t)base + data1_sec.first + data1_sec.second;
	auto ret = sdk::util::t_asm_res();
	for (auto &a : raw_asm)
	{
		if (a.empty()) continue;
		if (a.find("push") != std::string::npos)
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

sdk::util::t_asm_res sdk::util::c_disassembler::get_adds(uint32_t address, size_t size, size_t min)
{
	if (!size) size = sdk::util::c_mem::Instance().find_size(address);
	auto raw_asm = this->get_asm(address, size);
	if (!raw_asm.size()) return {};
	auto base = GetModuleHandleA(0);
	auto data1_sec = sdk::util::c_mem::Instance().get_section(".data1", base);
	auto data1_max = (uintptr_t)base + data1_sec.first + data1_sec.second;
	if (!min) min = (uintptr_t)base;
	auto ret = sdk::util::t_asm_res();
	for (auto &a : raw_asm)
	{
		if (a.empty()) continue;
		if (a.find("push") != std::string::npos)
		{
			std::regex rex("0[xX][0-9a-fA-F]+"); std::smatch match;
			auto has = std::regex_search(a, match, rex);
			if (!has) continue;
			for (auto &b : match)
			{
				auto hex = std::stoull(b, nullptr, 16);
				if (hex > data1_max || hex < min) continue;
				ret.push_back((uint32_t)hex);
			}
		}
	}
	return ret;
}

sdk::util::t_asm_res sdk::util::c_disassembler::get_calls(uint32_t address, size_t size, size_t min, BOOL ripr)
{
	if (!size) size = sdk::util::c_mem::Instance().find_size(address);
	auto raw_asm = this->get_asm(address, size);
	if (!raw_asm.size()) return {};
	auto base = GetModuleHandleA(0);
	auto data1_sec = sdk::util::c_mem::Instance().get_section(".data1", base);
	auto data1_max = (uintptr_t)base + data1_sec.first + data1_sec.second;
	if (!min) min = (uintptr_t)base;
	auto ret = sdk::util::t_asm_res();
	for (auto a : raw_asm)
	{
		if (a.empty()) continue;
		if (a.find("call") != std::string::npos)
		{
			std::regex rex("0[xX][0-9a-fA-F]+"); std::smatch match;
			auto has = std::regex_search(a, match, rex);
			if (!has) continue;
			for (auto b : match)
			{
				auto hex = std::stoull(b, nullptr, 16);
				if (hex > data1_max || hex < min) continue;
				if (!ripr) ret.push_back((uint32_t)hex);
				else ret.push_back(this->convert_rip((uint32_t)hex, 1));
			}
		}
	}
	return ret;
}

sdk::util::t_asm_res sdk::util::c_disassembler::get_jumps(uint32_t address, size_t size, size_t min)
{
	if (!size) size = sdk::util::c_mem::Instance().find_size(address);
	auto raw_asm = this->get_asm(address, size);
	if (!raw_asm.size()) return {};
	auto base = GetModuleHandleA(0);
	auto data1_sec = sdk::util::c_mem::Instance().get_section(".data1", base);
	auto data1_max = (uintptr_t)base + data1_sec.first + data1_sec.second;
	auto ret = sdk::util::t_asm_res();
	for (auto a : raw_asm)
	{
		if (a.empty()) continue;
		if (a.find("jmp") != std::string::npos)
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

sdk::util::t_asm_res sdk::util::c_disassembler::get_offsets(uint32_t address, size_t size, size_t min, size_t max)
{
	if (!size) size = sdk::util::c_mem::Instance().find_size(address);
	auto raw_asm = this->get_asm(address, size);
	if (!raw_asm.size()) return {};
	auto base = GetModuleHandleA(0);
	auto data1_sec = sdk::util::c_mem::Instance().get_section(".data1", base);
	auto data1_max = (uintptr_t)base + data1_sec.first + data1_sec.second;
	if (!min) min = (uintptr_t)base;
	auto ret = sdk::util::t_asm_res();
	for (auto &a : raw_asm)
	{
		if (a.empty()) continue;
		if (a.find("push") != std::string::npos ||
			a.find("lea") != std::string::npos)
		{
			std::regex rex("0[xX][0-9a-fA-F]+"); std::smatch match;
			auto has = std::regex_search(a, match, rex);
			if (!has) continue;
			for (auto &b : match)
			{
				auto hex = std::stoull(b, nullptr, 16);
				if (hex > data1_max || hex < min) continue;
				ret.push_back((uint32_t)hex);
			}
		}
	}
	return ret;
}

sdk::util::t_asm_res sdk::util::c_disassembler::get_custom(uint32_t address, size_t size, size_t min, size_t max, std::vector<std::string> opcodes)
{
	if (!size) size = sdk::util::c_mem::Instance().find_size(address);
	auto raw_asm = this->get_asm(address, size);
	if (!raw_asm.size()) return {};
	auto base = GetModuleHandleA(0);
	auto data1_sec = sdk::util::c_mem::Instance().get_section(".data1", base);
	auto data1_max = (uintptr_t)base + data1_sec.first + data1_sec.second;
	if (!min) min = (uintptr_t)base;
	auto ret = sdk::util::t_asm_res();
	for (auto &a : raw_asm)
	{
		if (a.empty()) continue;
		auto needed = false;
		for (auto b : opcodes) if (strstr(a.c_str(), b.c_str())) { needed = true; break; }
		if (needed)
		{
			std::regex rex("0[xX][0-9a-fA-F]+"); std::smatch match;
			auto has = std::regex_search(a, match, rex);
			if (!has) continue;
			for (auto &b : match)
			{
				auto hex = std::stoull(b, nullptr, 16);
				if (hex > data1_max || hex < min) continue;
				ret.push_back((uint32_t)hex);
			}
		}
	}
	return ret;
}

sdk::util::t_asm_raw sdk::util::c_disassembler::dump_asm(uint32_t address, size_t size)
{
	if (!size) size = sdk::util::c_mem::Instance().find_size(address);
	auto raw_asm = this->get_asm(address, size);
	return raw_asm;
}

uint32_t sdk::util::c_disassembler::convert_rip(uint32_t start, int offset)
{
	BYTE bResult[0x8] = { 0x0 };
	memcpy(bResult, (PVOID)(start + offset), 0x4);

	if (bResult[3] == 0xFF)
	{
		bResult[4] = 0xFF;
		bResult[5] = 0xFF;
		bResult[6] = 0xFF;
		bResult[7] = 0xFF;
	}

	return (start + offset + *((PDWORD32)bResult) + 0x4);
}
