LBNL Physics-Based Intrusion Detection Bro Modules
================================================== 

This directory includes C-based simulations and/or network traffic traces of several physical systems, and a set of Bro and Broccoli signatures that analyze communication with the physical system and compare the effects of that communication with a physical simulation of the device.

The monitoring system is implemented using the [Bro Network Security Monitor](https://www.bro.org/).  There are two primary Bro modules for each scenario.

1. Network Module - Monitors the network activities which do not require any knoweledge of physical system

2. Physical Module - Has specific knowledge of the physics and physical limitations of the system and protects the system based on that.

We note that the modular design is intentional for two reasons:  

First, while the examples are potentially useful on their own, they are primarily intended to be illustrative of certain types of physical systems and how one might develop Bro scripts to monitor such systems.  The design can be used as a template for developing scripts to monitor other physical systems.  Is our hope that a large enough library of such templates could be useful in rapid development of physics-based Bro scripts.  In the interim, they are intended to be representative examples to develop from.  

Second, while we personally believe that Bro represents a superb environment for monitoring network activity, and while Bro also represents a reasonable environment for monitoring the physics of devices, there are some environments which could be even better suited depending on the characteristics of the device physics being monitored, for example in a situation where mathematical functions are more readily available in a different environment such as Matlab or Simulink.

Our code implemented using both Bro and also Broccoli, the Bro Client Communication Library.  The Broccoli program acts as "middleware" communication between the two Bro scripts to bridge the network layer and "physical elements" (not to be confused with the physical layer of the OSI stack).