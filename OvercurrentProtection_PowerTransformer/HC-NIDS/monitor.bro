#*** Copyright Notice ***
#"OverCurrentProtection_PowerTransformer", Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved.

#If you have questions about your rights to use or distribute this software, please contact Berkeley Lab's Technology Transfer Department at  TTD@lbl.gov.

#NOTICE.  This software is owned by the U.S. Department of Energy.  As such, the U.S. Government has been granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, and perform publicly and display publicly.  Beginning five (5) years after the date permission to assert copyright is obtained from the U.S. Department of Energy, and subject to any subsequent five (5) year renewals, the U.S. Government is granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
#****************************


#*** License Agreement ***
#"OverCurrentProtection_PowerTransformer", Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved."

#Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

#(1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

#(2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

#(3) Neither the name of the University of California, Lawrence Berkeley National Laboratory, U.S. Dept. of Energy nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#You are under no obligation whatsoever to provide any bug fixes, patches, or upgrades to the features, functionality or performance of the source code ("Enhancements") to anyone; however, if you choose to make your Enhancements available either publicly, or directly to Lawrence Berkeley National Laboratory, without imposing a separate written license agreement for such Enhancements, then you hereby grant the following license: a  non-exclusive, royalty-free perpetual license to install, use, modify, prepare derivative works, incorporate into other computer software, distribute, and sublicense such enhancements or derivative works thereof, in binary and source code form.
#***************************  

@load frameworks/communication/listen

module transformer_protection_anomalies;
@load base/protocols/modbus
type Idx: record {
variable: string;
};

type Val: record {
value: double;
};

export {
	redef enum Log::ID += { LOG };

	type Info: record {
attack: string &log &optional;
expected_state: string &log &optional;
observed_state: string &log &optional;
current_value: count &log &optional;
threshold_value: count &log &optional;
time_delay: double &log &optional;
value: count &log &optional;
tstamp: time &log &optional;
cb1: bool &log &optional;
cb2: bool &log &optional;	
};
}

# Hard Coded Port/Protocol
redef Communication::listen_port = 47758/tcp;
redef Communication::listen_ssl = F;



global cb_values: event(coils: ModbusCoils);
global current_value: event(current: count);
global current: count;
global state: count=0; #state 0 - read request, state 1 -rread response, sate 2- write request, state 3 -write response
global threshold: count = 370; 
global restoration: count=0; #number of cycles of restoration - 0 if there is no overcurrent
global flag: count =0; 
global time_prev: time=0;
global states:vector of string = {"READ_REGISTER_REQUEST","READ_REGISTER_RESPONSE","WRITE_MULTIPLE_COIL_REQUEST","WRITE_MULTIPLE_COIL_RESPONSE"};
# We communicate with a Broccoli-based program on the local host
redef Communication::nodes += {
	["broping"] = [$host = 127.0.0.1, $events = /ping/, $connect=F, $ssl=F]
};

event bro_init() &priority=5
    {
    # Create the stream. This also adds a default filter automatically.
    Log::create_stream(transformer_protection_anomalies::LOG, [$columns=Info]);
    }

event modbus_write_multiple_coils_request(c: connection, headers: ModbusHeaders, start_address: count, coils: ModbusCoils)
{

	if (state ==2)
	{
		state =3;
		if (restoration > 0)
		{
			restoration = restoration-1;
		}
		else
		{
			if (current < threshold)
			{
				if( (coils[0]==T) &&(coils[1]==T))
				{
#Things are happening in the expected way
				}
				else
				{
					Log::write(LOG, [$attack="Unexpected CB value", $cb1=coils[0],$cb2=coils[1],$current_value=current,$threshold_value=threshold]);

				}
			}
			else
			{
				if( (coils[0]==F) &&(coils[1]==F))
				{
					restoration=6; #in next 6 cycles expect lesser value of current - restoration persiod
				}
				else
				{
					Log::write(LOG, [$attack="Unexpected CB value",$cb1=coils[0],$cb2=coils[1],$current_value=current,$threshold_value=threshold]);

				}
			}
		}
	}
	else
	{
		Log::write(LOG, [$attack="Anomalous ordering of packets, expected packet - ", $expected_state=states[2],$observed_state=states[state]]);

	}

}
event modbus_write_multiple_coils_response(c: connection, headers: ModbusHeaders, start_address: count, quantity: count)
{


	if (state == 3)
	{
		state=0;
	}
	else
	{
		Log::write(LOG, [$attack="Anomalous ordering of packets, expected packet - ", $expected_state=states[3],$observed_state=states[state]]);
	}


}


event modbus_read_input_registers_request(c: connection, headers: ModbusHeaders, start_address: count, quantity: count)
{
	if (state == 0)
	{
		state =1;
	}
	else
	{
		 Log::write(LOG, [$attack="Anomalous ordering of packets, expected packet - ", $expected_state=states[0],$observed_state=states[state]]);

	}



#Check for time difference between two consecutive read request packets
	if(flag == 0)
	{
		flag=1;
		time_prev=network_time();
	}
	else
	{
		if(restoration == 0)
		{
			if (time_to_double(network_time())-time_to_double(time_prev) > 7)
			{
				 Log::write(LOG, [$attack="Anomalous time difference between consecutive cycles", $time_delay=time_to_double(network_time())-time_to_double(time_prev)]);

			}
		}
		time_prev=network_time();

	}
}	

#Check for generic network monitoring stuff

event modbus_read_input_registers_response(c: connection, headers: ModbusHeaders, registers: ModbusRegisters)
{


	if (state == 1)
	{
		current=registers[0];
		state=2;
	}
	else
	{
	 Log::write(LOG, [$attack="Anomalous ordering of packets, expected packet - ", $expected_state=states[1],$observed_state=states[state]]);

	}


}


event modbus_message(c: connection, headers: ModbusHeaders, is_orig: bool)
{

# Check for unknown source and destination ip address
# Read Device Information.
	if(headers$function_code==43)
	{
		Log::write(LOG, [$attack="Attempted to Read Device Identification", $tstamp=network_time()]);
	}

# Report Server information 
# Slave ID Request Information
	else if(headers$function_code==17)
	{
		Log::write(LOG, [$attack="Report Server Information", $tstamp=network_time()]);
	}

}

event modbus_exception(c: connection, headers: ModbusHeaders, code: count)
{
# Preset Single Register
	if(code==6)
	{
		Log::write(LOG, [$attack="Slave Device busy response, possibly crafted by attacker to delay response to avoid detection ", $value=code,$tstamp=network_time()]);
	}

# Force Single Coil
	else if(code==5)
	{
		Log::write(LOG, [$attack="Code delay, possible attempt to avoid timeout Exception Code-", $value=code,$tstamp=network_time()]);
	}

	else
	{
		Log::write(LOG, [$attack="Exception Code-", $value=code,$tstamp=network_time()]);
	}

}


