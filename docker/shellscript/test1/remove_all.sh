#!/bin/sh
# This script first stops the following containers
docker stop vm1
docker stop vm2
docker stop r1
docker stop r2
# Then it removes them
docker rm vm1
docker rm vm2
docker rm r1
docker rm r2
