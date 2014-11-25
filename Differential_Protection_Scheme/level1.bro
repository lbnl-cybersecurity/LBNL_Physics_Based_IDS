#*** Copyright Notice ***
#level1.bro -Auto Distribution DNP3, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved.

#If you have questions about your rights to use or distribute this software, please contact Berkeley Lab's Technology Transfer Department at  TTD@lbl.gov.

#NOTICE.  This software is owned by the U.S. Department of Energy.  As such, the U.S. Government has been granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, and perform publicly and display publicly.  Beginning five (5) years after the date permission to assert copyright is obtained from the U.S. Department of Energy, and subject to any subsequent five (5) year renewals, the U.S. Government is granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
#****************************


#*** License Agreement ***
#"level1.bro -Auto Distribution DNP3, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved."

#Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

#(1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

#(2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

#(3) Neither the name of the University of California, Lawrence Berkeley National Laboratory, U.S. Dept. of Energy nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#You are under no obligation whatsoever to provide any bug fixes, patches, or upgrades to the features, functionality or performance of the source code ("Enhancements") to anyone; however, if you choose to make your Enhancements available either publicly, or directly to Lawrence Berkeley National Laboratory, without imposing a separate written license agreement for such Enhancements, then you hereby grant the following license: a  non-exclusive, royalty-free perpetual license to install, use, modify, prepare derivative works, incorporate into other computer software, distribute, and sublicense such enhancements or derivative works thereof, in binary and source code form.
#***************************  

@load frameworks/communication/listen
module State;
@load base/protocols/modbus

# Let's make sure we use the same port no matter whether we use encryption or not:
redef Communication::listen_port = 47759/tcp;

# Redef this to T if you want to use SSL.
redef Communication::listen_ssl = F;

# Set the SSL certificates being used to something real if you are using encryption.
#redef ssl_ca_certificate   = "<path>/ca_cert.pem";
#redef ssl_private_key      = "<path>/bro.pem";
#global ping_log = open_log_file("ping");


type Info: record {
coil: count &log &optional;
targ: double &log &optional;
temp: double &log &optional;
};



global step:count=0;

global t1:double;
global t2:double;
global t:double=0;
global timee:count=2;
global slave_ip: addr=192.168.0.1;
global master_ip: addr=192.168.0.2;
global comp_ip: addr=192.168.0.241;


global registers: vector of count;
export {
# Create an ID for our new stream. By convention, this is
# called "LOG".
	redef enum Log::ID += { LOG };

# Define the fields. By convention, the type is called "Info".

	type Info: record {
attack: string &log &optional;
address: addr &log &optional;
value: count &log &optional;
tstamp: time &log &optional;
	
};

}
event bro_init() {
      Log::create_stream(LOG, [$columns=Info]);
      Log::write(LOG, [$attack="Monitoring Started",$tstamp=network_time()]);
} # When bro starts


redef Communication::nodes += {
	["broping"] = [$host = 127.0.0.1, $events = /.*/, $connect=F, $ssl=F]
};

global readreq: event(start_address: count);
global writereq: event();

event readreq(start_address: count)
{
	print "read req details received";
		if(timee==2)
	{
		t1=time_to_double(network_time());
		timee=1;
	}
	else if(timee==0)
	{
		t1=time_to_double(network_time());
		t=t1-t2;
		timee=1;
	}
	else
	{
		timee=0;
		t2=time_to_double(network_time());
		t=t2-t1;
	}

	if(((t>.040)||(t<.020))&&(timee!=2))
	{
#print t;
#Log::write(LOG, [$attack="Anomaly in the time difference in the packets",$tstamp=network_time()]);
	}
	if(start_address>0)
	{
		print "Possible Crafted modbus packet, trying to read unexpected address, possible network reconnaissance";
		Log::write(LOG, [$attack="Possible Crafted modbus packet, trying to read unexpected address network reconnaissance",$tstamp=network_time()]);
	}
	if(step==0)
	{
		step=1;
	}
	else if(step==1)
	{
		Log::write(LOG, [$attack="Unacceptable packet sequence: Repeated Read requests observed, possible DOS attack",$tstamp=network_time()]);
		print "Unacceptable packet sequence: Repeated Read requests observed, possible DOS attack";
	}
	else if(step==2)
	{
		step=1;
	}
}

event writereq()
{
	print "write req details received";
#Check for ordering of packets - Read -> Write
	if(step==0)
	{
		Log::write(LOG, [$attack="Unauthorized Write request observed, Unauthorized control of devices",$tstamp=network_time()]);
		print "Unauthorized Write request observed, Unauthorized control of devices";
	}
	else if(step==2)
	{
		Log::write(LOG, [$attack="Unauthorized Write request observed, Unauthorized control of devices",$tstamp=network_time()]);
		print "Unauthorized Write request observed, Unauthorized control of devices";

	}
	else if(step==1)
	{
		step=2;
	}
}
