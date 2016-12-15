#!/bin/sh
# This script starts vm2
#
# ----------------------------------------------------------------------------
# Below are the explanations for the options:
# 
# -ti indicates that we are running the container in an interactive way
# --privileged is required to turn off IP spooling protection
# --cap-add NET_ADMIN allows the this container to modify its ip route table
# --network=vm2 specifies the default network to which this container is attached. This network is used to connect to the Internet.
# --name vm2 specifies the name of this container
# shtsai7/server:vm is the name of docker image we will use, notice the tag is "vm"
# /bin/bash is the program we will run this container, which is shell
#
docker run -ti --privileged --cap-add NET_ADMIN --network=vm2 --name vm2 shtsai7/server:vm /bin/bash
