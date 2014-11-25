LBNL Physics-Based Intrusion Detection Bro Modules
================================================== 

Water Heater Scenario
---------------------

This directory includes a simulation of a physical system, a set of Bro and Broccoli signatures that analyze communication with the physical system and compare the effects of that communication with a physical simulation of the device.

In particular, this scenario monitors the activities of a water heater system that can be controlled remotely.  The simulation is implemented using [libmodbus](http://libmodbus.org/), a C library extension.

The monitoring system is implemented using the [Bro Network Security Monitor](https://www.bro.org/).  There are two primary Bro modules:

1. Network Module - Monitors the network activities which do not require any knoweledge of physical system

2. Physical Module - Has the specific knoweledge of physics of the system and protects the system based on that.

We note that the modular design is intentional for two reasons.  First, while the water heater example is potentially useful on its own, it is primarily intended to be illustrative of certain types of physical systems and how one might develop Bro scripts to monitor such systems.  The design can be used as a template for developing scripts to monitor other physical systems.  Second, while we personally believe that Bro represents a superb environment for monitoring network activity, and while Bro also represents a reasonable environment for monitoring the physics of devices, there are some environments which could be even better suited depending on the characteristics of the device physics being monitored, for example in a situation where mathematical functions are more readily available in a different environment such as Matlab or Simulink.

Our code implemented using both Bro and also Broccoli - the Bro Client Communication Library.  The Broccoli program acts as "middleware" communication between the two Bro scripts.

Steps to run 
------------

1. Compile the Broccoli-based C program and the simulation of the water heater by giving command "make". (In the Makefile, change the Broccoli variable to the address of the installed location of Bro)

2. Run the Bro scripts by running the shell scripts in different terminals (network.sh and physical.sh) by running the commands sh <name_of_shell_script>.sh (In the shell scripts change the BRO_PATH variable to the location of bro installation.)

3. There are three executables for the simulation: server, monitor client, and command client.

The server simulates the physical system and calculates the physical properties like current water temperature by using the other parameter values.

Monitor client is a process which can periodically read the status of the device.

Command client is a process that can send commands like turn on or off the heater, or set a different target temparature etc.

Simulation details
------------------

The simulation models the behavior of a simple hot water heater.  The water is heated by a simple heating element that is controlled by a simple control element with a programmable amount of hysteresis (i.e. over/under shoot).  Heat is lost through simple conduction, through an insulated wall, to the ambient environment.  Mathematical model is based on Newtonian exponential cooling model.  Constants have been computed externally - details provied as needed.

This model executes as a thread that is periodically inovoked to compute, via heat loss and gain, the current temperature of the contained water.  Calculations are based on elapsed seconds and time can be accelerated by scaling the number of seconds calculated by the model as opposed to the actual thread sleep interval.  Note the use of mutex to protect reads and writes to the modbus register structure. This mutex is shared with the main thread that is responsible for communications processing.


The following registers are implemented
---------------------------------------

- coil #0 is used to enable heater operation 
- input bit #0 reflects the current powered state of the heater 
- input register #0 is the current water temperature in deg. F x 10 
- holding register #0 is current target temperature in deg. F x 10

The heater model is enabled and set to an initial target temperature within the main routine at startup.


Commands to run the whole system
--------------------------------

Execute the shell script "execute.sh" to automatically compile and execute all the steps mentioned in "Steps to Run"

Simulation:

Terminal 1: make 
Terminal 1 : ./server -p <port number> -n <ip address>   

(*Note:* port number for Modbus TCP is 502. If you are using a port number less than 1024, you need to become a super user)

Terminal 2: ./monitor_client -p <port number> -n <ip address>

Terminal 3: ./command_client -p 502 -n 127.0.0.1 

(After executing this command, press help for command statements to operate this client)

Network Monitoring system:

Terminal 4: sh network.sh 

(Executes level0.bro which implments the network module)

Terminal 5: sh physical.sh  

(Executes level1.bro which monitors the physical module)

Terminal 6 : ./bro
