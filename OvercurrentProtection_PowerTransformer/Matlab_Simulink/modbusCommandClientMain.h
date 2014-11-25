/* 
 * File:   modbusCommandClient.h
 * Author: mcp
 *
 * Created on May 6, 2013, 3:13 AM
 */

#ifndef MODBUSCOMMANDCLIENT_H
#define	MODBUSCOMMANDCLIENT_H

#ifdef	__cplusplus
extern "C" {
#endif

#define IP_ADDRESS_FLAG "-n"
#define IP_PORT_FLAG "-p"

#define ENABLE_HEATER_CMD "enableHeater\n"
#define DISABLE_HEATER_CMD "disableHeater\n"
#define SET_TARGET_TEMP_CMD "setTemp"
#define EXIT_CMD "exit\n"
#define HELP_CMD "help\n"


#ifdef	__cplusplus
}
#endif

#endif	/* MODBUSCOMMANDCLIENT_H */

