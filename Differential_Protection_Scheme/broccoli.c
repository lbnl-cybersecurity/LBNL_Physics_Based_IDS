/*
*** Copyright Notice ***
Broccoli-Differential Protection Scheme, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved.

If you have questions about your rights to use or distribute this software, please contact Berkeley Lab's Technology Transfer Department at  TTD@lbl.gov.

NOTICE.  This software is owned by the U.S. Department of Energy.  As such, the U.S. Government has been granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, and perform publicly and display publicly.  Beginning five (5) years after the date permission to assert copyright is obtained from the U.S. Department of Energy, and subject to any subsequent five (5) year renewals, the U.S. Government is granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
****************************


*** License Agreement ***
"Broccoli-Differential Protection Scheme, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved."

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

(1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

(2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

(3) Neither the name of the University of California, Lawrence Berkeley National Laboratory, U.S. Dept. of Energy nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

You are under no obligation whatsoever to provide any bug fixes, patches, or upgrades to the features, functionality or performance of the source code ("Enhancements") to anyone; however, if you choose to make your Enhancements available either publicly, or directly to Lawrence Berkeley National Laboratory, without imposing a separate written license agreement for such Enhancements, then you hereby grant the following license: a  non-exclusive, royalty-free perpetual license to install, use, modify, prepare derivative works, incorporate into other computer software, distribute, and sublicense such enhancements or derivative works thereof, in binary and source code form.
***************************   
*/
/*
   B R O C C O L I  --  The Bro Client Communications Library

   Copyright (C) 2004-2007 Christian Kreibich <christian (at) icir.org>

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies of the Software and its documentation and acknowledgment shall be
   given in the documentation and software packages that this Software was
   used.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>

#include <broccoli.h>
#ifdef BROCCOLI
#include <broccoli.h>
#endif
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

char *host_default = "127.0.0.1";
char *port_default = "47758";
char *port_default1="47759";
char *host_str;
char *port_str;

int count = -1;

uint64 seq1;


static void writereq(void *data,BroConn *conn,uint64 *seq)
{
	int *val=(int *)data;
	double now = bro_util_current_time();
	BroConn *bc;
	char hostname[512];
	snprintf(hostname, 512, "%s:%s", host_default, port_default1);

	if (! (bc = bro_conn_new_str(hostname, BRO_CFLAG_RECONNECT | BRO_CFLAG_ALWAYS_QUEUE)))
	{

		printf("Could not get Bro connection handle.\n");
		exit(-1);
	}

	if (! bro_conn_connect(bc))
	{
		printf("Could not connect to Bro at %s:%s.\n", host_str, port_default1);
		exit(-1);
	}


	BroEvent *ev;

	bro_conn_process_input(bc);


//	printf("\nReceived event from lower level script, sending to higher level script %llu\n",*seq);
	printf("\nReceived write request, forwarding to physical layer");	
	if ((ev = bro_event_new("writereq")))
	{
		double timestamp = bro_util_current_time();

		bro_event_send(bc, ev);
		bro_event_free(ev);


		conn = NULL;
		
	}
	bro_conn_delete(bc);
}


static void readreq(void *data,BroConn *conn,uint64 *seq)
{
        int *val=(int *)data;
        double now = bro_util_current_time();
        BroConn *bc;
        char hostname[512];
        snprintf(hostname, 512, "%s:%s", host_default, port_default1);

        if (! (bc = bro_conn_new_str(hostname, BRO_CFLAG_RECONNECT | BRO_CFLAG_ALWAYS_QUEUE)))
        {

                printf("Could not get Bro connection handle.\n");
                exit(-1);
        }

        if (! bro_conn_connect(bc))
        {
                printf("Could not connect to Bro at %s:%s.\n", host_str, port_default1);
                exit(-1);
        }


        BroEvent *ev;

        bro_conn_process_input(bc);

	printf("\nReceived read event, forwarding to physical layer");
        //printf("\nRead req Received event from lower level script, sending to higher level script %llu\n",*seq);
        if ((ev = bro_event_new("readreq")))
        {
                double timestamp = bro_util_current_time();

                bro_event_add_val(ev, BRO_TYPE_COUNT, NULL,(void *)seq);
                bro_event_send(bc, ev);
                bro_event_free(ev);


                conn = NULL;

        }
        bro_conn_delete(bc);
}

int main(int argc, char **argv)
{
	int opt, port;
	BroConn *bc;
	BroConn *bc1;
	extern char *optarg;
	extern int optind;
	char hostname[512];
	char hostname1[512];
	int fd = -1;

	bro_init(NULL);

	host_str = host_default;
	port_str = port_default;

	bro_debug_calltrace = 0;
	bro_debug_messages  = 0;


	snprintf(hostname, 512, "%s:%s", host_str, port_str);

	snprintf(hostname1, 512, "%s:%s", host_str, port_default1);


	if (! (bc = bro_conn_new_str(hostname, BRO_CFLAG_RECONNECT | BRO_CFLAG_ALWAYS_QUEUE)))
	{

		printf("Could not get Bro connection handle.\n");
		exit(-1);
	}


	if(! (bc1 = bro_conn_new_str(hostname1, BRO_CFLAG_RECONNECT | BRO_CFLAG_ALWAYS_QUEUE)))
	{
		printf("NOT the second bro");
		exit(-1);
	}

	void *data;
	data=malloc(300);

	bro_event_registry_add(bc, "writereq", (BroEventFunc) writereq,data);
	bro_event_registry_request (bc);
	bro_event_registry_add(bc, "readreq", (BroEventFunc) readreq,data);
	bro_event_registry_request (bc);
	if (! bro_conn_connect(bc))
	{
		printf("Could not connect to Bro at %s:%s.\n", host_str, port_str);
		exit(-1);
	}

	if (! bro_conn_connect(bc1))
	{
		printf("Could not connect to Bro at %s:%s.\n", host_str, port_default1);
		exit(-1);
	}

	/* Enter pinging loop */
for ( ; ; )
	{
BroEvent *ev;

bro_conn_process_input(bc);

		

		/* Create empty "ping" event */
		

#ifdef __MINGW32__
		sleep(1000);
#else
		sleep(1);
#endif
	}

	/* Disconnect from Bro and release state. */
	bro_conn_delete(bc1);
	bro_conn_delete(bc);
	return 0;
}
