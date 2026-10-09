#include <windows.h>
#include "../compost.h"
#include "../tcg.h"

void invoke_entrypoint(LOADED_DLL *dll) {
	((DLLMAIN_FUNC) dll->entrypoint)((HINSTANCE)dll->base, DLL_PROCESS_ATTACH, NULL);
}
