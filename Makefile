CC=i686-w64-mingw32-gcc
CC_64=x86_64-w64-mingw32-gcc
CFLAGS=-O1 -fno-jump-tables -shared -Wall -Wno-pointer-arith

all: x64

bin:
	mkdir -p bin
	mkdir -p bin/resolvers
	mkdir -p bin/retrievers
	mkdir -p bin/loaders
	mkdir -p bin/invokers

x64: bin
	$(CC_64) -DWIN_X64 $(CFLAGS) -c src/compost.c -o bin/compost.x64.o
	
	$(CC_64) -DWIN_X64 $(CFLAGS) -c src/resolvers/libtcg_ror13.c -o bin/resolvers/libtcg_ror13.x64.o
	
	$(CC_64) -DWIN_X64 $(CFLAGS) -c src/retrievers/linked_raw_dll.c -o bin/retrievers/linked_raw_dll.x64.o
	$(CC_64) -DWIN_X64 $(CFLAGS) -c src/retrievers/linked_xor_dll.c -o bin/retrievers/linked_xor_dll.x64.o
	
	$(CC_64) -DWIN_X64 $(CFLAGS) -c src/loaders/libtcg_reflective.c -o bin/loaders/libtcg_reflective.x64.o
	$(CC_64) -DWIN_X64 $(CFLAGS) -c src/loaders/libtcg_reflective_stomp.c -o bin/loaders/libtcg_reflective_stomp.x64.o
	
	$(CC_64) -DWIN_X64 $(CFLAGS) -c src/invokers/dll_entrypoint.c -o bin/invokers/dll_entrypoint.x64.o
	$(CC_64) -DWIN_X64 $(CFLAGS) -c src/invokers/beacon_entrypoint.c -o bin/invokers/beacon_entrypoint.x64.o

clean:
	rm -rf bin
