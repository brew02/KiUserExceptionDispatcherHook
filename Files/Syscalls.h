#pragma once
#include <Windows.h>

extern "C" NTSTATUS NtContinue(
	PCONTEXT ContextRecord, bool RaiseAlert);