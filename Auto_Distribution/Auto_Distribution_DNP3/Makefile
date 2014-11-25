CFILE=./broccoli.c
GCC=/usr/bin/gcc
BROCCOLI=/usr/local/bro
all: 	
	$(GCC) -Wall -Wno-unused -g -DDEBUG -DBRO_DEBUG -g $(CFILE)  -o bro -rdynamic $(BROCCOLI)/lib/libbroccoli.so.5.1.0 -lssl -lcrypto -Wl,-rpath,$(BROCCOLI)/lib/

