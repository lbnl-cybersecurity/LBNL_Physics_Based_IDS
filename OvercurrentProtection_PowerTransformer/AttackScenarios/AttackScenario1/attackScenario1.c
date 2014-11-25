#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <modbus/modbus.h>
#define NO_REGS 1
#define NO_BITS 2
#define DEFAULT_CLIENT_IP "169.254.128.196"
#define DEFAULT_PORT 502


modbus_t *ctx;

int main(int argc, char *argv[])
{
    uint16_t regs[1];
	float heaterTemp;
	float heaterSetPoint;
	char * heaterEnableState;
	char * heaterElementState;

	char * modbusClientIP = DEFAULT_CLIENT_IP;
	int modbusPort = DEFAULT_PORT;
    int packet_counter = 0;
	uint8_t *tab_bits;
	uint16_t *tab_input_value_registers;

	tab_bits = (uint8_t *)malloc(NO_BITS * sizeof(uint8_t));
	memset(tab_bits, 0, NO_BITS * sizeof(uint8_t));


	tab_input_value_registers = (uint16_t *)malloc(NO_REGS* sizeof(uint16_t));
	memset(tab_input_value_registers, 0, NO_REGS * sizeof(uint16_t));
        
    
        bool sts;
        int rc;
        uint8_t coils[2];
        ctx = modbus_new_tcp(modbusClientIP, modbusPort);
        if (ctx == NULL) {
            fprintf(stderr, "Unable to allocate libmodbus context\n");
            return -1;
        }
        
        if (modbus_connect(ctx) == -1) {
            sts = false;
        }
        
        rc = modbus_read_input_registers(ctx,0,1, regs);
        packet_counter++;
        printf("Packet No: %d, Value of current:%d\n",packet_counter, regs[0]);
        modbus_close(ctx);
		
     
        ctx = modbus_new_tcp(modbusClientIP, modbusPort);
        if (ctx == NULL) {
            fprintf(stderr, "Unable to allocate libmodbus context\n");
            return -1;
        }
        if (modbus_connect(ctx) == -1) {
            sts = false;
            
        }
        modbus_close(ctx);
        modbus_free(ctx);

    
    return 0;

}
