/*
*** Copyright Notice ***
Water Heater monitoring system, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved.

If you have questions about your rights to use or distribute this software, please contact Berkeley Lab's Technology Transfer Department at  TTD@lbl.gov.

NOTICE.  This software is owned by the U.S. Department of Energy.  As such, the U.S. Government has been granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, and perform publicly and display publicly.  Beginning five (5) years after the date permission to assert copyright is obtained from the U.S. Department of Energy, and subject to any subsequent five (5) year renewals, the U.S. Government is granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
****************************


*** License Agreement ***
"Water Heater monitoring system, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved."

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

(1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

(2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

(3) Neither the name of the University of California, Lawrence Berkeley National Laboratory, U.S. Dept. of Energy nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

You are under no obligation whatsoever to provide any bug fixes, patches, or upgrades to the features, functionality or performance of the source code ("Enhancements") to anyone; however, if you choose to make your Enhancements available either publicly, or directly to Lawrence Berkeley National Laboratory, without imposing a separate written license agreement for such Enhancements, then you hereby grant the following license: a  non-exclusive, royalty-free perpetual license to install, use, modify, prepare derivative works, incorporate into other computer software, distribute, and sublicense such enhancements or derivative works thereof, in binary and source code form.
***************************   
*/

/* 
 * File:   main.c
 * Author: mcp
 *
 * Created on May 5, 2013, 11:43 PM
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <modbus/modbus.h>

#include "modbusMonitorClientMain.h"
#include "waterHeaterModel.h"

int main(int argc, char *argv[])
{
    uint8_t *tab_bits;
    uint8_t *tab_input_bits;
    uint16_t *tab_value_registers;
    uint16_t *tab_input_value_registers;
    modbus_t *ctx;
    int rc;
    
    float heaterTemp;
    float heaterSetPoint;
    char * heaterEnableState;
    char * heaterElementState;
    bool displayCnt = true;
    char * displayFlag;
    char * modbusClientIP = DEFAULT_CLIENT_IP;
    int modbusPort = DEFAULT_PORT;
    int modbusPollIntervalmSec = DEFAULT_POLL_INTERVAL_MSEC;

    int argcnt = 1;
    while(argc > argcnt) {
        if(strcmp(argv[argcnt], IP_ADDRESS_FLAG) == 0) {
            argcnt++;
            if(argcnt < argc) {
                modbusClientIP = argv[argcnt++];
            }
            else {
                printf("Illegal command flags\n");
                exit(1);
            }
        }
        else if(strcmp(argv[argcnt], IP_PORT_FLAG) == 0) {
            argcnt++;
            if(argcnt < argc) {
                sscanf(argv[argcnt++], "%i", &modbusPort);
            }
            else {
                printf("Illegal command flags\n");
                exit(1);
            }
        }
        else if(strcmp(argv[argcnt], POLL_INTERVAL_FLAG) == 0) {
            argcnt++;
            if(argcnt < argc) {
                sscanf(argv[argcnt++], "%i", &modbusPollIntervalmSec);
                /* interpret command line argument as seconds.  Convert to ms. */
                modbusPollIntervalmSec *= 1000;
            }
            else {
                printf("Illegal command flags\n");
                exit(1);
            }
        }
        else {
            printf("Illegal command flags\n");
            exit(1);
        }
    }

    /* create modbus context structure */
    ctx = modbus_new_tcp(modbusClientIP, modbusPort);
    if (ctx == NULL) {
        fprintf(stderr, "Unable to allocate libmodbus context\n");
        return -1;
    }
    

    /* allocate room to hold water heater register data */
    tab_bits = (uint8_t *)malloc(NUM_BIT_REG * sizeof(uint8_t));
    memset(tab_bits, 0, NUM_BIT_REG * sizeof(uint8_t));
    
    tab_input_bits = (uint8_t *)malloc(NUM_INPUT_BIT_REG * sizeof(uint8_t));
    memset(tab_input_bits, 0, NUM_INPUT_BIT_REG * sizeof(uint8_t));
    
    tab_value_registers = (uint16_t *)malloc(NUM_VALUE_REG * sizeof(uint16_t));
    memset(tab_value_registers, 0, NUM_VALUE_REG * sizeof(uint16_t));
    
    tab_input_value_registers = (uint16_t *)malloc(NUM_INPUT_VALUE_REG * sizeof(uint16_t));
    memset(tab_input_value_registers, 0, NUM_INPUT_VALUE_REG * sizeof(uint16_t));
    
   
    
    /* loop forever */
    while(1) {

        if (modbus_connect(ctx) == -1) {
            fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
            modbus_free(ctx);
            break;
        }

        rc = modbus_read_bits(ctx, HEATER_COIL_ENABLE, 
                NUM_BIT_REG, tab_bits);
        if (rc != 1) {
            break;
        }

        rc = modbus_read_input_bits(ctx, HEATER_COIL_ENERGIZED, 
                NUM_INPUT_BIT_REG, tab_input_bits);
        if (rc != 1) {
            break;
        }

        rc = modbus_read_input_registers(ctx, HEATER_WATER_TEMP_REG,
                NUM_INPUT_VALUE_REG, tab_input_value_registers);
        if (rc != 1) {
            break;
        }

        rc = modbus_read_registers(ctx, HEATER_WATER_TARGET_TEMP_REG,
                NUM_VALUE_REG, tab_value_registers);
        if (rc != 1) {
            break;
        }

        /* close the connection */
        modbus_close(ctx);
        
        if(tab_bits[0]) {
            heaterEnableState = "Heater Enabled, ";
        }
        else {
            heaterEnableState = "Heater Disabled, ";
        }
        
        if(tab_input_bits[0]) {
            heaterElementState = "Heater Element On, ";
        }
        else {
            heaterElementState = "Heater Element Off, ";
        }
        
        heaterSetPoint = ((float)tab_value_registers[0]) / 10.0;
        heaterTemp = ((float)tab_input_value_registers[0]) / 10.0;
        
        if (displayCnt) {
            displayCnt = false;
            displayFlag = "+";
        }
        else {
            displayCnt = true;
            displayFlag = "-";
        }
        
        printf("Status(%s): %s%sSet Point: %5.1f, Temp: %5.1f\n",
                displayFlag,
                heaterEnableState, heaterElementState, 
                heaterSetPoint, heaterTemp);

        memset(tab_input_value_registers, 0, NUM_INPUT_VALUE_REG * sizeof(uint16_t));
        
        usleep(modbusPollIntervalmSec * 1000);
    }
    
    printf("Failed modbus read\n", rc);
    printf("Exiting due to read failure.\n");

    /* Free the memory */
    free(tab_bits);
    free(tab_input_bits);
    free(tab_value_registers);
    free(tab_input_value_registers);

    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
