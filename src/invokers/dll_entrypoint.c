#include <windows.h>
#include "../compost.h"
#include "../tcg.h"

void invoke_entrypoint(LOADED_DLL *dll) {
	// Basic invoker that just calls the DLL's entrypoint directly.
	// This won't work with Beacon DLLs! They must be invoked slightly differently.
	
	((DLLMAIN_FUNC) dll->entrypoint)((HINSTANCE)dll->base, DLL_PROCESS_ATTACH, NULL);
}
