# SOCKET_SERVER

## Description
	
This program creates a ethernet socket and reads all the incoming IP packets. It will then handle these packets depending on their protocol types.

Note: this program currently handles packets by printing their header info and data.

## Files

* socket_server.c

	--contains the main program

* print_packet.c
	
	--contains some functions that prints the packet's headers and data

* print_packet.h
	
	--it is a header file of print_packet.c


## Getting Started
    
### Compilation
	
To compile the program, simply type "make" in this directory, and an executable "socket_server" will be generated:

```
>> make
```

To clean the executable and .o files in the directory, use "make clean":

```
>> make clean
```

### To run the program

Run it in super user mode. This program takes no argument.

```
>> sudo ./socket_server
```

### To test the program

Use programs in /socket_tests to generate traffic. 

* clientUDP: generates a UDP packet and send it to the destination
* clientIPinIP: generates an IP-in-IP encapsulated packet and send it to the destination
* clientTCP: generates a TCP packet and send it to the destination (currently don't work, b/c it requires TCP handshake)

Note: currently cannot use iperf to test the program, b/c iperf needs to establish a TCP connection with the destination
		(even for UDP packets).

Please refer to the README file in socket_tests/ for more information about testing.

## Authors

* **Shang-Hung Tsai** - [networking-project](https://github.com/shtsai7/networking-project.git)
