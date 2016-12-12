#!/bin/sh
# This script starts r4
#
# ----------------------------------------------------------------------------
# Below are the explanations for the options:
# 
# -ti indicates that we are running the container in an interactive way
# --privileged is required for running quagga on the router
# --cap-add NET_ADMIN allows the this container to modify its ip route table
# --network=r4 specifies the default network to which this container is attached. This network is used to connect to the Internet.
# --name r4 specifies the name of this container
# shtsai7/server:router is the name of docker image we will use, notice the tag is "router"
# /bin/bash is the program we will run this container, which is shell
#
docker run -ti --privileged --cap-add NET_ADMIN --network=r4 --name r4 shtsai7/server:router /bin/bash
