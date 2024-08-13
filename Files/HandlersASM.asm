extern Wow64PrepareForExceptionHook : proc
extern NtContinue : proc
extern Wow64PrepareForExceptionOriginal : qword

.code

	Wow64PrepareForExceptionHookGate proc

		push rcx
		push rdx

		sub rsp, 28h
		call Wow64PrepareForExceptionHook
		add rsp, 28h

		test al, al
		jz Exit

		pop rcx							; PCONTEXT ContextRecord
		add rsp, 10h					; remove return address and PEXCEPTION_RECORD
		
		xor rdx, rdx					; bool RaiseAlert
		jmp NtContinue

	Exit:
		pop rdx
		pop rcx

		mov rax, Wow64PrepareForExceptionOriginal
		test rax, rax
		jz NoWow64
		call rax

	NoWow64:
		ret

	Wow64PrepareForExceptionHookGate endp

end