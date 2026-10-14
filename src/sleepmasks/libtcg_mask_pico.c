#include <windows.h>
#include "../tcg.h"

WINBASEAPI LPVOID WINAPI KERNEL32$GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
WINBASEAPI VOID WINAPI KERNEL32$Sleep(DWORD dwMilliseconds);
WINBASEAPI NTSTATUS NTAPI NTDLL$NtWaitForSingleObject(HANDLE Handle, BOOLEAN Alertable, PLARGE_INTEGER Timeout);

WINBASEAPI VOID WINAPI _Sleep(DWORD dwMilliseconds) {
	dprintf("Masking logic would go here..."); // TODO
	KERNEL32$Sleep(dwMilliseconds);
	dprintf("Unmasking logic would go here..."); //TODO
}

WINBASEAPI NTSTATUS NTAPI _NtWaitForSingleObject(HANDLE Handle, BOOLEAN Alertable, PLARGE_INTEGER Timeout) {
	dprintf("Masking logic would go here..."); // TODO
	NTSTATUS status = NTDLL$NtWaitForSingleObject(Handle, Alertable, Timeout);
	dprintf("Unmasking logic would go here..."); //TODO
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
