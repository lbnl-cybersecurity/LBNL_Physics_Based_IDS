LBNL Physics-Based Intrusion Detection Bro Modules
==================================================

Transformer Over Current Protection Scenario
--------------------------------------------

Detailed explanation of this system model and the NIDS is in the paper - http://www.escholarship.org/uc/item/4m08h4xn

The specific example consists of the following files:

1. Simulink Model of a power transformer 
2. S-function code that simulates a slave controller
3. C code that simulates a master controller

The specific implementation was tested as follows:
- Matlab/Simulink code: Ubuntu 12.04, Matlab/Simulink R2013a with mex compiler installed, libmodbus library installed
- Master simulator: Tested on both Ubuntu and MacOS, libmodbus library needed
Description:
------------

The Simulink model continuously generates values of the current that flows on the transmission line.

The S-function gets as inputs N samples within a cycle of the current (this is sinusoidal signal) and also the timestamp of each sample. Then we estimate the current based on the N samples.

The master controller implements an overcurrent protection scheme. We choose a pickup current, which is the current threshold. Whenever the estimated current exceeds the predefined pickup current, the master controller should issue a write command to activate the circuit breakers in the simulink model. Then, the circuit breakers remain activated for 5 cycles, which are followed by a restoration cycle (i.e. the value of current is too small).

Execution of the codes:
-----------------------

1. Compile the S-function code: mex -lm -lpthread -lmodbus filename.c modbusRegisters.c
2. Run the Simulink Model
3. Compile the C code: cc filename.c -lmodbus
4. Run the C code: ./a.out
5. Execute the shell script network_monitoring.sh in the folder HC-NIDS to run the network monitoring scripts

***The S-function c code should be compiled every time that we want to start a new simulation.

The folder also contains the following network traffic traces:

1. NormalTraffic.pcap-there are not attacks and there is only one fault at the start of the simulation. When we test our NIDS, no alerts should be generated.
2. AttackScenario1.pcap-the attacker issues single read command requests (total # of attacks in the captured traffic: 6), thus our NIDS should raise 6 alerts ( *2 one for the read request and one for the read response) in total by the packet sequence NIDS rule.
3. attackScenario2.pcap-the attacker issues a block of packets (RQ-RR-WQ-WR) but every time tries to activate the CBs (status of CBs=0). Total # of attacks in the captured traffic: 6, the NIDS should raise 6 alerts in total based on the physical constants NIDS rule.
4. attackScenario3. pcap- the attacker sends block of read command packets => slave unavailable to master controller
5. attackScenario4.pcap-the attacher issues blocks of packets (RQ-RR-WQ-WR) and sets the status of the CBs correctly but the attacker issues the commands with a smaller frequently. Our NIDS should detect the attacks based on the time gap between the newly issued blocks. 

More configuration details:
---------------------------

In our configuration we used an ethernet to usb adaptor to connect the Unix machine with the one that runs the Master Simulator.
Then we created the following network configuration at the Unix machine’s side:
1. We choose the wired connection that corresponds to the adaptor
2. Under the IPv4 settings, we choose a link-local only method
3. The two devices should be in a local network ex.
Unix machine: 169.254.128.196
Master simulator’s machine: 169.254.219.136

In the S-function code we should give to the parameter DEFAULT_CLIENT_IP (line 32) the IP address of the master simulator
In the master simulator’s code we should give to the parameter DEFAULT_CLIENT_IP the IP address of the slave simulator
