#include <windows.h>
#include "../compost.h"
#include "../tcg.h"

WINBASEAPI LPVOID WINAPI KERNEL32$VirtualAlloc (LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);

void load_dll(char *rawDll, LOADED_DLL *dll) {
	DLLDATA data;
	ParseDLL(rawDll, &data);
	
	dll->size = SizeOfDLL(&data);
	
	dll->base = KERNEL32$VirtualAlloc(NULL, dll->size, MEM_RESERVE|MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	LoadDLL(&data, rawDll, dll->base);
	
	IMPORTFUNCS funcs;
	funcs.GetProcAddress = GetProcAddress;
	funcs.LoadLibraryA   = LoadLibraryA;
	ProcessImports(&funcs, &data, dll->base);
	
	dll->entrypoint = (char *) EntryPoint(&data, dll->base);
}
