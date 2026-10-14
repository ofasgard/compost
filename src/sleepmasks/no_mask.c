#include <windows.h>
#include "../compost.h"

WINBASEAPI LPVOID WINAPI KERNEL32$GetProcAddress(HMODULE hModule, LPCSTR lpProcName);

void *get_sleep_hook() {
	// No sleepmask at all. Returns the original unhooked address of the GetProcAddress() API.
	return (void *) KERNEL32$GetProcAddress;
}
