#include <modbus/modbus.h>
#include <stdbool.h>
#include "modbusRegisters.h"

/* these routines allow programatic set/get access to modbus values in the
 context structure. Since this same structure is accessed from within the 
 libmodbus communications routines, use of these routines should be made
 thread safe by use of shared mutexes at the calling level.*/

void setRegisterBit(modbus_mapping_t * mb_mapping, int address, bool value) {
    mb_mapping->tab_bits[address] = value;
}

void setInputRegisterBit(modbus_mapping_t * mb_mapping, int address, bool value) {
    mb_mapping->tab_input_bits[address] = value;
}

bool getRegisterBit(modbus_mapping_t * mb_mapping, int address) {
    return mb_mapping->tab_bits[address];
}

bool getInputRegisterBit(modbus_mapping_t * mb_mapping, int address) {
    return mb_mapping->tab_input_bits[address];
}

void setRegisterValue(modbus_mapping_t * mb_mapping, int address, short value) {
    mb_mapping->tab_registers [address]= value;
}

void setInputRegisterValue(modbus_mapping_t * mb_mapping, int address, short value) {
    mb_mapping->tab_input_registers [address]= value;
}

short getRegisterValue(modbus_mapping_t * mb_mapping, int address) {
    return mb_mapping->tab_registers[address];
}

short getInputRegisterValue(modbus_mapping_t * mb_mapping, int address) {
    return mb_mapping->tab_input_registers[address];
}