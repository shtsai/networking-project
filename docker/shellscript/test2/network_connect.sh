#!/bin/sh
# This script connects the docker containers to their corresponding subnets
# Assume the following network names have been created in docker environment
# and the following containers are running
docker network connect --ip 10.1.1.2 vm1r1 vm1
docker network connect --ip 10.1.1.3 vm1r1 r1
docker network connect --ip 10.1.2.3 vm2r2 vm2
docker network connect --ip 10.1.2.2 vm2r2 r2
docker network connect --ip 10.1.3.2 vm3r3 vm3
docker network connect --ip 10.1.3.3 vm3r3 r3
docker network connect --ip 10.1.4.2 r1r3 r1
docker network connect --ip 10.1.4.3 r1r3 r3
docker network connect --ip 10.1.5.3 r2r3 r2
docker network connect --ip 10.1.5.2 r2r3 r3
docker network connect --ip 10.1.6.2 r1r4 r1
docker network connect --ip 10.1.6.3 r1r4 r4
docker network connect --ip 10.1.7.2 r2r4 r2
docker network connect --ip 10.1.7.3 r2r4 r4
