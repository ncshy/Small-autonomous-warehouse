# Small_autonomous_warehouse

There are several projects inside the robot code folder. m3pi_519_project is the complete set with the entire robot state machine.
The other two are used to test individual components of the robot code and hardware.
linefollow_test contains the code which was used in the demo video on Youtube.

server_program directory contains the initial TCP/IP server that was implemented. Too bad it was naught of use. In a quite interesting twist, the working Wifi system was rendered ineffective because of power constraints of the m3pi robots. A true Embedded System limitation. 

Following this the code was repackaged to use the Zigbee communication instead of WiFi. It was more involved and convoluted because it needed to use user space drivers as well as adhere to certain handshake mechanisms.  

controller_program directory contains the zigbee communication server and its related processes, the Vicon client and the controller logic in its infancy.
