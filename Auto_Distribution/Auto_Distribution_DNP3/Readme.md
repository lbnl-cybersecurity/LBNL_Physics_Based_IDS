LBNL Physics-Based Intrusion Detection Bro Modules
================================================== 

Automated Distribution System Scenario - Modbus DNP3 Version
-----------------------------------------------------------
This directory includes a simulation of a physical system, a set of Bro and Broccoli signatures that analyze communication with the physical system and compare the effects of that communication with a physical simulation of the device.

In particular, we consider an automated distribution system that consists of a circuit breaker (CB) and four load points (LP).  We assume that each load point is equipped with two section switches (SS) and a fault detector exists besides each section switch. When a fault occurs on the line between two load points, the fault is detected by the fault detectors and the main station is updated about the occurrence. Then, the main station enables the circuit breaker and the circuit breaker de-energizes the whole distribution line.  Based on where the fault occurred, the main station enables the section switches on the faulted line in order to isolate the specific section of the distribution line. Then, the main station closes (disables) the circuit breaker and energizes the load points prior to the faulted line. When the fault is fixed, the enabled section switches are disabled and the whole distribution line is re-energized.

Please refer to "AutomatedDistributionSystem.pdf" and http://www.escholarship.org/uc/item/1q63z548 for further details about the system model and the NIDS.

The mapping from fault detectors to holding registers that we used is as follows:

Please refer to the Auto Distribution System document for further details about the system

Register Mapping--
k- Number of load poinnts

Holding Register[1]: Circuit Breaker LED,  

Holding Register[2]: Fault Detector 0 , 
.
.
.
Holding Register[2*k]: Fault Detector 2*k-2 

Holding Register[2*k+1]: Fault Indicator 0 (FI0),
.
.
.
.
Holding Register[3*k]: Fault Indicator k-1 ,

Holding Register[3*k-1]: Section Switch 0 , 
.
.
.
.
Holding Register[5*k-1]: Section Switch 2*k-2 .
 

The server displays and refresh the following values [The following are initial values][for 4 load points] -

CB=0  
FD0=0 FD1=0 FD2=0 FD3=0 FD4=0 FD5=0 FD6=0   
FI0=0 FI1=0 FI2=0 FI3=0   
SS0=0 SS1=0 SS2=0 SS3=0 SS4=0 SS5=0 SS6=0   
LP0=1 LP1=1 LP2=1 LP3=1


Abbreviation of above -
CB- Circuit Breaker 	
FD- Fault Detector 	
FI- Fault Indicator 	
SS- Section Switch 
LP-Load Point [1 when LP is active, 0 when LP is inactive]



An example scenario
-------------------

Suppose a fault is created between Main and LP0, then Circuit breaker opens the circuit, FD0 and FI0 becomes 1. Then, the corresponding section switches are turned on to isolate the section and the circuit breaker is turned off. All LPs will go down after this step.

Then, if a fault is created between LP2 and LP3, the corresponding Fault detector, Fault indicator and the section switches are turned on.

If the line between Main and LP0 is repaired, then the fault detectors FD0, and fault indicators FI0 and SS0 goes down. Then LP0,LP1,LP2 will start working but LP3 will remain OFF due to the fault in LP2-LP3 line.

If command to repair fault in line LP2-LP3 is given, the whole system becomes fully functional again.

Guidelines to use the client program to control the simulation is provided in the program.

Steps to run
-------------

1. Compile the broccoli executable and the simulation of physical system using the Makefile (in the Makefile, change the Broccoli variable to the address of the installed location of the bro)

2. Run the Bro scripts by running the shell scripts -- network.sh and physical.sh -- by running the command "sh <name_of_shell_script>.sh" (in the shell scripts, change the BRO_PATH variable to the location of Bro installation)

3.	Execute the server and clients of the physical systems:

% ./server -n <IP address> -p <port number> -l <Number of load points>
% ./monitor_client -n <IP address> -p <port number> -l <Number of load points> 
% ./command_client -n <IP address> -p <port number> -l <Number of load points>

4. Execute the command to execute the Broccoli file - 

% ./bro
 
The Bro script for the automated distribution system raises an alert by adding an entry in log file for the following cases:

1. CB is turned OFF whenever there is a fault in any of the lines and the faulty line is not isolated.

2. CB is turned ON when there is no fault or if the faulty line is isolated.

3. Fault Indicator is turned ON when the fault detector is turned OFF

4. In between LP1, LP2, and LP3 there are 2 fault detectors between 2 load points, if one of them is ON and another one is OFF, then the system is comprimised.

Commands to run the whole system
--------------------------------

The Dnp3 is implemented using the opendnp3

1. Run the Bro script by running the shell script :
	bro.sh by running the commands sh <name_of_shell_script>.sh [In the shell scripts change the BRO_PATH variable to the location of bro installation] [Change the number of load points which is a global variable, currently 4]. 

2.Compilation - Run Make commands in the folder "master" and "outstation", Then run the shell scripts in the "Auto_Distribution_DNP3" folder.
 
3. execute the master, and outstation in 2 terminals. The communication between the master and outstation can be see
n as they exchange the status of the physical system. Changes can be made to the system by using the outstation process.
The command to execute master and outstation
sh server -l <Number of load points>
sh client -l <Number of load points>

