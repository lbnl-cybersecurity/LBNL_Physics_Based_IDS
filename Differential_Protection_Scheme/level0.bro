#*** Copyright Notice ***
#level0.bro -Differential Protection Scheme, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved.

#If you have questions about your rights to use or distribute this software, please contact Berkeley Lab's Technology Transfer Department at  TTD@lbl.gov.

#NOTICE.  This software is owned by the U.S. Department of Energy.  As such, the U.S. Government has been granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, and perform publicly and display publicly.  Beginning five (5) years after the date permission to assert copyright is obtained from the U.S. Department of Energy, and subject to any subsequent five (5) year renewals, the U.S. Government is granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
#****************************


#*** License Agreement ***
#"level0.bro -Differential Protection Scheme, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved."

#Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

#(1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

#(2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

#(3) Neither the name of the University of California, Lawrence Berkeley National Laboratory, U.S. Dept. of Energy nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#You are under no obligation whatsoever to provide any bug fixes, patches, or upgrades to the features, functionality or performance of the source code ("Enhancements") to anyone; however, if you choose to make your Enhancements available either publicly, or directly to Lawrence Berkeley National Laboratory, without imposing a separate written license agreement for such Enhancements, then you hereby grant the following license: a  non-exclusive, royalty-free perpetual license to install, use, modify, prepare derivative works, incorporate into other computer software, distribute, and sublicense such enhancements or derivative works thereof, in binary and source code form.
#***************************  

@load frameworks/communication/listen
module State;
type Idx: record {
variable: string;
};

type Val: record {
value: double;
};

export {
# Create an ID for our new stream. By convention, this is
# called "LOG".
	redef enum Log::ID += { LOG };

# Define the fields. By convention, the type is called "Info".

	type Info: record {
attack: string &log &optional;
	};

# Define a hook event. By convention, this is called
# "log_<stream>".
}

redef Communication::nodes += {
	["broping"] = [$host = 127.0.0.1, $events = /.*/, $connect=F, $ssl=F]
};



redef Communication::listen_port = 47758/tcp;

global slave_ip: addr=127.0.0.1;
global master_ip: addr=127.0.0.1;

global readreq: event(start_address: count);
global writereq: event();

event bro_init() &priority=5
    {
    # Create the stream. This also adds a default filter automatically.
    Log::create_stream(State::LOG, [$columns=Info]);
    }


event modbus_read_holding_registers_request(c: connection , headers: ModbusHeaders , start_address: count , quantity: count )
{
	print "read req observed, forwarding details to higher level script";
	event readreq(start_address);
}
event modbus_write_single_register_request (c: connection, headers: ModbusHeaders, address: count, value: count)
{
	print "write req observed, forwarding details to higher level script";
	event writereq();
}

event modbus_exception(c: connection, headers: ModbusHeaders, code: count)
{
	if(code==6)
	{
		Log::write(LOG, [$attack="Slave Device busy response, possibly crafted by attacker to delay response to avoid detection ",$value=code,$tstamp=network_time()]);
	}
	else if(code==5)
	{
		Log::write(LOG, [$attack="Code delay, possible attempt to avoid timeout Exception Code-",$value=code,$tstamp=network_time()]);
	}
	else
	{
		Log::write(LOG, [$attack="Exception Code-",$value=code,$tstamp=network_time()]);
	}

}

event modbus_message(c: connection, headers: ModbusHeaders, is_orig: bool)
{
#Check for unknown source and destination ip address
	if(!((c$id$orig_h==master_ip)||(c$id$orig_h==slave_ip)))
	{
		Log::write(LOG, [$attack="Unknown source IP address",$address=c$id$orig_h,$tstamp=network_time()]);

	}
	if(!((c$id$resp_h==master_ip)||(c$id$resp_h==slave_ip)))
	{
		Log::write(LOG, [$attack="Unknown source IP address",$address=c$id$resp_h,$tstamp=network_time()]);

	}

#Read Device Information.
	if(headers$function_code==43)
	{
		Log::write(LOG, [$attack="Attempted to Read Device Identification",$tstamp=network_time()]);
	}
#Report Server information
	else if(headers$function_code==17)
	{
		Log::write(LOG, [$attack="Report Server Information",$tstamp=network_time()]);
	}

}
