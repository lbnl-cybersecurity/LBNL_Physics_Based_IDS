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

#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <modbus/modbus.h>
#include "auto_distribution.h"
#include "modbusRegisters.h" 

extern pthread_mutex_t modbusRegisterAccessLock;
modbus_mapping_t *mb_mapping;
int lp0=1,lp1=1,lp2=1,lp3=1;
int cbstatus=0;
extern int k;
//Predicts which load points will have power supply with the information of the fault detector values
void * startPhysicalModel(void * data)
{
	

	int sts;
	struct timespec tm;
	mb_mapping = (modbus_mapping_t *)data;
	short fdreg[2*k-1],cbreg;
	short fdc[2*k-1],cbc;
	short fi[k],cb;
	short fd[2*k-1];
	int lp[k];
	short ss[2*k-1];
	int flag=0;
	float modelDelayTime_hrs =0.0005;
	char * displayFlag;
	bool displayCnt = true;
	int fault=0;
	int i;
	if(data == NULL) {
		perror("Illegal modbus mapping struct.");
		exit(1);
	}
	 for(i=0;i<k;i++)
                        {
                                lp[i]=1;
                        }


	printf("\nCB- Circuit Breaker \tFD- Fault Detector \tFI- Fault Indicator \tSS- Section Switch \nLP-Load Point [1 when LP is active, 0 when LP is inactive]\n\n");

	//Register mapping-
	//CB-1
	//FD- 2 to 2k
	//FI - 2k+1 to 3k
	//SS - 3k+1 to 5k-1
	while(1){

		pthread_mutex_lock(&modbusRegisterAccessLock);
		cbreg = getRegisterValue(mb_mapping,CIRCUIT_BREAKER_REG);
	
		for(i=0;i<2*k-1;i++)
		{
			fdreg[i] = getRegisterValue(mb_mapping,i+2);                        		}
		for(i=0;i<k;i++)
		{
			fi[i]=getRegisterValue(mb_mapping,i+2*k+1);
		}
		for(i=0;i<2*k-1;i++)
		{	
			ss[i]=getRegisterValue(mb_mapping,i+3*k+1);
		}

		pthread_mutex_unlock(&modbusRegisterAccessLock);




		//predict lps
		if(cbstatus==1)
		{


			for(i=0;i<k;i++)
			{
				lp[i]=0;
			}
			setRegisterValue(mb_mapping,CIRCUIT_BREAKER_REG,0);
			cbstatus=0;

		}
		else
		{
			if(fd[0]==0)
			{
				lp[0]=1;
				for(i=1;i<2*k-2;)
				{
					if((fd[i]==0)&&(fd[i+1]==0))
					{
						lp[(i+1)/2]=1;

					}
					i+=2;
				}
			}
		}


		printf("\nCB=%u\n",cbreg);

		for(i=0;i<2*k-1;i++)
		{printf("fd[%d]=%u ",i,fdreg[i]);}

		printf("\n");		
		for(i=0;i<k;i++)
		{printf("fi[%d]=%u",i,fi[i]);}

		printf("\n");		
		for(i=0;i<2*k-1;i++)
		{printf("ss[%d]=%u",i,ss[i]);}

		printf("\n");
		for(i=0;i<k;i++)
		{printf("lp[%d]=%u",i,lp[i]);}


		if(flag==0)
		{	
			for(i=0;i<2*k-1;i++)
			{fd[i]=fdreg[i];}
			cb=cbreg;
			flag=1;
			continue;
		}
		if(flag==1)
		{
			for(i=0;i<2*k-1;i++)
			{fdc[i]=0;}
			cbc=0;		
			for(i=0;i<2*k-1;i++)
			{if(fdreg[i]!=fd[i])
				{
					fdc[i]=1;
					fd[i]=fdreg[i];
				}
			}


			if(cbreg!=cb)
			{cbc=1;
				cb=cbreg;
			}
		}

		usleep(PHYSICAL_MODEL_DELAY_INTERVAL_MSEC * 500);
		pthread_mutex_lock(&modbusRegisterAccessLock);

		if(fdc[0]==1)
		{
			if((fd[0]==1))
			{
				printf("\nFault between Main and LP0\nIsolated the line with switches SS1");
			}
			else
			{
				printf("\nThe line between Main and LP0 is repaired");
			}

			setRegisterValue(mb_mapping,CIRCUIT_BREAKER_REG,1);
			cbstatus=1;
			//set ss and fi to value based on the fd CB will be turned off later
			setRegisterValue(mb_mapping,2*k+1,fd[0]);
			setRegisterValue(mb_mapping,3*k+1,fd[0]);
		}
		for(i=1;i<2*k-2;)
		{


			if((fdc[i]==1)&&(fdc[i+1]==1))
			{
				if((fd[i]==1)&&(fd[i+1]==1))
				{
					printf("\nFault between LP%d and LP%d\nIsolated the line with switches SS%d and SS%d",(i+1)/2-1,(i+1)/2,i,i+1);
				}
				else
				{
					printf("\nThe line between LP%d and LP%d is repaired",(i+1)/2,(i+1)/2-1);
				} 
				setRegisterValue(mb_mapping,CIRCUIT_BREAKER_REG,1);
				cbstatus=1;
				//Turn on/off the switches and the fault indicator based on the fd value, CB will be turned off in next iteration
				setRegisterValue(mb_mapping,2*k+1+i,fd[i]);
				setRegisterValue(mb_mapping,3*k+1+i,fd[i]);
				setRegisterValue(mb_mapping,3*k+2+i,fd[i]);
			}
			i+=2;	
		}

		pthread_mutex_unlock(&modbusRegisterAccessLock);



	}
}
