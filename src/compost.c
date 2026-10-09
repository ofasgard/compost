#include <windows.h>
#include "compost.h"

void go() {
	// Central module for the reflective loader.
	// All it does is invoke the composable APIs we mixed in at link-time.

    char *rawDll = retrieve_dll();
 
 	LOADED_DLL dll;
    load_dll(rawDll, &dll);
 
 	invoke_entrypoint(&dll);
}
