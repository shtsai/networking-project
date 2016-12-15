#-----------------------------------------------------------------------------------#
# This Dockerfile is used to build the docker image of vm servers for the project   #
# It is based on ubuntu:16.04                                                       #
# Below is a list of programs that are installed in this build                      #
#-----------------------------------------------------------------------------------#
FROM ubuntu:16.04
RUN apt-get -y update && apt-get install -y \
  git \
  net-tools \
  inetutils-ping \
  emacs \
  vim \
  make \
  gcc \
  traceroute \
  scapy
