/*
*** Copyright Notice ***
Auto Distribution DNP3, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved.

If you have questions about your rights to use or distribute this software, please contact Berkeley Lab's Technology Transfer Department at  TTD@lbl.gov.

NOTICE.  This software is owned by the U.S. Department of Energy.  As such, the U.S. Government has been granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, and perform publicly and display publicly.  Beginning five (5) years after the date permission to assert copyright is obtained from the U.S. Department of Energy, and subject to any subsequent five (5) year renewals, the U.S. Government is granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
****************************


*** License Agreement ***
"Auto Distribution DNP3, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved."

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

(1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

(2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

(3) Neither the name of the University of California, Lawrence Berkeley National Laboratory, U.S. Dept. of Energy nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

You are under no obligation whatsoever to provide any bug fixes, patches, or upgrades to the features, functionality or performance of the source code ("Enhancements") to anyone; however, if you choose to make your Enhancements available either publicly, or directly to Lawrence Berkeley National Laboratory, without imposing a separate written license agreement for such Enhancements, then you hereby grant the following license: a  non-exclusive, royalty-free perpetual license to install, use, modify, prepare derivative works, incorporate into other computer software, distribute, and sublicense such enhancements or derivative works thereof, in binary and source code form.
***************************   
*/



//
// Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
// more contributor license agreements. See the NOTICE file distributed
// with this work for additional information regarding copyright ownership.
// Green Energy Corp licenses this file to you under the Apache License,
// Version 2.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This file was forked on 01/01/2013 by Automatak, LLC and modifications
// have been made to this file. Automatak, LLC licenses these modifications to
// you under the GNU Affero General Public License Version 3.0
// (the "Additional License"). You may not use these modifications except in
// compliance with the additional license. You may obtain a copy of the Additional
// License at
//
// http://www.gnu.org/licenses/agpl.html
//
// Contact Automatak, LLC for a commercial license to these modifications
//

#include <opendnp3/LogToStdio.h>
#include <opendnp3/DNP3Manager.h>
#include <opendnp3/SlaveStackConfig.h>
#include <opendnp3/IChannel.h>
#include <opendnp3/IOutstation.h>
#include <opendnp3/TimeTransaction.h>
#include <opendnp3/CommandStatus.h>
#include <opendnp3/ICommandHandler.h>
#include <opendnp3/SimpleCommandHandler.h>
#include <string>
#include <iostream>
#define LOAD_FLAG "-l"
#define LOAD 4
using namespace std;
using namespace opendnp3;
int onprocess=0;
int command=0;
int val;

int k=4;
int flag=0;
bool *fd=new bool[2*k-1];
bool *fi=new bool[k];
bool *ss=new bool[2*k-1];
bool *lp=new bool[k];
//bool fd0=false,fd1=false,fd2=false,fd3=false,fd4=false,fd5=false,fd6=false,cb=false;
//bool fi0=false,fi1=false,fi2=false,fi3=false;
//bool ss0=false,ss1=false,ss2=false,ss3=false,ss4=false,ss5=false,ss6=false;
//bool lp0=true,lp1=true,lp2=true,lp3=true;
bool cbstatus=false;

// 1- Switch ON 
// 2- Switch OFF
// 3- Analog add 10 
// 4- Analog decrease 10

void initiate()
{
	int i;
	for(i=0;i<k;i++)
	{
		lp[i]=true;
	}

	for(i=0;i<2*k-1;i++)
	{
		fd[i]=false;
	}

	for(i=0;i<k;i++)
	{
		fi[i]=false;
	}

	for(i=0;i<2*k-1;i++)
	{
		ss[i]=0;
	}

}

