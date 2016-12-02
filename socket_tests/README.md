SOCKET_TESTS

Description
	This directory contains a number of client and server programs.
	    -Client programs create packets and deliver them to the destination hosts.
       	    -Server programs receive packets sent by other hosts.
	The filenames indicate the types of protocol used by each program, for example, UDP or TCP.
	Programs that includes "RAW" in their names use raw sockets and create packets from scratch.
	Running "RAW" program requires super user privilege.


Getting Started
    Compilation
	To compile a program, simply type "make" followed by the program name. An executable will be generated.
	   >> make clientUDP   (generates executable "clientUDP")

	To clean all executable, type "make clean"
	   >> make clean


Programs
	clientUDP.c
		Create a UDP packet and deliver it to the destination host and port. User will be prompted to enter a message.
		It takes two arguments.
		   - destination IP address
		   - port number
		Example:
			>> ./clientUDP 10.10.10.4 51717


	clientTCP.c
		Create a TCP packet and deliver it to the destination host and port. User will be prompted to enter a message.
		It takes two arguments.
		   - destination IP address
		   - port number
		Example:
			>> ./clientTCP 10.10.10.4 51717


	clientIPinIP.c
		Create an IP-in-IP encapsulated packet and deliver it to the destination host and port.
		The data contained in this packet is hardcoded.

		This program takes four arguments.
		   - source IP address (inner header source address)
		   - destination IP address (inner header destination address, and outer header source address)
		   - forward IP address (outer header source address)
		   - port number

		This packet is a packet encapsulated by the destination server and sent to the forward server.

             **************************************************************************************
             *                                                                                    *
	     *		     original packet                     encapsulate                      *
             *	source server ------------>  destination server ------------> forward server      *
             *  (10.10.10.2)                   (10.10.10.3)                   (10.10.10.4)        *
	     **************************************************************************************
		Example:
			>> ./clientIPinIP 10.10.10.2 10.10.10.3 10.10.10.4 51717


	clientRAW.c
		Create a UDP packet from scratch and deliver it to the destination host and port using a raw socket.
	        This program takes two arguments.
		     - destination IP address
		     - port number
		Data and source IP address are hardcoded.
		
		Example:
			>> ./clientRAW 10.10.10.4 51717

		** Note: This program is still in test. Avoid it if possible.


	clientTCPRAW.c
		Create a TCP SYN packet from scratch and deliver it to the destination host and port using a raw socket.
	        This program takes two arguments.
		     - destination IP address
		     - port number
		Source info and data are hardcoded.
		
		Example:
			>> ./clientTCPRAW 10.10.10.4 51717

		** Note: This program is not completed yet.
		   	 It only sends a TCP connection initiate message, does not complete the handshakes.


	serverUDP.c
	serverTCP.c
	serverRAW.c
	serverIPinIP.c
		These server programs are used for testing the client programs. 
