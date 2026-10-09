#include <windows.h>

typedef BOOL WINAPI (*DLLMAIN_FUNC)(HINSTANCE, DWORD, LPVOID);

void invoke_entrypoint(char *dllBase, char *dllEntry, size_t dllSize) {
	((DLLMAIN_FUNC)dllEntry)((HINSTANCE)dllBase, DLL_PROCESS_ATTACH, NULL);
}
