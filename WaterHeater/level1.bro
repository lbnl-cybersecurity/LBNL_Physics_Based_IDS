#*** Copyright Notice ***
#"Water Heater monitoring system, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved.

#If you have questions about your rights to use or distribute this software, please contact Berkeley Lab's Technology Transfer Department at  TTD@lbl.gov.

#NOTICE.  This software is owned by the U.S. Department of Energy.  As such, the U.S. Government has been granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, and perform publicly and display publicly.  Beginning five (5) years after the date permission to assert copyright is obtained from the U.S. Department of Energy, and subject to any subsequent five (5) year renewals, the U.S. Government is granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
#****************************


#*** License Agreement ***
#"Water Heater monitoring system, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved."

#Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

#(1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

#(2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

#(3) Neither the name of the University of California, Lawrence Berkeley National Laboratory, U.S. Dept. of Energy nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#You are under no obligation whatsoever to provide any bug fixes, patches, or upgrades to the features, functionality or performance of the source code ("Enhancements") to anyone; however, if you choose to make your Enhancements available either publicly, or directly to Lawrence Berkeley National Laboratory, without imposing a separate written license agreement for such Enhancements, then you hereby grant the following license: a  non-exclusive, royalty-free perpetual license to install, use, modify, prepare derivative works, incorporate into other computer software, distribute, and sublicense such enhancements or derivative works thereof, in binary and source code form.
#***************************

# This Bro script raises an alert by adding an entry in log file for the following cases:
# 1. When the Boiler is Enabled or Disabled
# 2. When the Temp is set above 1000 and the heater is enabled
# 3. Whenever current temparature increases by 0.1F (Temp during beginning of script execution
#    is taken as reference, then for every 0.1F increase an entry is made in log file)

@load frameworks/communication/listen

# Hard Coded Port/Protocol
# The Bro scripts send and receive events to broccoli via listen port 47759.
redef Communication::listen_port = 47759/tcp;
redef Communication::listen_ssl = F;

#Event triggered when heater is turned on or off
global turnonoff: event(value: count);

#Event triggered when there is a command to set a target temparature
global settemp: event(value: count);

#Event triggered for a read temparature command
global readtemp: event(value: count);
type Info: record {
	coil: count &log &optional;
	targ: double &log &optional;
	temp: double &log &optional;
};



global button: count=1;
global tprev: double=0;
global flag: bool=T;

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


event turnonoff(value: count)
{
	print "turnonoff",value;
	button=value;

}

event settemp(value: count)
{

	print "settemp",value;
	fmt("ping received se %d",button);
	local t:double=value/10;
	if((t>1000)&&(button==1)){
		print "heater turned on when temp too high",t;
		Log::write(LOG,[$targ=t]);
	}


}
event readtemp(value: count)
{
	local temp:double=value;

	# Current Temp of Boiler
	if(flag==F)
	{
		local t:double=temp/10;
		if((t-tprev)>.01)
		{
			print t;
			print "high rate of increase in temp",t-tprev;
			#   Log::write(LOG,[$temp=t]);
		}
		tprev=t;
	}
	else
	{
		flag=F;
		tprev=temp/10.0;

	}
}
