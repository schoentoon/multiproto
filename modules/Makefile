CFLAGS := -Wall -O2 -mtune=native -g ${CFLAGS}
INC    := -I../include ${INC}
LFLAGS := -shared -fPIE
DEFINES:= ${DEFINES}
CC     := ${CC}
MODULES:= http.so irc.so smtp.so minecraft.so

.PHONY: all clean

all: $(MODULES)

http.so: src/http.c
	$(CC) $(CFLAGS) $(DEFINES) $(INC) $(LFLAGS) -o http.so src/http.c

irc.so: src/irc.c
	$(CC) $(CFLAGS) $(DEFINES) $(INC) $(LFLAGS) -o irc.so src/irc.c

smtp.so: src/smtp.c
	$(CC) $(CFLAGS) $(DEFINES) $(INC) $(LFLAGS) -o smtp.so src/smtp.c

minecraft.so: src/minecraft.c
	$(CC) $(CFLAGS) $(DEFINES) $(INC) $(LFLAGS) -o minecraft.so src/minecraft.c

clean:
	rm -rfv *.so