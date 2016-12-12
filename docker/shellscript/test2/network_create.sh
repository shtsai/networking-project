#!/bin/sh
# Test 2
# This script creates docker subnets
# Assume the following network names do not exist in docker environment
#
# To the Internet
docker network create -d bridge --subnet=172.18.0.0/16 vm1
docker network create -d bridge --subnet=172.19.0.0/16 vm2
docker network create -d bridge --subnet=172.22.0.0/16 vm3
docker network create -d bridge --subnet=172.20.0.0/16 r1
docker network create -d bridge --subnet=172.21.0.0/16 r2
docker network create -d bridge --subnet=172.23.0.0/16 r3
docker network create -d bridge --subnet=172.24.0.0/16 r4
#
# network that connecting containers
docker network create -d bridge --subnet=10.1.1.0/24 vm1r1
docker network create -d bridge --subnet=10.1.2.0/24 vm2r2
docker network create -d bridge --subnet=10.1.3.0/24 vm3r3
docker network create -d bridge --subnet=10.1.4.0/24 r1r3
docker network create -d bridge --subnet=10.1.5.0/24 r2r3
docker network create -d bridge --subnet=10.1.6.0/24 r1r4
docker network create -d bridge --subnet=10.1.7.0/24 r2r4
