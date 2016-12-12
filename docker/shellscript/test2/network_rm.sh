#!/bin/sh
# This script removes all user-created docker subnets
# Assume the following network names already exist in docker environment
docker network rm vm1r1
docker network rm vm2r2
docker network rm vm3r3
docker network rm r1r3
docker network rm r2r3
docker network rm r1r4
docker network rm r2r4
