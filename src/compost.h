typedef struct {
	char *base;
	char *entrypoint;
	size_t size;
} LOADED_DLL;

char *retrieve_dll();
void load_dll(char *rawDll, LOADED_DLL *dll);
void invoke_entrypoint(LOADED_DLL *dll);
void *get_sleep_hook(char *capability, size_t size);
