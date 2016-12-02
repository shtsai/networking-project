# Docker

This document will go over the steps of setting up a docker network.

## Subdirectory

There are two subdirectories:   
* build: contains the Dockerfile for creating docker image of the vm servers we will use
* shellscript: contains shellscripts for networks and containers
      
Check the README files in these two subdirectories for further details.

## Getting Started

These instructions will give you the tools you need to set up a docker network in your machine for the experienment. 

### Prerequisites

The first thing you need to do is to install docker on your machine if you haven't done so.

Docker offers many different versions of their software for different platforms. However, the operating system that I used to set up the docker environment is Ubuntu 16.04. I would recommand using the same environment as I do if you can for the best result. 

There is a detailed instruction for the installation on docker's website. Please check the link below for more information.
     
      https://docs.docker.com/engine/installation/linux/ubuntulinux/ 

Next, you might want to follow some tutorials to get yourself familiar with some basic docker commands and operations. Below is the link to the official docker tutorial.

      https://docs.docker.com/engine/getstarted/

After the above steps, you are ready to roll.

### Obtaining the docker images for server

There are two ways to get the docker images you need to use.

1. (Recommended) Clone the docker images that I have already created from Docker Hub

      There are two docker images I created. One is for "vm" server, and the other is for "router". Each of them has their required software pre-installed. For example, both images have network tools like ifconfig, ping, and traceroute installed. In addition, the "router" image has quagga installed.

A step by step series of examples that tell you have to get a development env running

Say what the step will be

```
Give the example
```

And repeat

```
until finished
```

End with an example of getting some data out of the system or using it for a little demo

## Running the tests

Explain how to run the automated tests for this system

### Break down into end to end tests

Explain what these tests test and why

```
Give an example
```

### And coding style tests

Explain what these tests test and why

```
Give an example
```

## Deployment

Add additional notes about how to deploy this on a live system

## Built With

* [Dropwizard](http://www.dropwizard.io/1.0.2/docs/) - The web framework used
* [Maven](https://maven.apache.org/) - Dependency Management
* [ROME](https://rometools.github.io/rome/) - Used to generate RSS Feeds

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

## Authors

* **Billie Thompson** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Hat tip to anyone who's code was used
* Inspiration
* etc

