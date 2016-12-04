# Quagga

This document will go over the steps of setting up Quagga in a docker container that plays the role of a router.

## Getting Started

The following instructions will give you the tools and procedures you need know to run Quagga in a container, which allow you to turn a container into a router running OSPF. 

**If you are using the docker image I created for routers (shtsai7/server:router), most of following configurations have already been done. However, I will go over the configuration process step by step for further reference.**

### Prerequisites

**Before you preceed, make sure you have read the [documentation for setting up docker containers and networks](https://github.com/shtsai7/networking-project/tree/master/docker).**

#### Installation

The first thing you need to do is to install quagga in the router containers.

**If you are using the docker image I created for routers (shtsai7/server:router), the quagga software is already installed and you can skip the Installation step.**

The command for installing quagga is pretty straightforward.
```
apt-get update && apt-get install quagga
```

**If you are building docker image using Dockerfile, make sure you add this installation command to your build for the router image.

---------------------------------------------------------------------------------------------------

### Quagga Configuration

#### Activate the Quagga Daemons

You need to first activate the quagga daemons that you want to run on your router. In this experiment, we will need *zebra* and *ospfd*.

To do this, we need to change the daemons configuration files. Use your favorite text editor to open the following file:
```
emacs /etc/quagga/daemons
```

After you open this file, change the value of *zebra* and *ospfd* to "yes".
```
zebra=yes
bgpd=no
ospfd=yes
ospf6d=no
ripd=no
ripngd=no
```
Now, *zebra* and *ospfd* daemons have been activated.

*This step has been done in the router image (shtsai7/server:router).*

----------------------------------------------------------------------------------------

#### Add Configuration Files for Daemons and vtysh (vty shell)

A configuration file (.conf) is required for each daemon you activated. You can copy the sample config files provided by quagga and go from there.

To copy the config file:
```
cp /usr/share/doc/quagga/examples/zebra.conf.sample /etc/quagga/zebra.conf
cp /usr/share/doc/quagga/examples/ospfd.conf.sample /etc/quagga/ospfd.conf
```
*As the their names suggested, these configuration file are just sample file. You will need to change them later when you start quagga.*

You also need the config file for vtysh. Do:
```
cp /usr/share/doc/quagga/examples/vtysh.conf.sample /etc/quagga/vtysh.conf
```

After you have the vtysh.conf file, you need to make one change in it. Disable the setting for "service integrated-vtysh-config" by commenting out this line.
```
!
! Sample configuration file for vtysh.
!
! service integrated-vtysh-config
! hostname quagga-router
username root nopassword
!
```
When you disable this setting, the configuration you saved under vtysh will be stored in separate files depending on the protocols you activated. If you activate this setting, all the configuration you saved under vtysh will be put in one file called Quagga.conf, which might cause conflicts when you change the protocols you want to use. Therefore, it is recommended that you disable this setting and keep a separate config file for each daemon (protocol) you use.

*This step has been done in the router image (shtsai7/server:router).*

----------------------------------------------------------------------------------------

#### Edit debian.conf File

By default, the quagga daemons are only listening to the loopback interface. To make the daemons listen to all the interfaces' IP addresses, remove the -A option in the debian.conf.

Open the debian.conf file:
```
emacs /etc/quagga/debian.conf
```

Remove all the -A options:
```
zebra_options="  --daemon "
bgpd_options="  --daemon "
ospfd_options="  --daemon "
ospf6d_options="  --daemon "
ripd_options="  --daemon "
ripngd_options="  --daemon "
isisd_options="  --daemon "
babeld_options="  --daemon "
```

Note that the "vtysh_enable" option should be "yes", so that we can access the quagga router via vtysh later.#

*This step has been done in the router image (shtsai7/server:router).*

----------------------------------------------------------------------------------------

#### Change File Permission

Give user and group ownership to respectively quagga and quaggavty to the files inside the /etc/quagga directory.
```
chown quagga.quaggavty /etc/quagga/*.conf
chmod 640 /etc/quagga/*.conf
```

*This step has been done in the router image (shtsai7/server:router).*

-----------------------------------------------------------------------------------

#### Run Quagga and vtysh, and Get the Correct .conf Files (configuration NOT finished yet)

Now you can run the quagga daemons. Use the following command:
```
/etc/init.d/quagga start
```
This will get the quagga daemons running. However, we are not finished yet because we haven't configure the OSPF protocol to running on the correct interfaces. To do this, we will need to first get a correct version of the daemon config file, which correctly shows the network interfaces our container has and other information.

First, we run vtysh, which is a shell interface for the router:
```
vtysh
```
You will probably see a weird blank page with "END" showing on the bottom-left corner. Press "q" to exit this blank page and now you are inside the vtysh.

To obtain the new config files, enter the following command in vtysh:
```
write
```
You should see "Configuration saved to /etc/quagga/zebra.conf" and "Configuration saved to /etc/quagga/ospfd.conf". This two files correctly reflect the actual interfaces that you container has. 

Next, we need to let the router know where we want it to run ospf. Open the /etc/quagga/ospfd.conf, find the "router ospf" line, and add the subnet id, netmask, and the area below that line. For example:
```
route ospfd
 network 10.1.0.0/16 area 0.0.0.0
```
The subnet id and netmask tell the router where you want the ospf to run. These two parameters should come from one of subnets that the container is currently attached. The area parameter can be set to 0.0.0.0 for now because we are not using it in this experiment.

----------------------------------------------------------------------------------------------

## Authors

* **Shang-Hung Tsai** - [networking-project](https://github.com/shtsai7/networking-project.git)

## Acknowledgments

* Docker [tutorials](https://docs.docker.com/engine/tutorials/)

