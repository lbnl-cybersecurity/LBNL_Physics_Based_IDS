/* 
 * File:   modbusRegisters.h
 * Author: mcp
 *
 * Created on April 10, 2013, 5:37 PM
 */

#ifndef MODBUSREGISTERS_H
#define	MODBUSREGISTERS_H

#ifdef	__cplusplus
extern "C" {
#endif


   
extern void setRegisterBit(modbus_mapping_t * mb_mapping, int address, bool value);

extern void setInputRegisterBit(modbus_mapping_t * mb_mapping, int address, bool value);

extern bool getRegisterBit(modbus_mapping_t * mb_mapping, int address);

extern bool getInputRegisterBit(modbus_mapping_t * mb_mapping, int address);

extern void setRegisterValue(modbus_mapping_t * mb_mapping, int address, short value);

extern void setInputRegisterValue(modbus_mapping_t * mb_mapping, int address, short value);

extern short getRegisterValue(modbus_mapping_t * mb_mapping, int address);

extern short getInputRegisterValue(modbus_mapping_t * mb_mapping, int address);

#ifdef	__cplusplus
}
#endif

#endif	/* MODBUSREGISTERS_H */

