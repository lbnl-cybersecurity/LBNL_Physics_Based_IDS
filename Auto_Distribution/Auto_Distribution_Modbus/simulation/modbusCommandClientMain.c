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

#include "modbusCommandClientMain.h"
#include "auto_distribution.h"

static bool changestate(int num);
static bool changeval(char * arg);
modbus_t *ctx;
int k,num_load;
//state 1 - make fault, 0- repair fault
int state=1;
int main(int argc, char *argv[])
{


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
				sscanf(argv[argcnt++], "%i", &num_load);
				k=num_load;
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
			if(strcmp(token, ST0) == 0) {
				if(changestate(0)) {
					printf("state changed. \n");
				}
				else {
					printf("Unable to change state\n");
				}
			}
			else if(strcmp(token, ST1) == 0) {
				if(changestate(1)) {
					printf("state changed. \n");
				}
				else {
					printf("Unable to change state\n");
				}
			}


			/* help? */
			else if(strcmp(token, HELP_CMD) == 0) {
				printf("\nThis program can send a command to the distribution system, first the state of the client software needs to be set.The 2 modes available are fault mode and repair mode.\nFault mode, creates a fault in a distribution line.\nRepair Mode repairs a distribution.\n Steps to use, Firsst change state- ST0,ST1 are two commands to change state, after changing state, choose the line[the line between main and LP0 is 1, LP0 and LP1 is 2 etcc.   Commands:\nSTF- Change State of Commands to create fault\nSTR- Change state of commands to repair fault\n\nHelp\nExit\nEnter your choice:-");
			}

			/* exit? */
			else if(strcmp(token, EXIT_CMD) == 0) {
				printf("Exiting.\n");
				exit(0);
			}
			else  {
				if(changeval(token)) {
					printf("operation Completed! \n");
				}
				else {
					printf("Failed\n");
				}
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
static bool changestate(int num) {
	state=num;
	return true;
}
static bool changeval(char * arg) {
	int num=atoi( arg);
	if(num>(k-1))
	{
		printf("\nInvalid line number given,please try again!\n");
	}
	bool sts;
	int reg;
	int rc;
	if (modbus_connect(ctx) == -1) {
		fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
		sts = false;
	} 
	reg=(num)*2;      
	if(num==1)
	{

		rc = modbus_write_register(ctx,2,state);
	}
	else
	{
		rc = modbus_write_register(ctx,reg,state);
		rc = modbus_write_register(ctx,reg-1,state);
	}

	if (rc != 1) {
		sts = false;
	}       
	else {
		sts = true;
	}       
	modbus_close(ctx);
	return sts;
}
