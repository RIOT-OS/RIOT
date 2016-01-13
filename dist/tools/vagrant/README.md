# RIOT VM


## About
This repository includes a [Vagrantfile](https://github.com/RIOT-OS/RIOT/blob/master/Vagrantfile)
to create and control a Linux virtual machine based on an Ubuntu Trusty (64-bit) image that contains
all necessary toolchains and dependencies to build and flash compatible devices with RIOT-OS.
The advantage of using this VM is to have a reproducable, portable and even disposable environment
that can be used to develop for RIOT-OS with decreased setup times and without the requirement of
making changes to the underlying host system.

## Requirements
Make sure your system satisfies the latest version of all following dependencies:
* [VirtualBox](https://www.virtualbox.org/wiki/Downloads)
* [VirtualBox Exntension Pack](https://www.virtualbox.org/wiki/Downloads)
* [Vagrant](https://www.vagrantup.com/downloads.html)

## Usage
The following commands must be run from the RIOT-OS root directory on the host system.

```
vagrant up
```
This will start up the virtual machine and download the Ubuntu image as well as all necessary toolchains.
```
vagrant ssh
```
This will logs you in to the VM as the vagrant user.
```
vagrant halt
```
This will shut down the VM gracefully.
```
vagrant destroy
```
This will reset your VM to the default state. All modifications made to the VM by the
[provisioning script](https://github.com/RIOT-OS/RIOT/tree/master/dist/tools/vagrant/bootstrap.sh)
and by you will be removed.
```
vagrant provision
```
This will re-run the [provisioning script](https://github.com/RIOT-OS/RIOT/tree/master/dist/tools/vagrant/bootstrap.sh).

## Inside the VM
Once logged in to the VM via `vagrant ssh` you can find the RIOT-OS root directory in your
working directory. This is a shared directory and stays synchronized with your RIOT-OS directory
on the host system. All changes made will be mirrored from the host system to the guest system
and vice versa.

This feature allows you to conveniently develop code for RIOT-OS with your preferred IDE on
your host system and use the VM for compiling, flashing devices and running the native port of RIOT-OS.

## Additional Information
For new boards it is necessary to add new udev rules to `dist/tools/vagrant/udev_rules` and
additional usb filters to the Vagrantfile so that VirtualBox is able to capture the devices.
The needed `vendor id` and `product id` can be obtained by running `vboxmanage list usbhost`.
