#include "Disassembler.h"

bool Disassembler::Initialize()
{
	if (ZYAN_FAILED(ZydisDecoderInit(&decoder,
		ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64)))
	{
		return false;
	}

	if (ZYAN_FAILED(ZydisFormatterInit(&formatter, 
		ZYDIS_FORMATTER_STYLE_INTEL)))
	{
		return false;
	}

	return true;
}

bool Disassembler::Decode(ZyanUSize length)
{
	if (ZYAN_FAILED(ZydisDecoderDecodeInstruction(
		&decoder, &context, address, length, &instruction)))
	{
		return false;
	}

	if (ZYAN_FAILED(ZydisDecoderDecodeOperands(&decoder,
		&context, &instruction, operands, instruction.operand_count)))
	{
		return false;
	}

	return true;
}

void Disassembler::Increment()
{
	address = reinterpret_cast<void*>(
		reinterpret_cast<uint64_t>(address) + instruction.length);
}

bool Disassembler::FormatInstruction(char* buffer, size_t size, bool absolute)
{
	if (ZYAN_FAILED(ZydisFormatterFormatInstruction(&formatter, &instruction, 
		operands, instruction.operand_count_visible, buffer, size, absolute ? 
		reinterpret_cast<uint64_t>(address) : ZYDIS_RUNTIME_ADDRESS_NONE, ZYAN_NULL)))
	{
		return false;
	}

	return true;
}