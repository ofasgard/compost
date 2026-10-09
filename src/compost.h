typedef struct {
	char *base;
	char *entrypoint;
	size_t size;
} LOADED_DLL;

char *retrieve_dll();
void load_dll(char *rawDll, LOADED_DLL *dll);
void invoke_entrypoint(LOADED_DLL *dll);
