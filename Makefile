CFLAGS := -Wall -O2 -mtune=native -g ${CFLAGS}
INC    := -Iinclude ${INC}
LFLAGS := -levent -ldl -Wl,--export-dynamic
DEFINES:= ${DEFINES}
CC     := gcc
BINARY := multiproto
DEPS   := build/main.o build/debug.o build/config.o build/listener.o build/module.o build/proxy.o

.PHONY: all clean modules

all: build $(DEPS) bin/$(BINARY) modules

build:
	-mkdir -p build bin

build/main.o: src/main.c
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o build/main.o src/main.c

build/debug.o: src/debug.c include/debug.h
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o build/debug.o src/debug.c

build/config.o: src/config.c include/config.h
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o build/config.o src/config.c

build/listener.o: src/listener.c include/listener.h
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o build/listener.o src/listener.c

build/module.o: src/module.c include/module.h
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o build/module.o src/module.c

build/proxy.o: src/proxy.c include/proxy.h
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o build/proxy.o src/proxy.c

bin/$(BINARY): $(DEPS)
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -o bin/$(BINARY) $(DEPS) $(LFLAGS)

modules:
	$(MAKE) -C modules

clean:
	rm -rfv build bin
	$(MAKE) -C modules clean

clang:
	$(MAKE) CC=clang
