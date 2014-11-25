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
#include <stdlib.h>

/*
 * Copyright © 2008-2012 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>

#include <modbus/modbus.h>
#include "modbusServerMain.h"
#include "auto_distribution.h"
#include "modbusRegisters.h"

extern void * startPhysicalModel(void * data);
pthread_t physicalModelThread;
pthread_mutex_t modbusRegisterAccessLock = PTHREAD_MUTEX_INITIALIZER;

static void processTCPrequest(modbus_t * ctx,
		modbus_mapping_t *mb_mapping);

char * modbusClientIP = DEFAULT_CLIENT_IP;
int modbusPort = DEFAULT_PORT;
int num_load;
int k;    
int main(int argc, char *argv[])
{
	int socket;
	int requestSocket;
	int pid;
	modbus_t *ctx;
	modbus_mapping_t *mb_mapping;
	int rc;
	int use_backend;

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
				sscanf(argv[argcnt++], "%i", &num_load);
				k=num_load;
			}
			else {
				printf("Illegal command flags\n");
				exit(1);
			}
		}

		else if(strcmp(argv[argcnt], HELP_FLAG) == 0) {
                        argcnt++;
			printf("\nHelp Commands \n1. Set port  -p <port number \n2. Set IP  -n <ip address> \n3. Set number of load points -l <no. of load>\n");
                	exit(1);        
                }

		else {
			printf("Illegal command flags1\n");
			exit(1);
		}
	}

	/* create modbus context structure */
	ctx = modbus_new_tcp(modbusClientIP, modbusPort);
	if (ctx == NULL) {
		fprintf(stderr, "Unable to allocate libmodbus context\n");
		return -1;
	}


	/* create a mapping that describes our PLC */
	mb_mapping = modbus_mapping_new(0,0,5*num_load,0);
	if (mb_mapping == NULL) {
		fprintf(stderr, "Failed to allocate the mapping: %s\n",
				modbus_strerror(errno));
		modbus_free(ctx);
		return -1;
	}

	/* start the physical model */
	pthread_create(&physicalModelThread, NULL,startPhysicalModel, (void * )mb_mapping);
	/* sleep a bit to let model start up */
	sleep(1);

	pthread_mutex_lock(&modbusRegisterAccessLock);
	setRegisterValue(mb_mapping,CIRCUIT_BREAKER_REG,0);
	setRegisterValue(mb_mapping,CIRCUIT_BREAKER_REG,0);
	int i;
	for(i=0;i<2*k-1;i++)
	{
		setRegisterValue(mb_mapping,i+2,0);                                    
	}
	for(i=0;i<k;i++)
	{
		setRegisterValue(mb_mapping,i+2*k+1,0);
	}
	for(i=0;i<2*k-1;i++)
	{
		setRegisterValue(mb_mapping,i+3*k+1,0);
	}



	pthread_mutex_unlock(&modbusRegisterAccessLock);

	ctx = modbus_new_tcp(modbusClientIP, modbusPort);
	socket = modbus_tcp_listen(ctx, 1);
	/* this is the main receive loop */
	while(1) {
		requestSocket = modbus_tcp_accept(ctx, &socket);
		/* process the request */
		processTCPrequest(ctx, mb_mapping);
		close(requestSocket);
	}

	/* we have encountered some error and will exit */
	printf("Quit the loop: %s\n", modbus_strerror(errno));

	modbus_mapping_free(mb_mapping);
	close(socket);
	modbus_free(ctx);

	return 0;
}

void processTCPrequest(modbus_t * ctx,
		modbus_mapping_t * mb_mapping) {
	for(;;) {
		uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH+1];
		//query[MODBUS_TCP_MAX_ADU_LENGTH] = 0;

		int rc = modbus_receive(ctx, query);
		if (rc > 0) {
			pthread_mutex_lock(&modbusRegisterAccessLock);
			modbus_reply(ctx, query, rc, mb_mapping);
			pthread_mutex_unlock(&modbusRegisterAccessLock);
		} else if (rc  == -1) {
			/* Connection closed by the client or error */
			break;
		}

	}
}
