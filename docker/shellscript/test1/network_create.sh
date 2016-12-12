#!/bin/sh
# This script creates docker subnets
# Assume the following network names do not exist in docker environment
docker network create -d bridge --subnet=10.1.1.0/24 vm1r1
docker network create -d bridge --subnet=10.1.2.0/24 r1r2
docker network create -d bridge --subnet=10.1.3.0/24 r2vm2
