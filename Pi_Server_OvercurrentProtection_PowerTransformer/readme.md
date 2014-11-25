LBNL Physics-Based Intrusion Detection using OSIsoft PI Data Historian
======================================================================

Transformer Over Current Protection Scenario using PI data historian
--------------------------------------------------------------------

Detailed explanation of this system model and the NIDS is in the paper - http://www.escholarship.org/uc/item/4m08h4xn

This specific example leverage OSIsoft data historian to monitor the activities of the physical system simulated using Matlab Simulink (Details of the simulink can be obtained in the readme of folder OvercurrentProtection_PowerTransformer). 
The data historian is used to tap the network traffic in the control network and also to query master devices to obtain the state of the system. In absence of a data injection attack, both state obtained from both the sources needs to be same. The advantage of leveraging the data historian is availability of the entire history of data and it gives ability to keep track of previous states which makes monitoring more efficient.

The detailed explanation of execution of the Data historian is explained in the file "Documentation OSI PI Server Reading and Writing script.docx". Details of installation of the Matlab Simulink program is explained in readme of OvercurrentProtection_PowerTransformer folder.

