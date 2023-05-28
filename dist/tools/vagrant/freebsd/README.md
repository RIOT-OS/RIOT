# RIOT FreeBSD VM

## About
This provides a [Vagrantfile] to compile and test the RIOT [`native`][native]
platform on FreeBSD stable (12.1).

## Requirements
Make sure your system satisfies the latest version of all following dependencies:
* [VirtualBox](https://www.virtualbox.org/wiki/Downloads)
* [VirtualBox Extension Pack](https://www.virtualbox.org/wiki/Downloads)
* [Vagrant](https://www.vagrantup.com/downloads.html)

## General usage
The following commands must be run from this directory on the host system

```
vagrant plugin install vagrant-disksize # for big enough disk
vagrant up
vagrant provision   # depending on the vagrant version this might alreardy be
                    # executed with vagrant up
```
This will start up and set-up the virtual machine.
```
vagrant ssh
```
This logs you into the VM as vagrant user.

See the general vagrant [README.md](../README.md) for more commands.

## Inside the VM
Once logged in to the VM you can run compile and run tests e.g.

```sh
make -C tests/sys/shell/ all -j
make -C tests/sys/shell/ test
```

Even applications requiring network interface access should be able to work:

```sh
sudo dist/tools/tapsetup/tapsetup
make -C examples/gnrc_networking all -j16
make -C examples/gnrc_networking term
```

```
> ifconfig
ifconfig
Iface  6  HWaddr: F1:28:23:23:F1:28
          L2-PDU:1500  MTU:1500  HL:64  RTR
          RTR_ADV
          Source address length: 6
          Link type: wired
          inet6 addr: fe80::f328:23ff:fe23:f128  scope: link  TNT[1]
          inet6 group: ff02::2
          inet6 group: ff02::1
          inet6 group: ff02::1:ff23:f128

          Statistics for Layer 2
            RX packets 5  bytes 738
            TX packets 2 (Multicast: 2)  bytes 78
            TX succeeded 2 errors 0
          Statistics for IPv6
            RX packets 4  bytes 340
            TX packets 2 (Multicast: 2)  bytes 128
            TX succeeded 2 errors 0
```

[Vagrantfile]: ./Vagrantfile
[native]: https://doc.riot-os.org/group__boards__native.html
