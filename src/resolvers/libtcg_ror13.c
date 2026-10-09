#include <windows.h>
#include "../tcg.h"

FARPROC resolve(DWORD modHash, DWORD funcHash) {
	// Basic ROR13 resolver that uses the API resolution functions provided by LibTCG.
	
    HANDLE hModule = findModuleByHash(modHash);
    return findFunctionByHash(hModule, funcHash);
}
