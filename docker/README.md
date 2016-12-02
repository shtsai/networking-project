# Docker

This document will go over the steps of setting up a docker network.

### Subdirectory

There are two subdirectories:   
* build: contains the Dockerfile for creating docker images of the vm servers we will use
* shellscript: contains shellscripts for configuring docker networks and containers
      
Checkout the README files in these two subdirectories for further details.

## Getting Started

The following instructions will give you the tools and procedures you need know to set up a docker network in your machine for the experienment. 

### Prerequisites

#### Installation

The first thing you need to do is to install docker on your machine if you haven't done so.

Docker offers many different versions of their software for different platforms. However, the operating system that I used to set up the docker environment is Ubuntu 16.04. I would recommand using the same environment as I do if you can for the best result. 

There is a detailed instruction for the installation on docker's website. Please check the link below for more information.
     
      https://docs.docker.com/engine/installation/linux/ubuntulinux/ 
      
#### Docker Tutorials

Next, you might want to follow some tutorials to get yourself familiar with some basic docker commands and operations. Below is the link to the official docker tutorial.

      https://docs.docker.com/engine/getstarted/

After the above steps, you are ready to roll.

### Obtaining the docker images for server

There are two ways to get the docker images you need to use.

1. (Recommended) Pull the docker images that I have already created from Docker Hub

      There are two docker images that I created. One is for "vm" server, and the other is for "router". Both of them are based on ubuntu:16.04. They are already correctly configured and have their required software pre-installed. For example, both images have network tools like ifconfig, ping, and traceroute installed. In addition, the "router" image has quagga installed.
      
      The command for pulling images from my docker repository:

      ```
      docker pull shtsai7/server:vm        // image for "vm" 
      docker pull shtsai7/server:router    // image for "router"
      ```
      
      After executing the above commands, you can check the result by doing the following command, which displays all local docker image files on your machine:
      
      ```
      docker images
      ```
      
      If everything works out correctly, you should see those two images listed there.
      
      Once you obtain the docker images, feel free to make any change you need to them and commit to your own repository.
      
2. Build the docker images from scratch using Dockerfile

      Building the docker images using Dockerfile gives you more options and flexibilities to choose and configure what you want in your docker image. Note that most docker images of Linux distros (such as ubuntu) on Docker Hub are in their most basic forms, meaning they come with almost no software pre-installed (even for the most basic programs like ifconfig, ping, etc). Therefore, if you decide to take this approach, you will need to install all the software you need in the build process. This is important because if you install some software after you start a container using an image, the changes you made in that container will not affect the image you used. Therefore, every time you start a new container, you will have to repeat the tedious installation process, which can be very annoying.
      
      In the subdirectory "build/", I included the Dockerfile I used to build the server image. You can start from there. There is also a README file in that directory, which briefly explains how the Dockerfile works. Check that README file for more information about building docker images.

## Authors

* **Shang-Hung Tsai** - [networking-project](https://github.com/shtsai7/networking-project.git)

## Acknowledgments

* Hat tip to anyone who's code was used
* Inspiration
* etc

