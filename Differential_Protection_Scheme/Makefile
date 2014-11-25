CFILE=./broccoli.c
GCC=/usr/bin/gcc
BROCCOLI=/usr/local/bro
all:	$(PROGRAMS)
	$(GCC) -Wall -Wno-unused -g -DDEBUG -DBRO_DEBUG -g $(CFILE)  -o broccoli -rdynamic $(BROCCOLI)/lib/libbroccoli.so.5.1.0 -lssl -lcrypto -Wl,-rpath,$(BROCCOLI)/lib/
	cc ./simulation/modbusServerMain.c ./simulation/modbusRegisters.c ./simulation/differential.c -lmodbus -lm -pthread -o server
	cc ./simulation/modbusCommandClientMain.c -lmodbus -o command_client
	cc ./simulation/modbusClientMain.c -lmodbus -o monitor_client

clean:
	rm -f $(PROGRAMS) *.o core
