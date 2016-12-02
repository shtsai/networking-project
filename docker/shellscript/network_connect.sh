#!/bin/sh
# This script connects the docker containers to their corresponding subnets
# Assume the following network names have been created in docker environment
# and the following containers are running
docker network connect --ip 10.1.1.2 vm1r1 vm1
docker network connect --ip 10.1.1.3 vm1r1 r1
docker network connect --ip 10.1.2.2 r1r2 r1
docker network connect --ip 10.1.2.3 r1r2 r2
docker network connect --ip 10.1.3.2 r2vm2 r2
docker network connect --ip 10.1.3.3 r2vm2 vm2
