# RIOT VM


## About
This repository includes a [Vagrantfile](https://github.com/RIOT-OS/RIOT/blob/master/Vagrantfile)
to create and control a Linux virtual machine based on an Ubuntu 16.04 (64-bit) image that contains
all necessary toolchains and dependencies to build and flash compatible devices with RIOT.
The advantage of using this VM is to have a reproducible, portable and even disposable environment
that can be used to develop for RIOT with decreased setup times and without the requirement of
making changes to the underlying host system.

## Requirements
Make sure your system satisfies the latest version of all following dependencies:
* [VirtualBox](https://www.virtualbox.org/wiki/Downloads)
* [VirtualBox Extension Pack](https://www.virtualbox.org/wiki/Downloads)
* [Vagrant](https://www.vagrantup.com/downloads.html)

## Usage
The following commands must be run from the RIOT root directory on the host system.

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
Once logged in to the VM via `vagrant ssh` you can find the RIOT root directory in your
working directory. This is a shared directory and stays synchronized with your RIOT directory
on the host system. All changes made will be mirrored from the host system to the guest system
and vice versa.

This feature allows you to conveniently develop code for RIOT with your preferred IDE on
your host system and use the VM for compiling, flashing devices and running the native port of RIOT.

## Additional Information
1. VirtualBox: For new boards it is necessary to add missing usb filters to the Vagrantfile so that VirtualBox is able to capture the devices.
  * For Linux Guest Systems: For new boards it is necessary to add new udev rules to `dist/tools/vagrant/udev_rules`.
    The needed `vendor id` and `product id` can be obtained by running `vboxmanage list usbhost`.
  * For Linux Host Systems: Additionally, in order to allow USB access from within the guest system, the host system user
    must be a member of the `vboxusers` group (see [here](https://www.virtualbox.org/manual/ch02.html#idm1051)).
2. You can forward ports on the virtual machine with the following command:

   ```
   config.vm.network "forwarded_port", guest: <port_1>, host: <port_2>
   ```
