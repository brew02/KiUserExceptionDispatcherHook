# KiUserExceptionDispatcherHook

This project hooks KiUserExceptionDispatcher, the initial usermode exception dispatcher called from the kernel, by hooking Wow64PrepareForException, a function pointer stored in ntdll's .mrdata section.

![KiUserExceptionDispatcher](/Resources/KiUserExceptionDispatcher.PNG)

## Notes

To modify the .mrdata section in ntdll we call the LdrProtectMrdata function in ntdll. 

This project doesn't necessarily need Zydis, but:
1. I ripped this from a larger project that uses Zydis
2. It makes it much easier to find LdrProtectMrdata and Wow64PrepareForException

## Credits

[Zydis](https://github.com/zyantific/zydis/)