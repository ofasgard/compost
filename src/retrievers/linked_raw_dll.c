#include <windows.h>

char __DLLDATA__[0] __attribute__((section("linked_capability")));

char *retrieve_dll() {
    return (char *)&__DLLDATA__;
}
