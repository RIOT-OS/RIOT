# RIOT VM


## About
This repository includes a [Vagrantfile](https://github.com/RIOT-OS/RIOT/blob/master/Vagrantfile)
to download and control a pre-configured Linux virtual machine (VM) based on an Ubuntu 16.04 (64-bit) image that contains all necessary toolchains and dependencies to build and flash compatible devices with RIOT.
The advantage of using this VM is to have a reproducible, portable and even disposable environment
that can be used to develop for RIOT with decreased setup times and without the requirement of
making changes to the underlying host system.

## Requirements
Make sure your system satisfies the latest version of all following dependencies:
* [VirtualBox](https://www.virtualbox.org/wiki/Downloads)
* [VirtualBox Extension Pack](https://www.virtualbox.org/wiki/Downloads)
* [Vagrant](https://www.vagrantup.com/downloads.html)

## Selecting a non-default VM (optional)
In case a non-default VM should be used, one can easily adapt the source for the vagrant box. This is done by the following command.

```
vagrant box add RIOT-VM <path to box>
```

## General usage
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
This will reset your VM to the default state.

## Inside the VM
Once logged in to the VM via `vagrant ssh` you can find the RIOT root directory in your
working directory. This is a shared directory and stays synchronized with your RIOT directory
on the host system. All changes made will be mirrored from the host system to the guest system
and vice versa.

This feature allows you to conveniently develop code for RIOT with your preferred IDE on
your host system and use the VM for compiling, flashing devices and running the native port of RIOT.

## Additional Information
1. VirtualBox: For new boards it is necessary to add a usb filter (open VirtualBox, click on USB and add the new filter for your board).
  * For Linux Guest Systems: For new boards it is necessary to add new udev rules in the Vagrant config (in `dist/tools/vagrant/udev_rules`) so that Vagrant can capture the device.
    The needed `vendor id` and `product id` can be obtained by running `vboxmanage list usbhost`.
  * For Linux Host Systems: Additionally, in order to allow USB access from within the guest system, the host system user
    must be a member of the `vboxusers` group (see [here](https://www.virtualbox.org/manual/ch02.html#idm1051)).
2. You can forward ports on the virtual machine with the following command:

   ```
   config.vm.network "forwarded_port", guest: <port_1>, host: <port_2>
   ```

## Quirks
* Windows
  * For some Git Bash and Vagrant versions the prompt is not displayed on `vagrant ssh`.
    In this case, the Windows command line should be used to invoke `vagrant ssh`.
