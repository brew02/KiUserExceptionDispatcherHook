#include <Windows.h>
#include <stdio.h>

#include "Disassembler.h"
#include "Syscalls.h"
#include "Handlers.h"

extern "C" bool Wow64PrepareForExceptionHook(PEXCEPTION_RECORD exceptionRecord, PCONTEXT contextRecord)
{
	printf("Exception Address: 0x%p\n", exceptionRecord->ExceptionAddress);

	if (exceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
	{
		printf("Breakpoint exception\n");

		if (*reinterpret_cast<uint8_t*>(contextRecord->Rip) == 0xCC)
			contextRecord->Rip++;

		return true;
	}

	return false;
}

int main()
{
	HMODULE ntdll = LoadLibraryA("ntdll.dll");
	if (!ntdll)
	{
		printf("Failed to load ntdll.dll\n");
		return -1;
	}

	/*
		There are many ways to find LdrProtectMrdata, this is
		just the one that I chose to use.
	*/
	void* rtlInitializeNtUserPfn = GetProcAddress(ntdll, "RtlInitializeNtUserPfn");
	if (!rtlInitializeNtUserPfn)
	{
		printf("Failed to find RtlInitializeNtUserPfn\n");
		return -1;
	}

	Disassembler disassembler{ rtlInitializeNtUserPfn };
	if (!disassembler.Initialize())
	{
		printf("Failed to initialize disassembler\n");
		return -1;
	}

	void(*ldrProtectMrdata)(bool protect) = nullptr;
	for (; disassembler.Decode(); disassembler.Increment())
	{
		if (disassembler.instruction.mnemonic != ZYDIS_MNEMONIC_CALL ||
			!(disassembler.instruction.attributes & ZYDIS_ATTRIB_IS_RELATIVE))
		{
			continue;
		}

		/*
			Should find: call LdrProtectMrdata
		*/
		for (ZyanU8 i = 0; i < disassembler.instruction.operand_count_visible; ++i)
		{
			ZydisDecodedOperand* operand = &disassembler.operands[i];
			if (operand->type != ZYDIS_OPERAND_TYPE_IMMEDIATE)
				continue;

			uint64_t address = 0;
			if (ZYAN_FAILED(ZydisCalcAbsoluteAddress(&disassembler.instruction, operand,
				reinterpret_cast<uint64_t>(disassembler.address), &address)))
			{
				printf("Failed to calculate address for LdrProtectMrdata\n");
				return -1;
			}

			ldrProtectMrdata = reinterpret_cast<decltype(
				ldrProtectMrdata)>(address);

			break;
		}

		break;
	}

	if (!ldrProtectMrdata)
	{
		printf("Failed to find LdrProtectMrdata\n");
		return -1;
	}

	void* kiUserExceptionDispatcher = GetProcAddress(ntdll, "KiUserExceptionDispatcher");
	if (!kiUserExceptionDispatcher)
	{
		printf("Failed to find KiUserExceptionDispatcher\n");
		return -1;
	}

	disassembler.address = kiUserExceptionDispatcher;

	uint64_t wow64PrepareForException = 0;
	for (; disassembler.Decode(); disassembler.Increment())
	{
		if (disassembler.instruction.mnemonic != ZYDIS_MNEMONIC_MOV ||
			!(disassembler.instruction.attributes & ZYDIS_ATTRIB_IS_RELATIVE))
		{
			continue;
		}

		/*
			Should find: mov rax, cs:Wow64PrepareForException
		*/
		for (ZyanU8 i = 0; i < disassembler.instruction.operand_count_visible; ++i)
		{
			ZydisDecodedOperand* operand = &disassembler.operands[i];
			if (operand->type != ZYDIS_OPERAND_TYPE_MEMORY)
				continue;

			if (ZYAN_FAILED(ZydisCalcAbsoluteAddress(&disassembler.instruction, operand, 
				reinterpret_cast<uint64_t>(disassembler.address), &wow64PrepareForException)))
			{
				printf("Failed to calculate address for Wow64PrepareForException\n");
				return -1;
			}

			break;
		}

		break;
	}

	if (!wow64PrepareForException)
	{
		printf("Failed to find Wow64PrepareForException\n");
		return -1;
	}

	Wow64PrepareForExceptionOriginal = *reinterpret_cast<void**>(wow64PrepareForException);

	ldrProtectMrdata(false);
	*reinterpret_cast<void**>(wow64PrepareForException) = Wow64PrepareForExceptionHookGate;
	ldrProtectMrdata(true);

	/*
		Small demo
	*/
	__debugbreak();

	int* x = nullptr;
	/*
		SEH still works (so does VEH)
	*/
	__try
	{
		*x = 0xdead;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		printf("Caught exception\n");
	}

	ldrProtectMrdata(false);
	*reinterpret_cast<void**>(wow64PrepareForException) = Wow64PrepareForExceptionOriginal;
	ldrProtectMrdata(true);

	return 0;
}