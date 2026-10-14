#include <windows.h>

WINBASEAPI LPVOID WINAPI KERNEL32$GetProcAddress(HMODULE hModule, LPCSTR lpProcName);

LPVOID WINAPI sleepmask(HMODULE hModule, LPCSTR lpProcName) {
	// A function that hooks GetProcAddress() to implement sleep masking.
	
	// TODO temporary implementation just to see if it works, doesn't hook or mask anything...
	return KERNEL32$GetProcAddress(hModule, lpProcName);
}
