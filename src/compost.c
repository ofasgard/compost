#include <windows.h>
#include "compost.h"

void go() {
    char *rawDll = retrieve_dll();
 
 	LOADED_DLL dll;
    load_dll(rawDll, &dll);
 
 	invoke_entrypoint(&dll);
}
