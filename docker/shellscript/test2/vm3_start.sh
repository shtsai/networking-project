#!/bin/sh
# This script starts vm3
#
# ----------------------------------------------------------------------------
# Below are the explanations for the options:
# 
# -ti indicates that we are running the container in an interactive way
# --cap-add NET_ADMIN allows the this container to modify its ip route table
# --network=vm3 specifies the default network to which this container is attached. This network is used to connect to the Internet.
# --name vm3 specifies the name of this container
# shtsai7/server:vm is the name of docker image we will use, notice the tag is "vm"
# /bin/bash is the program we will run this container, which is shell
#
docker run -ti --cap-add NET_ADMIN --network=vm3 --name vm3 shtsai7/server:vm /bin/bash
