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
	if (!ZYAN_SUCCESS(ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL)) ||
		!ZYAN_SUCCESS(ZydisFormatterSetProperty(&formatter,
		ZYDIS_FORMATTER_PROP_FORCE_SEGMENT, ZYAN_TRUE)) ||
		!ZYAN_SUCCESS(ZydisFormatterSetProperty(&formatter,
		ZYDIS_FORMATTER_PROP_FORCE_SIZE, ZYAN_TRUE))) return ret;
	

	ZydisDecodedInstruction instruction;
	ZyanStatus status;
	ZyanUSize read_offset = 0;
	ZyanUSize buffer_size = size - 1;
	ZyanUSize buffer_remaining = 0;
	ZyanUSize read_offset_base = 0;
	char format_buffer[256];
	
	while ((status = ZydisDecoderDecodeBuffer(&decoder, cbytes + read_offset,
		   buffer_size - read_offset, &instruction)) != ZYDIS_STATUS_NO_MORE_DATA)
	{
		const ZyanU64 runtime_address = read_offset_base + read_offset;

		if (!ZYAN_SUCCESS(status)) break;
		ZydisFormatterFormatInstruction(&formatter, &instruction, format_buffer,
										sizeof(format_buffer), runtime_address);

		ret.push_back(format_buffer);

		sdk::util::c_log::Instance().duo("%s\n", format_buffer);

		read_offset += instruction.length;
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
	for (auto a : raw_asm)
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

sdk::util::t_asm_res sdk::util::c_disassembler::get_calls(uint32_t address, size_t size, size_t min)
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
				ret.push_back((uint32_t)hex);
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
	for (auto a : raw_asm)
	{
		if (a.empty()) continue;
		if (a.find("push") != std::string::npos ||
			a.find("lea") != std::string::npos)
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
