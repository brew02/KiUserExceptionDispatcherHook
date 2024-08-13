#pragma once
#include <Windows.h>
#include <Zydis/Zydis.h>

class Disassembler
{
public:
	ZydisFormatter formatter;
	ZydisDecoder decoder;
	ZydisDecoderContext context;
	ZydisDecodedInstruction instruction;
	ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];
	void* address;

	Disassembler(void* address)
	{
		this->address = address;
		ZeroMemory(&formatter, sizeof(formatter));
		ZeroMemory(&decoder, sizeof(decoder));
		ZeroMemory(&context, sizeof(context));
		ZeroMemory(&instruction, sizeof(instruction));
		ZeroMemory(operands, sizeof(operands));
	}

	bool Initialize();
	bool Decode(ZyanUSize length = ZYDIS_MAX_INSTRUCTION_LENGTH);
	void Increment();
	bool FormatInstruction(char* buffer, size_t size, bool absolute = false);
};