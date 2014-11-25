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
#include <opendnp3/SimpleDataObserver.h>
#include <opendnp3/DNP3Manager.h>
#include <opendnp3/IChannel.h>
#include <opendnp3/IMaster.h>
#include <opendnp3/MasterStackConfig.h>
#include <opendnp3/ICommandProcessor.h>
#include <opendnp3/TimeTransaction.h>

#include <iostream>
#include <future>
#define LOAD_FLAG "-l"
using namespace std;
using namespace opendnp3;
int k;

class Observer : public SimpleDataObserver
{
private:
        static Observer mInstance;
protected:
        Observer() : SimpleDataObserver([](const std::string&) {})
        {}
public:
        static Observer* Inst() {
                return &mInstance;
        }
};



class test: public IDataObserver
{
public:
		void _Start(){}
		void _End(){}
		void _Update(const opendnp3::Binary& check, size_t k){
			//TimeTransaction t(this);
			//	this->Update(check,k);
		}
		void _Update(const opendnp3::Analog& check, size_t k){
			//TimeTransaction t(this);
			//      this->Update(check,k);

		}
		void _Update(const opendnp3::Counter& check, size_t k){
			//	TimeTransaction t(this);
			//      this->Update(check,k);

		}
		void _Update(const opendnp3::ControlStatus& check, size_t k){
			//TimeTransaction t(this);
			//              this->Update(check,k);

		}
		void _Update(const opendnp3::SetpointStatus& check, size_t k){
			//TimeTransaction t(this);
			//              this->Update(check,k);
		}

};


int main(int argc, char* argv[])
{//	observer *test=new observer;
//	test t1;
test *observer= new test;
	const Binary *bin=new Binary (5*k-1);
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

	// Specify a FilterLevel for the stack/physical layer to use.
	// Log statements with a lower priority will not be logged.
	const FilterLevel LOG_LEVEL = LEV_INFO;

	// This is the main point of interaction with the stack. The
	// AsyncStackManager object instantiates master/slave DNP
	// stacks, as well as their physical layers.
	DNP3Manager mgr(1); // 1 stack only needs 1 thread

	// You can optionally subcribe to log messages
	// This singleton logger just prints messages to the console
	mgr.AddLogSubscriber(LogToStdio::Inst());

	// Connect via a TCPClient socket to a slave.  The server will
	// wait 3000 ms in between failed connect calls.
	auto pClient = mgr.AddTCPClient("tcpclient", LOG_LEVEL, 3000, "127.0.0.1", 20000);

	// You can optionally add a listener to the channel. You can do this anytime and
	// you will receive a stream of all state changes
	pClient->AddStateListener([](ChannelState state) {
			std::cout << "Client state: " << ConvertChannelStateToString(state) << std::endl;
			});

	//auto pDataObserver= pClient->IDataObserver;

	// The master config object for a master. The default are
	// useable, but understanding the options are important.
	MasterStackConfig stackConfig;


	// Create a new master on a previously declared port, with a
	// name, log level, command acceptor, and config info. This
	// returns a thread-safe interface used for sending commands.
	auto pMaster = pClient->AddMaster(
			"master",						// stack name
			LOG_LEVEL,						// log filter level
			NullDataObserver::Inst(),
			//Observer::Inst(),
			//test->Inst(),
			//observer,	// callback for data processing
			//test,
			stackConfig						// stack configuration
			);


	// You can optionally add a listener to the stack to observer communicate health. You
	// can do this anytime and you will receive a stream of all state changes.
	pMaster->AddStateListener([](StackState state) {
			std::cout << "master state: " << ConvertStackStateToString(state)<<"done" << std::endl;
			});

	auto pCmdProcessor = pMaster->GetCommandProcessor();
	cout<<"0. Exit\n";
	cout << "1. Create fault between Main and LP0\n";
	for(int i=0;i<k-1;i++)
	{
		cout<<"\n"<<i+2<<". Create Fault between LP"<<i<<"and LP"<<i+1<<"\n";
	}
	cout <<"\n"<< k+1<<". Repair fault between Main and LP0\n";

	for(int i=0;i<k;i++)
	{
		cout<<"\n"<<k+2+i<<". Repair Fault between LP"<<i<<"and LP"<<i+1<<"\n";
	}
	cout<<"\nEnter your choice: -\n";

	std::string cmd;
	int code=1;
	do {
		cout << "\n 0. Exit\n\n1.Create fault between Main and LP0\n";
		for(int i=0;i<k-1;i++)
		{
			cout<<"\n"<<i+2<<".Create Fault between LP"<<i<<"and LP"<<i+1<<"\n";
		}
		cout <<"\n"<< k+1<<".Repair fault between Main and LP0\n";
		for(int i=0;i<k-1;i++)
		{
			cout<<"\n"<<k+2+i<<".Repair Fault between LP"<<i<<"and LP"<<i+1<<"\n";
		}
		cout<<" \nEnter your choice: -\n";


		std::cin >> code;
		cout<<"\n\n\n Command Sent"<<code<<"\n\n";
		if(code == 0) break;
		else {
			//ControlRelayOutputBlock crob(IntToControlCode(code),1,10,100);
			//cout<<"\nCROB string-- "<<crob.ToString();
			//cout<<"\nCODE:  "<<crob.GetCode();
			promise<CommandResponse> selectResult;
			pCmdProcessor->DirectOperate(AnalogOutputInt16(0), code, [&](CommandResponse cr) {
					selectResult.set_value(cr);
					//cout<<"\nDONE\n";
					});
			CommandResponse rsp = selectResult.get_future().get();
			std::cout << "Select/Operate result: " << ToString(rsp.mResult) << std::endl;
		}
	}
	while(true);

	return 0;
}

