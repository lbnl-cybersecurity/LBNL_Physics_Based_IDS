CFILE=./broccoli.c
GCC=/usr/bin/gcc
BROCCOLI=/usr/local/bro
all: 	
	$(GCC) -Wall -Wno-unused -g -DDEBUG -DBRO_DEBUG -g $(CFILE)  -o bro -rdynamic $(BROCCOLI)/lib/libbroccoli.so.5.1.0 -lssl -lcrypto -Wl,-rpath,$(BROCCOLI)/lib/
	cc ./simulation/modbusServerMain.c ./simulation/modbusRegisters.c ./simulation/auto_distribution.c -lmodbus -lm -pthread -o server
	cc ./simulation/modbusCommandClientMain.c -lmodbus -o command_client
	cc ./simulation/modbusMonitorClient.c -lmodbus -o monitor_client
