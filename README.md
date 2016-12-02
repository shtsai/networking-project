# Networking Project

This repository is used to build a docker network for the experiment.

## Subdirectory

There are five subdirectories:   
* docker: contains all the code and instruction for configuring docker containers and networks
* socker_server: contains the server program that will be run in the docker networks
* socker_tests: consists of programs that generate UDP, TCP, and IP-in-IP traffics for testing
* simplesniffers: contains simple packet sniffer program implemented using pcap library
* tcpfilereader: contains a program that can read .pcap file and output packet info
      
Checkout the README files in each subdirectorie for further details.

## Authors

* **Shang-Hung Tsai** - [networking-project](https://github.com/shtsai7/networking-project.git)
