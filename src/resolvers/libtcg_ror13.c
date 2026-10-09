#include <windows.h>
#include "../tcg.h"

FARPROC resolve(DWORD modHash, DWORD funcHash) {
    HANDLE hModule = findModuleByHash(modHash);
    return findFunctionByHash(hModule, funcHash);
}