/*

   Register[1]: Circuit Breaker LED,  

   Register[2]: Fault Detector 0 (FD0), 
   Register[3]: Fault Detector 1 (FD1),
   Register[4]: Fault Detector 2 (FD2), 
   Register[5]: Fault Detector 3 (FD3), 
   Register[6]: Fault Detector 4 (FD4),
   Register[7]: Fault Detector 5 (FD5), 
   Register[8]: Fault Detector 6 (FD6), 

   Register[9]: Fault Indicator 0 (FI0),
   Register[10]: Fault Indicator 1 (FI1), 
   Register[11]: Fault Indicator 2 (FI2), 
   Register[12]: Fault Indicator 3 (FI3),

   Register[13]: Section Switch 0 (SS0), 
   Register[14]: Section Switch 1 (SS1), 
   Register[15]: Section Switch 2 (SS2),
   Register[16]: Section Switch 3 (SS3), 
   Register[17]: Section Switch 4 (SS4),
   Register[18]: Section Switch 5 (SS5),
   Register[19]: Section Switch 6 (SS6).

   Register[20]: LP0
   Register[21]: LP1
   Register[22]: LP2
   Register[23]: LP3
 */


//Predicts which load points will have power supply with the information of the fault detector values
void predictlp()
{int i;
	//if(cbstatus==true)
	{
		for(i=0;i<k;i++)
		{
			lp[i]=false;
		}

	}
	//	else
	{
		if(fd[0]==false)
		{
			lp[0]=true;
			for(i=1;i<2*k-2;)
			{
				if((fd[i]==false)&&(fd[i+1]==false))
				{
					lp[(i+1)/2]=true;

				}
				else
				{
					break;
				}
				i+=2;
			}
		}

	}
}



CommandStatus SimpleCommandHandler::DirectOperate (const AnalogOutputInt16 &arCommand, size_t aIndex)
{	command=aIndex;
	val=arCommand.GetValue();
	onprocess=1;
	//std::cout<<"\nCHECK Received Command:"<<command;
}


