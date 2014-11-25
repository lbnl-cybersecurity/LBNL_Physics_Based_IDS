#*** Copyright Notice ***
#Auto Distribution DNP3, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved.

#If you have questions about your rights to use or distribute this software, please contact Berkeley Lab's Technology Transfer Department at  TTD@lbl.gov.

#NOTICE.  This software is owned by the U.S. Department of Energy.  As such, the U.S. Government has been granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, and perform publicly and display publicly.  Beginning five (5) years after the date permission to assert copyright is obtained from the U.S. Department of Energy, and subject to any subsequent five (5) year renewals, the U.S. Government is granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
#****************************


#*** License Agreement ***
#"Auto Distribution DNP3, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved."

#Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

#(1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

#(2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

#(3) Neither the name of the University of California, Lawrence Berkeley National Laboratory, U.S. Dept. of Energy nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#You are under no obligation whatsoever to provide any bug fixes, patches, or upgrades to the features, functionality or performance of the source code ("Enhancements") to anyone; however, if you choose to make your Enhancements available either publicly, or directly to Lawrence Berkeley National Laboratory, without imposing a separate written license agreement for such Enhancements, then you hereby grant the following license: a  non-exclusive, royalty-free perpetual license to install, use, modify, prepare derivative works, incorporate into other computer software, distribute, and sublicense such enhancements or derivative works thereof, in binary and source code form.
#***************************   

# Bro script for automated distribution system
# This Bro script raises an alert by adding an entry in log file for the following cases-
# 1. CB is turned OFF whenever there is a fault in any of the lines and the faulty line 
#    is not isolated.
# 2. CB is turned ON when there is no fault or if the faulty line is isolated.
# 3. Fault Indicator is turned ON when the fault detector is turned OFF
# 4. In between LP1,LP2 and LP3 there are 2 fault detectors between 2 load points, if one 
#    of them is ON and another one is OFF, then the system is comprimised.

@load frameworks/communication/listen


# Let's make sure we use the same port no matter whether we use encryption or not:
redef Communication::listen_port = 47759/tcp;

# Redef this to T if you want to use SSL.
redef Communication::listen_ssl = F;

# Set the SSL certificates being used to something real if you are using encryption.
#redef ssl_ca_certificate   = "<path>/ca_cert.pem";
#redef ssl_private_key      = "<path>/bro.pem";
#global ping_log = open_log_file("ping");

global monitor: event(value: count);


#K - Number of sensors
global k: count=4;
global registers: vector of count[5*k-1];
global counter1: count=0;
global cb: count;
global fd: vector of count[2*k-1];
global fi: vector of count[k];
global ss: vector of count[2*k-1];
global cnt: count=0;
global flag: count=0;
global change: count=0;

export {
# Create an ID for our new stream. By convention, this is
# called "LOG".
	redef enum Log::ID += { LOG };

# Define the fields. By convention, the type is called "Info".

	type Info: record {
attack: string &log &optional;
	};

}


redef Communication::nodes += {
	["broping"] = [$host = 127.0.0.1, $events = /.*/, $connect=F, $ssl=F]
};


event monitor(data_value: count)
{
	if(data_value==255){}
	else
	{
		counter1=counter1+1;
#	print data_value,counter1;

		if(counter1==1)
		{
			if(data_value==0)
			{
				cb=data_value;
				registers[0]=0;
			}
			else if(data_value==128) 
			{
				cb=1;
				registers[0]=1;
			}
		}
		else if(counter1<=2*k)
		{	
			if(data_value==0)
			{
				fd[counter1-2]=data_value;
				registers[counter1-2]=data_value;
			}
			else if(data_value==128)
			{
				fd[counter1-2]=1;
				registers[counter1-2]=1;		
			}

		}
		else if(counter1<=3*k)
		{
			if(data_value==0)
			{
				fi[counter1-2*k-1]=data_value;
				registers[counter1-2]=0;			
			}
			else if(data_value==128)
			{
				fi[counter1-2*k-1]=1;
				registers[counter1-2]=1;
			}

		}
		else if(counter1<=5*k-1)
		{	if(data_value==0)
			{
				ss[counter1-3*k-1]=data_value;
				registers[counter1-2]=0;			
			}
			else if(data_value==128)
			{	ss[counter1-3*k-1]=1;
				registers[counter1-2]=1;
			}
		}
		if(counter1==5*k-1)
		{

			counter1=0;

			local j:count=1;
			local i: count=0;
			local min: count=0;
			local max: count=4;
			local temp: count;
#make sure we get the response sent to the monitor client.
#initially we store all the register values into the state, then we store to the states only if there is a change in the value.
			print "CB",cb;
			print "FD",fd,"FI",fi,"SS",ss;
			print "Registers",registers;

		



#check for case where cb is off when the faulty line is not isolated
#also check for CB being turned ON even when there is no faulty line.

		i=0;
		for(x in fi)
		{
			if(i==0)
			{
				if((fi[0]==1)&&(fd[0]==1))
				{
					if((ss[0]==0)&&(cb==0))
					{
						print "CB not working";
						Log::write(LOG, [$attack="CB NOT WORKING"]);
					}

					if((ss[0]==1)&&(cb==1))
					{
						print "CB FALSE POSITIVE";
						Log::write(LOG, [$attack="CB FALSE POSITIVE"]);


					}


				}
			}
			else
			{

				if((fi[i]==1)&&(fd[2*i-1]==1)&&(fd[2*i]==1))
				{
					if((ss[2*i-1]==0)&&(ss[2*i]==0)&&(cb==0))
					{
						print "CB NOT WORKING!";
						Log::write(LOG, [$attack="CB NOT WORKING"]);

					}
					if((ss[2*i-1]==1)&&(ss[2*i]==1)&&(cb==1))
					{
						print "CB FALSE POSITIVE!";
						Log::write(LOG, [$attack="CB FALSE POSITIVE"]);


					}
				}
			}
			i=i+1;
		}


#Since there are 2 Fault detectors between 2 LP, if one is ON and other is OFF, system is comprimised and raise alert
		i=0; 
		for(x in fi)
		{
			if(i>=1)
			{

				if(fd[2*i-1]!=fd[2*i])
				{

					print "FD Attacked";

					Log::write(LOG, [$attack="FD ATTACKED"]);

				}
			}
			i=i+1;
		}
#If fault indicator is attacked, i.e, if it turns ON when there is no fault 
		i=0;
		for(x in fi)
		{
			if(i==0)
					{
						if((fi[0]!=fd[0])&&(fd[0]==0)&&(ss[0]==0))
						{

							print "FI0 attacked";
							Log::write(LOG, [$attack="FI0 ATTACKED"]);


						}
					}
					else
					{

						if(((fd[2*i-1]!=fi[i])&&(fd[2*i-1]==0))||((fd[2*i]!=fi[i])&&(fd[2*i]==0)))
						{
							print "FI1 attacked";
							Log::write(LOG, [$attack="FI0 ATTACKED"]);
						}

					}
			i=i+1;



		}


	} #end if step==5k-1
} #first else
}
