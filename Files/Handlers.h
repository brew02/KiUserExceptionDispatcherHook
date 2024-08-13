#pragma once
#include <Windows.h>

extern "C" void* Wow64PrepareForExceptionOriginal = nullptr;
extern "C" void Wow64PrepareForExceptionHookGate(PEXCEPTION_RECORD exceptionRecord, PCONTEXT contextRecord);