int main(int argc, char* argv[])
{

	int argcnt = 1;
	while(argc > argcnt) {
		if(strcmp(argv[argcnt], LOAD_FLAG) == 0) {
			argcnt++;
			if(argcnt < argc) {
				sscanf(argv[argcnt++], "%i", &k);
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

	initiate();
	// Specify a FilterLevel for the stack/physical layer to use.
	// Log statements with a lower priority will not be logged.
	const FilterLevel LOG_LEVEL = LEV_INFO;

	// This is the main point of interaction with the stack
	DNP3Manager mgr(1); // only 1 thread is needed for a single stack

	// You can optionally subcribe to log messages
	// This singleton logger just prints messages to the console
	mgr.AddLogSubscriber(LogToStdio::Inst());

	// Add a TCPServer to the manager with the name "tcpserver".
	// The server will wait 3000 ms in between failed bind calls.
	auto pServer = mgr.AddTCPServer("tcpserver", LOG_LEVEL, 3000, "127.0.0.1", 20000);

	// You can optionally add a listener to the channel. You can do this anytime and
	// you will receive a stream of all state changes
	pServer->AddStateListener([](ChannelState state) {
			std::cout << "Server state: " << ConvertChannelStateToString(state) << std::endl;
			});

	// The master config object for a slave. The default are
	// useable, but understanding the options are important.
	SlaveStackConfig stackConfig;


	// The DeviceTemplate struct specifies the structure of the
	// slave's database
	DeviceTemplate device(5*k-1,0,0,0,0);
	stackConfig.device = device;
	stackConfig.device.mStartOnline = true;
	stackConfig.slave.mDisableUnsol = true;


	// Create a new slave with a log level, command handler, and
	// config info this	returns a thread-safe interface used for
	// updating the slave's database.

	auto pOutstation = pServer->AddOutstation("outstation", LOG_LEVEL, SuccessCommandHandler::Inst(), stackConfig);

	//	pServer->check=2;

	// You can optionally add a listener to the stack to observer communicate health. You
	// can do this anytime and you will receive a stream of all state changes.
	pOutstation->AddStateListener([](StackState state) {
			std::cout << "outstation state: " << ConvertStackStateToString(state) << std::endl;
			});

	auto pDataObserver = pOutstation->GetDataObserver();


	/*	TimeTransaction tx(pDataObserver);
		tx.Update(Binary(cbstatus, 0), 0);
		int i;	
		for(i=0;i<2*k-1;i++)
		{
		tx.Update(Binary(fd[i], 0), i+1);
		}
		for(i=0;i<k;i++)
		{
		tx.Update(Binary(fi[i], 0), i+2*k);
		}
		for(i=0;i<2*k-1;i++)
		{
		tx.Update(Binary(ss[i], 0), i+3*k);        
		}
		for(i=0;i<k;i++)
		{
		tx.Update(Binary(lp[i], 0), i+5*k-1);
		}



	 */



	//automatically calls Start()/End() and sets time for each measurement
	int i;
	std::string input;
	uint32_t count = 0;
	do {
		if(command!=0)
		{
			//cout<<"Receieved COmmand"<<command<<"!!!\n";	
			if(onprocess==1)
			{

				if(command==1)
				{	std::cout<<"\nCommand 1 Fault between Main and LP0.\nIsolated the line with switches SS1";
					cbstatus=false;
					fd[0]=true;
					fi[0]=true;
					ss[0]=true;
					predictlp();

					printf("\nCB=%d\n",cbstatus);
					for(i=0;i<2*k-1;i++)
					{printf("fd[%d]=%d ",i,fd[i]);}
					printf("\n");
					for(i=0;i<k;i++)
					{printf("fi[%d]=%d",i,fi[i]);}
					printf("\n");
					for(i=0;i<2*k-1;i++)
					{printf("ss[%d]=%d",i,ss[i]);}
					printf("\n");
					for(i=0;i<k;i++)
					{printf("lp[%d]=%d",i,lp[i]);}
					cout<<"\n--------------------------------------\n";

					TimeTransaction tx(pDataObserver);				

					//set ss and fi to value based on the fd CB will be turned off later

					tx.Update(Binary(cbstatus, 0), 0);
					tx.Update(Binary(fd[0], 0), 1);
					tx.Update(Binary(fi[0], 0), 2*k);
					tx.Update(Binary(ss[0],0),3*k);

					command=0;
				}

				else if((command>1)&&(command<=k)) 
				{

					std::cout<<"\nCommand "<<command<<"Fault between LP"<<command-2<<" and LP"<<command-1<<"\nIsolated the line with switches SS"<<2*(command-1)-1<< "and"<<"SS"<<2*(command-1);
					cbstatus=false;
					fd[2*(command-1)-1]=true;
					fd[2*(command-1)]=true;

					fi[command-1]=true;
					ss[2*(command-1)-1]=true;
					ss[2*(command-1)]=true;

					predictlp();
					printf("\nCB=%d\n",cbstatus);
					for(i=0;i<2*k-1;i++)
					{printf("fd[%d]=%d ",i,fd[i]);}
					printf("\n");
					for(i=0;i<k;i++)
					{printf("fi[%d]=%d",i,fi[i]);}
					printf("\n");
					for(i=0;i<2*k-1;i++)
					{printf("ss[%d]=%d",i,ss[i]);}
					printf("\n");
					for(i=0;i<k;i++)
					{printf("lp[%d]=%d",i,lp[i]);}


					TimeTransaction tx(pDataObserver);
					tx.Update(Binary(cbstatus, 0), 0);
					tx.Update(Binary(fd[2*(command-1)-1], 0),2*(command-1) );
					tx.Update(Binary(fd[2*(command-1)], 0),2*(command-1)+1 );
					tx.Update(Binary(fi[(command-1)], 0), 2*k+(command)-1);
					tx.Update(Binary(ss[2*(command-1)-1],0),2*(command-1)+3*k-1);
					tx.Update(Binary(ss[2*(command-1)],0),2*(command-1)+3*k);


					cout<<"\n--------------------------------------\n";
					command=0;
				}

				else if(command==k+1)
				{   std::cout<<"\nCommand "<<k+1<<"Repair Fault between Main and LP0";

					if(fd[0]==false)
					{
						cout<<"\nNO FAULT between main and LP0";
						cout<<"\n--------------------------------------\n";
						onprocess=0;
						continue;
					}
					cbstatus=false;
					fd[0]=false;
					fi[0]=false;
					ss[0]=false;

					predictlp();
					printf("\nCB=%d\n",cbstatus);
					for(i=0;i<2*k-1;i++)
					{printf("fd[%d]=%d",i,fd[i]);}
					printf("\n");
					for(i=0;i<k;i++)
					{printf("fi[%d]=%d",i,fi[i]);}
					printf("\n");
					for(i=0;i<2*k-1;i++)
					{printf("ss[%d]=%d",i,ss[i]);}
					printf("\n");
					for(i=0;i<k;i++)
					{printf("lp[%d]=%d",i,lp[i]);}
					cout<<"\n--------------------------------------\n";

					TimeTransaction tx(pDataObserver);
					tx.Update(Binary(cbstatus, 0), 0);
					tx.Update(Binary(fd[0], 0), 1);
					tx.Update(Binary(fi[0], 0), 2*k);
					tx.Update(Binary(ss[0],0),3*k);


					/*

					   TimeTransaction tx(pDataObserver);
					   tx.Update(Binary(cbstatus, 0), 0);
					   tx.Update(Binary(fd0, 0), 1);
					   tx.Update(Binary(fi0, 0), 8);
					   tx.Update(Binary(ss0,0),12);
					   tx.Update(Binary(lp0, 0), 19);
					   tx.Update(Binary(lp1, 0), 20);
					   tx.Update(Binary(lp2, 0), 21);
					   tx.Update(Binary(lp3, 0), 22);
					 */
					command=0;
				}
				else if((command>k+1)&&(command<=2*k+1))
				{
					std::cout<<"\nCommand"<< command<< "Repair Fault between LP"<<command-2-k<<" and"<<"LP"<<command-1-k;

					if((fd[2*(command-1-k)-1]==false)&&(fd[2*(command-1-k)]==false))
					{
						cout<<"\nNO FAULT between LP"<<command-k-2<<" and LP"<<command-k-1;
						cout<<"\n--------------------------------------\n";
						onprocess=0;
						continue;
					}

					cbstatus=false;
					fd[2*(command-1-k)-1]=false;
					fd[2*(command-1-k)]=false;

					fi[command-k-1]=false;
					ss[2*(command-1-k)-1]=false;
					ss[2*(command-1-k)]=false;

					predictlp();
					printf("\nCB=%d\n",cbstatus);
					for(i=0;i<2*k-1;i++)
					{printf("fd[%d]=%d ",i,fd[i]);}
					printf("\n");
					for(i=0;i<k;i++)
					{printf("fi[%d]=%d",i,fi[i]);}
					printf("\n");
					for(i=0;i<2*k-1;i++)
					{printf("ss[%d]=%d",i,ss[i]);}
					printf("\n");
					for(i=0;i<k;i++)
					{printf("lp[%d]=%d",i,lp[i]);}
					cout<<"\n--------------------------------------\n";


					TimeTransaction tx(pDataObserver);
					tx.Update(Binary(false, 0), 0);
					tx.Update(Binary(false, 0),2*(command-1-k) );
					tx.Update(Binary(false, 0),2*(command-k-1)+1 );
					tx.Update(Binary(false, 0), (command-1-k)+2*k);
					tx.Update(Binary(false,0),2*(command-1-k)+3*k-1);
					tx.Update(Binary(false,0),2*(command-1-k)+3*k);



					command=0;

				}

				onprocess=0;
			}//end on process
		}//end command!=0		 
		if(command==0)
		{	
			if(flag==0)
			{
				flag=1;
				cbstatus=false;
				TimeTransaction tx(pDataObserver);
				tx.Update(Binary(cbstatus,0),0);
				for(i=0;i<2*k-1;i++)
				{
					tx.Update(Binary(false, 0), i+1);
				}
				for(i=0;i<k;i++)
				{
					tx.Update(Binary(false, 0), i+2*k);
				}
				for(i=0;i<2*k-1;i++)
				{
					tx.Update(Binary(false, 0), i+3*k);
				}


			}		

			//	cout<<sizeof(device);

			//count+=atoi(input.c_str());
		}

		sleep(1);
	}while(true);

	return 0;
}
