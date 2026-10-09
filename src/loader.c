#include <windows.h>
#include "tcg.h"
#include "compost.h"

WINBASEAPI LPVOID WINAPI KERNEL32$VirtualAlloc (LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);

void go() {
    DLLDATA data;
    IMPORTFUNCS funcs;
 
    char *src = retrieve_dll();
    
    ParseDLL(src, &data);
 
    char *dst = KERNEL32$VirtualAlloc( NULL, SizeOfDLL(&data), MEM_RESERVE|MEM_COMMIT, PAGE_EXECUTE_READWRITE );
 
    LoadDLL(&data, src, dst);
 
    funcs.GetProcAddress = GetProcAddress;
    funcs.LoadLibraryA   = LoadLibraryA;
 
    ProcessImports(&funcs, &data, dst);
 
 	DLLMAIN_FUNC entry = EntryPoint(&data, dst);
 	invoke_entrypoint(dst, (char *) entry, SizeOfDLL(&data));
}
