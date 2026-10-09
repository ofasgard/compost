CC=i686-w64-mingw32-gcc
CC_64=x86_64-w64-mingw32-gcc
CFLAGS=-O1 -fno-jump-tables -shared -Wall -Wno-pointer-arith

all: x64

bin:
	mkdir -p bin
	mkdir -p bin/resolvers
	mkdir -p bin/retrievers

x64: bin
	$(CC_64) -DWIN_X64 $(CFLAGS) -c src/loader.c -o bin/loader.x64.o
	$(CC_64) -DWIN_X64 $(CFLAGS) -c src/resolvers/libtcg_ror13.c -o bin/resolvers/libtcg_ror13.x64.o
	$(CC_64) -DWIN_X64 $(CFLAGS) -c src/retrievers/linked_raw_dll.c -o bin/retrievers/linked_raw_dll.x64.o

clean:
	rm -rf bin
