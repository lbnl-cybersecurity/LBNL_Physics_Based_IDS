/*
*** Copyright Notice ***
Auto Distribution Modbus, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved.

If you have questions about your rights to use or distribute this software, please contact Berkeley Lab's Technology Transfer Department at  TTD@lbl.gov.

NOTICE.  This software is owned by the U.S. Department of Energy.  As such, the U.S. Government has been granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, and perform publicly and display publicly.  Beginning five (5) years after the date permission to assert copyright is obtained from the U.S. Department of Energy, and subject to any subsequent five (5) year renewals, the U.S. Government is granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
****************************


*** License Agreement ***
"Auto Distribution Modbus, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved."

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
 * Author: Vishak Muthukumar
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <modbus/modbus.h>

#include "modbusMonitorClient.h"
#include "auto_distribution.h"
int k;
modbus_t *ctx;

int main(int argc, char *argv[])
{
	int i;

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

		else if(strcmp(argv[argcnt], LOAD_FLAG) == 0) {
			argcnt++;
			if(argcnt < argc) {
				sscanf(argv[argcnt++], "%i", &k);

			}
			else {
				printf("Illegal command flags\n");
				exit(1);
			}


		}
	}

	/* loop forever */
	uint16_t reg[5*k-1];

	while(1)
	{	
		ctx = modbus_new_tcp(modbusClientIP, modbusPort);


		if (ctx == NULL) {
			fprintf(stderr, "Unable to allocate libmodbus context\n");
			return -1;
		}

		if (modbus_connect(ctx) == -1) {
			fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
		}



		int rc=modbus_read_registers(ctx,1,5*k-1,reg);
		if (rc == -1) {
			fprintf(stderr, "ERRORRR-%s\n", modbus_strerror(errno));
			return -1;
		}

		printf("\nCB=%d\n",reg[0]);
		int i;
		for(i=0;i<2*k-1;i++)
		{printf("fd[%d]=%d",i,reg[i+1]);}
		printf("\n");
		 for(i=0;i<k;i++)
                {printf("fi[%d]=%d",i,reg[2*k+i]);}
		printf("\n"); 
		for(i=0;i<2*k-1;i++)
                {printf("ss[%d]=%d",i,reg[3*k+i]);}


		/* Close the connection */
		modbus_close(ctx);
		modbus_free(ctx);

		sleep(4);
	}
	return 0;
}
