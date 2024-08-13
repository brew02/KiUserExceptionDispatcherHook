.code

	NtContinue proc

		mov r10, rcx
		mov eax, 43h
		syscall
		ret

	NtContinue endp

end