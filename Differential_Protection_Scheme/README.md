LBNL Physics-Based Intrusion Detection Bro Modules
================================================== 

Differential Protection Scheme Scenario
---------------------------------------

This directory includes a simulation of a physical system, a set of Bro and Broccoli signatures that analyze communication with the physical system and compare the effects of that communication with a physical simulation of the device.

In particular, this scenario monitors the activities of a differential protection scheme --- a protection mechanism to find a fault in an electrical line.  The current phase values at two points in the line are compared and a difference in the phase values imply a fault in the line.

We used two relays connected to circuit breakers to measure the phase difference. 

A positive phase difference signifies an internal fault where the circuit breakers isolate the transmission line from transmission system.

One of the relays is the Modbus master and the other relay is the Modbus slave. 

The master polls the slave for the phase value, then compares the phase values and if there is a phase difference, it then signifies an internal fault. 

The circuit breaker (CB) in the master will be turned ON and a signal is sent to slave to turn on its circuit breaker thus protecting the transmission line from fault.

In addition to containing the simulation, Bro, and Broccoli signatures, this directory also includes:

- Traffic_DifferentialProtectionScheme.pcap -- a trace file that can be run with the Bro signatures by using the "bro -r" command

- report_differential_protection_scheme.pdf -- a more detailed description of the differential protection scheme

Implementation
--------------

The monitoring system is implemented using the Bro Network Security Monitor [https://www.bro.org/].  There are two primary Bro modules:

1. Network Module - Monitors the network activities which do not require any knoweledge of physical system

2. Physical Module - Has the specific knoweledge of physics of the system and protects the system based on that.

We note that the modular design is intentional for two reasons.  First, while the water heater example is potentially useful on its own, it is primarily intended to be illustrative of certain types of physical systems and how one might develop Bro scripts to monitor such systems.  The design can be used as a template for developing scripts to monitor other physical systems.  Second, while we personally believe that Bro represents a superb environment for monitoring network activity, and while Bro also represents a reasonable environment for monitoring the physics of devices, there are some environments which could be even better suited depending on the characteristics of the device physics being monitored, for example in a situation where mathematical functions are more readily available in a different environment such as Matlab or Simulink.

Packet sequence
---------------

1. Master sends a Modbus read register request to the slave to read its value.

2. Slave replies by sending a Modbus read register response.

3. Master compares its value with the slave's value.

4. If they are the same, then a Modbus write request is sent to change value of coil 0 to FALSE, otherwise the same coil value is changed to True.



Compilation and execution
-------------------------

1. Compile the Broccoli executable using the Makefile (In the Makefile change the BROCCOLI variable to the address of the installed location of Bro)

2. Run the Bro scripts by running the shell scripts (network.sh and physical.sh) by running the commands sh <name_of_shell_script>.sh [In the shell scripts change the BRO_PATH variable to the location of bro installation] 

3. There are three executables for the simulation: server, monitor client, and command client.  

Server simulates the physical system.  

Monitor client is a process which can periodically read the status of the device.  

Command client is a process which can send commands like turn on or off the CB and phase.


Following registers are implemented
----------------------------------
- Circuit Breaker -  holding register 0
- Phase Slave -  holding register 1

- Initial state is  CB=0, Phase =0.


Commands to run the whole system
---------------------------------

Simulation:

1. Execute the command "sh execute.sh" it will compile and execute the simulation and bro scripts (all steps mentioned in "Compilation and execution is performed in the shell script")

Optional way to execute each step manually -

Simulation:

Terminal 1: make Terminal 1: ./server -p -n

(Note: port number for Modbus TCP is 502. If you are using a port number less than 1024, you need to become a super user)

Terminal 2: ./monitor_client -p -n

Terminal 3: ./command_client -p 502 -n 127.0.0.1

(After executing this command, press help for command statements to operate this client)

Network Monitoring system:

Terminal 4: sh network.sh

(Executes level0.bro which implments the network module)

Terminal 5: sh physical.sh

(Executes diffprot.bro which monitors the physical module)

Terminal 6: ./bro


