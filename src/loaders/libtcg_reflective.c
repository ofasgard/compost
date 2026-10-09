#include <windows.h>
#include "../compost.h"
#include "../tcg.h"

WINBASEAPI LPVOID WINAPI KERNEL32$GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
WINBASEAPI HMODULE WINAPI KERNEL32$LoadLibraryA(LPCSTR lpLibFileName);
WINBASEAPI LPVOID WINAPI KERNEL32$VirtualAlloc (LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);

void load_dll(char *rawDll, LOADED_DLL *dll) {
	// Basic reflective loader that uses the default implementation included with LibTCG.

	DLLDATA data;
	ParseDLL(rawDll, &data);
	
	dll->size = SizeOfDLL(&data);
	
	dll->base = KERNEL32$VirtualAlloc(NULL, dll->size, MEM_RESERVE|MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	LoadDLL(&data, rawDll, dll->base);
	
	IMPORTFUNCS funcs;
	funcs.GetProcAddress = (__typeof__(GetProcAddress) *) KERNEL32$GetProcAddress;
	funcs.LoadLibraryA = (__typeof__(LoadLibraryA) *) KERNEL32$LoadLibraryA;
	ProcessImports(&funcs, &data, dll->base);
	
	dll->entrypoint = (char *) EntryPoint(&data, dll->base);
}
