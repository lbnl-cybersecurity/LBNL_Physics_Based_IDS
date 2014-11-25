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

#include "modbusCommandClientMain.h"
#include "waterHeaterModel.h"

static bool enableHeater();
static bool disableHeater();
static bool setTargetTemp(float degF);

modbus_t *ctx;
    
int main(int argc, char *argv[])
{
    
    float heaterTemp;
    float heaterSetPoint;
    char * heaterEnableState;
    char * heaterElementState;
   
    char * modbusClientIP = DEFAULT_CLIENT_IP;
    int modbusPort = DEFAULT_PORT;

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
    
    /* start the main loop */
    printf("Modbus Command Client (\"help\" for commands)\n");
    
    /* loop forever */
    while(1) {
        char s[32];
        char * token;
        
        /* get command line */
        fgets(s, sizeof(s), stdin);
        /* tokenize and walk through the line */
        token = strtok(s, " ");
        if(token != NULL) {
            /* enable heater? */
            if(strcmp(token, ENABLE_HEATER_CMD) == 0) {
                if(enableHeater()) {
                    printf("Heater enabled. \n");
                }
                else {
                    printf("Unable to enable heater.\n");
                }
            }
            
            /* disable heater ? */
            else if(strcmp(token, DISABLE_HEATER_CMD) == 0) {
                if(disableHeater()) {
                    printf("Heater disabled. \n");
                }
                else {
                    printf("Unable to disable heater.\n");
                }
            }
            
            /* set target temp? */
            else if(strcmp(token, SET_TARGET_TEMP_CMD) == 0) {
                token = strtok(NULL, " ");
                if(token != NULL) {
                    float degF;
                    int valueSts;
                    bool sts;
                    /* remove the newline */
                    char * p = strchr( token, '\n' );
                    if ( p ) *p = '\0';
                    /* attempt to convert it */
                    valueSts = sscanf(token, "%f", &degF);
                    if(valueSts == 1) {
                        sts = setTargetTemp(degF);
                        if(sts) {
                            printf("Target temp set.\n");
                        }
                        else {
                            printf("Unable to set target temp.\n");
                        }
                    }
                    else {
                        printf("Badly formatted temp.\n");
                    }
                    
                }
                else {
                    printf("Target temp missing.\n");
                }
            }
            
            /* help? */
            else if(strcmp(token, HELP_CMD) == 0) {
                printf("Commands:\n\t%s\t%s\t%s 123.4\n"
                        "\t%s\t%s\n",
                        ENABLE_HEATER_CMD, DISABLE_HEATER_CMD, 
                        SET_TARGET_TEMP_CMD, HELP_CMD, EXIT_CMD);
            }
            
            /* exit? */
            else if(strcmp(token, EXIT_CMD) == 0) {
                printf("Exiting.\n");
                exit(0);
            }
            
            /* unknown command */
            else {
                printf("Unknown command.\n");
            } 
        }
        else {
            printf("Unknown command.\n");
        }
    }
    
    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}

static bool enableHeater() {
    bool sts;
    int rc;
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
            sts = false;
    }
    rc = modbus_write_bit(ctx, HEATER_COIL_ENABLE, true);
    if (rc != 1) {
        sts = false;
    }
    else {
        sts = true;
    }
    modbus_close(ctx);
    return sts;
}

static bool disableHeater() {
    bool sts;
    int rc;
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
            sts = false;
    }
    rc = modbus_write_bit(ctx, HEATER_COIL_ENABLE, false);
    if (rc != 1) {
        sts = false;
    }
    else {
        sts = true;
    }
    modbus_close(ctx);
    return sts;
}

static bool setTargetTemp(float degF) {
    bool sts;
    int rc;
    /* convert to short degF * 10 after rounding up to nearest tenth deg F */
    short degFx10 = (short)((degF + 0.05) * 10.0);
    
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
            sts = false;
    }
    rc = modbus_write_register(ctx, HEATER_WATER_TARGET_TEMP_REG, degFx10);
    if (rc != 1) {
        sts = false;
    }
    else {
        sts = true;
    }
    modbus_close(ctx);
    return sts;
}
