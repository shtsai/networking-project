# Networking Project

This repository is used to build a docker network for the experiment.

## Subdirectory

There are six subdirectories:   
* [docker](https://github.com/shtsai7/Networking-project/tree/master/docker): contains all the code and instructions for configuring docker containers and networks
* [quagga](https://github.com/shtsai7/networking-project/tree/master/quagga): contains the instructions for configuring quagga in a router container.
* [socker_server](https://github.com/shtsai7/Networking-project/tree/master/socket_server): contains the server program that will be run in the docker networks
* [socker_tests](https://github.com/shtsai7/Networking-project/tree/master/socket_tests): consists of programs that generate UDP, TCP, and IP-in-IP traffics for testing
* [simple_sniffers](https://github.com/shtsai7/networking-project/tree/master/simple_sniffers): contains simple packet sniffer program implemented using pcap library
* [tcp_file_reader](https://github.com/shtsai7/networking-project/tree/master/tcp_file_reader): contains a program that can read .pcap file and output packet info
      
Checkout the README files in each subdirectorie for further details.

## Authors

* **Shang-Hung Tsai** - [networking-project](https://github.com/shtsai7/networking-project.git)
