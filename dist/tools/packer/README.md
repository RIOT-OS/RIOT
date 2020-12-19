# Packer

## About
[Packer](https://www.packer.io/) is a command-line tool to create virtual machines
based on a source configuration file.

## Requirements
To use packer you need to download and install it (compare [Install Packer](https://www.packer.io/docs/installation.html)).

## Usage
A simple build of the vagrant box can be done by:
```
packer build riot.json
```
This command must be executed in the packer directory [RIOT/dist/tools/packer](https://github.com/RIOT-OS/RIOT/tree/master/dist/tools/packer).
Note that the build process might take several minutes.
