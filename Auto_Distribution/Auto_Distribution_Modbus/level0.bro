#*** Copyright Notice ***
#level0.bro -Auto Distribution Modbus, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved.

#If you have questions about your rights to use or distribute this software, please contact Berkeley Lab's Technology Transfer Department at  TTD@lbl.gov.

#NOTICE.  This software is owned by the U.S. Department of Energy.  As such, the U.S. Government has been granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, and perform publicly and display publicly.  Beginning five (5) years after the date permission to assert copyright is obtained from the U.S. Department of Energy, and subject to any subsequent five (5) year renewals, the U.S. Government is granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.
#****************************


#*** License Agreement ***
#"level0.bro -Auto Distribution Modbus, Copyright (c) 2014, The Regents of the University of California, through Lawrence Berkeley National Laboratory (subject to receipt of any required approvals from the U.S. Dept. of Energy).  All rights reserved."

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





#The bro scripts send and receive events to broccoli via listen port 47758.



redef Communication::listen_port = 47758/tcp;


#K - Number of sensors
global k: count=4;

#event triggered for a command to monitor the state of the machine.
global monitor: event(v: vector of count);


global cb: count;
global fd: vector of count[2*k-1];
global ss: vector of count[k];
global fi: vector of count;
global cnt: count=0;
global flag: count=0;
global change: count=0;

event modbus_read_holding_registers_response(c: connection, headers: ModbusHeaders, registers: ModbusRegisters)
{
	print "to monitor";
event monitor(registers);
}

event modbus_message(c: connection, headers: ModbusHeaders, is_orig: bool)
{

#Check for unknown source and destination ip address
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
