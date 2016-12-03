# Docker

This document will go over the steps of setting up a docker network.

### Subdirectory

There are two subdirectories:   
* [build](https://github.com/shtsai7/Networking-project/tree/master/docker/build): contains the Dockerfile for creating docker images of the vm servers we will use
* [shellscript](https://github.com/shtsai7/Networking-project/tree/master/docker/shellscript): contains shellscripts for configuring docker networks and containers
      
Checkout the README files in these two subdirectories for further details.

## Getting Started

The following instructions will give you the tools and procedures you need know to set up a docker network in your machine for the experienment. 

### Prerequisites

#### Installation

The first thing you need to do is to install docker on your machine if you haven't done so.

Docker offers many different versions of their software for different platforms. However, the operating system that I used to set up the docker environment is Ubuntu 16.04. I would recommand using the same environment as I do if you can for the best result. 

There is a detailed instruction for the installation on docker's website. Please check the link below for more information.
     
      https://docs.docker.com/engine/installation/linux/ubuntulinux/ 
     
After you installed docker, don't forget to check out how to manage docker as a non-root user. After you configure this, you will not need to use "sudo" every time you want to execute a docker command. The link is shown below.

      https://docs.docker.com/engine/installation/linux/ubuntulinux/#manage-docker-as-a-non-root-user
     
#### Docker Tutorials

Next, you might want to follow some tutorials to get yourself familiar with some basic docker commands and operations. Below is the link to the official docker tutorial.

      https://docs.docker.com/engine/getstarted/

After the above steps, you are ready to roll.

---------------------------------------------------------------------------------------------------

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
      
      In the subdirectory "[build/](https://github.com/shtsai7/Networking-project/tree/master/docker/build)", I included the Dockerfile I used to build the server image. You can start from there. There is also a README file in that directory, which briefly explains how the Dockerfile works. Check that README file for more information about building docker images.

---------------------------------------------------------------------------------------------------

### Creating Docker networks

The next step is to create docker networks so that our containers can connect to them. 

**It is recommended that you have all your network design ready on your hand, and then create the docker networks you need. It would be very difficult to change things around after you connect everything together. In worst case, you might have to give up all the networks you created and start all over again.**

#### Create a network

When you create a network, you need to specify the network driver you want to use, network id, netmask, and the name of the network you are creating.

* For the network driver, I found that the default driver "bridge" works fine in this case, so I will stick with the default driver. 
* You can specify the network id and netmask you want to use. This is optional, but it is helpful to specify it when you are designing a network.
* Network name is the name of the network. You will be using it when you connect your containers to the network, so pick a good name for it.

Example:
```
docker network create -d bridge --subnet=192.168.0.0/16 network1
```
> The above command creates a docker network using the *bridge* driver, with a network id of 192.168.0.0 and a netmask of             255.255.0.0, and the name of this network is called *network1*.

For more options regarding docker network creation, please use the help command:
```
docker network create --help
```

#### To check the list of all docker networks on your machine

Use the following command:
```
docker network ls
```
You should see the default networks and the networks you created.

#### To inspect a network

Use the *inspect* command following by the name of the network you want to inspect:
```
docker network inspect network1
```
This command should give you the configuration details of this network, including its subnet, driver, a list of containers that are connected to this network. 

#### To remove a network

Use the *rm* command following by the name of the network you want to remove:
```
docker network rm network1
```
If you list all the networks again, this network should be removed.

#### To connect a container to a network

Although I haven't talked about how to run a container yet, I will quickly go over the command you will need to use to connect a container to a network. This is actually pretty straightforward. 
The command you will use is *connect*. When you make this connection, you can also specify an IP address you want your container to use in that subnet. If you don't specify it, an IP address in that subnet will be automatically assigned one for you. After that, just enter the names of the network and container you want to connect.
```
docker network connect --ip 192.168.0.101 network1 container1
```

To check the result, use *inspect* command to inspect the network, and see if the container is connected to it.

#### To connect a container to a network

Simply use the *disconnect* command, followed by the names of the network and container.
```
docker network disconnect network1 container1
```

**Note that when we start a docker container, we usually specify a network in the option. This network is used by the container to connect to the Internet. Thus, no matter what other network configurations we made, the container can still maintain Internet connection. Every container should have their own private connection to the Internet through this interface. These networks should be created before we start the containers, and they must be configured in different subnets.**

---------------------------------------------------------------------------------------------------

### Running docker containers

#### Run a docker container

There are several ways that you can run a docker container, but I am going to focus on the interactive way because we need to frequently issue commands and change configurations. Below is a typical command I used to run "vm" containers:
```
docker run -ti --cap-add NET_ADMIN --network=vm1 --name vm1 shtsai7/server:vm /bin/bash
``` 
* -ti indicates that we are running the container in an interactive way
* --cap-add NET_ADMIN allows the this container to modify its ip route table
* --network=vm1 specifies the default network to which this container is attached. This network is used to connect to the Internet.
* --name vm1 specifies the name of this container
* shtsai7/server:vm is the name of docker image we will use, notice the tag is "vm"
* /bin/bash is the program we will run this container, which is shell

After entering this command, you are now inside a docker container. 

**Note: for containers that will be running quagga (i.e. routers), you need to give them additional privilege. Otherwise, you will get an error when you try to run quagga. To do this, add an additional option in the command. Example:
```
docker run -ti --privileged --cap-add NET_ADMIN --network=r1 --name r1 shtsai7/server:router /bin/bash
```

#### Temporarily disattach from the container

If you want disattach from the container, you can do:
```
CTRL-P CTRL-Q
```
This command will bring you back you the shell of your host OS. Notice that the container is still running in the background.

#### List all the containers in your machine

Simply use the following command:
```
docker ps -a
```
This command will list all the containers (no matter what state they are in) along with some information about each of them.

#### Attach to a container

First you need to make sure the container to which you want to attach is actually running. This can be done using the above command.
After that, you can attach to a container using *docker attach* followed by the name of the container:
```
docker attach container1
```

---------------------------------------------------------------------------------------------------

### Connecting docker containers into a large network

Once you have your docker networks created and containers running, you can go ahead and connect them together. This step should be pretty straightforward, as we just need to use *docker connect* command to connect networks and containers. 
```
docker network connect --ip 192.168.0.101 network1 container1
docker network connect --ip 192.168.0.102 network1 container2
```
Two containers that connect to the same network are in the same subnet and thus directly connected. There is a logical link between such two containers. On the other hand, containers that are not in the same subnet are not directly connected. As a result, routers are needed to direct traffics between these containers.

---------------------------------------------------------------------------------------------------

### Using shellscript to facilitate network creation

Repeating the the above commands every time you create a network can be tedious. Fortunately, we can use shellscript file to make this job easier. All you need to do is to change the configuration in these files, and then execute them in the correct order.

Please refer to the README file in [shellscript/](https://github.com/shtsai7/Networking-project/tree/master/docker/shellscript) directory for more information.

## Quagga

Once you have everything mentioned above correctly set up, you are ready to run Quagga on your routers.

Please refer to the README file in [quagga/](https://github.com/shtsai7/networking-project/tree/master/quagga) for further information about running Quagga.

## Authors

* **Shang-Hung Tsai** - [networking-project](https://github.com/shtsai7/networking-project.git)

## Acknowledgments

* Docker [tutorials](https://docs.docker.com/engine/tutorials/)

