This README contains information how to establish an IPv6 connecton between
Linux BLE router and an IPSP enabled BLE device.

Prerequisites
=============
In general, any device capable of running Linux operating system, can be used
as a BLE router provided the following conditions are met:

* Linux Kernel >3.18 and <=4.12 is used
* bluez, libcap-ng0, radvd tools are present.

If a built-in Bluetooth device is not available then Bluetooth 4.0 compatible
USB dongle can be used.

The following procedures have been tested on Ubuntu 15.10 and Ubuntu 16.04.

Establishing an IPv6 connection
===============================
Use the following procedure to establish a connection between an nRF52 device
and Linux router:

First enable 6LoWPAN module. This is necessary only once per session:

	# Log in as a root user.
	sudo su
	
	# Mount debugfs file system.
	mount -t debugfs none /sys/kernel/debug
	
	# Load 6LoWPAN module.
	modprobe bluetooth_6lowpan
	
	# Enable the bluetooth 6lowpan module.
	echo 1 > /sys/kernel/debug/bluetooth/6lowpan_enable
	
	# Look for available HCI devices.
	hciconfig
	
	# Reset HCI device - for example hci0 device.
	hciconfig hci0 reset
	
	# Read 00:AA:BB:XX:YY:ZZ address of the nRF5x device.
	hcitool lescan
	
If you see device name and address in lescan output then you can connect to the
device:
	
	echo "connect 00:AA:BB:XX:YY:ZZ 1" > /sys/kernel/debug/bluetooth/6lowpan_control
	
If above is successful then LED1 will stop blinking and LED2 will switch on.
You can then check the connection using the following commands:

	# Check if bt0 interface is present and up
	ifconfig
	
	# Try to ping the device using its link-local address, for example, on bt0 interface.
	ping6 fe80::2aa:bbff:fexx:yyzz%bt0
	
If you'd like to learn more about the procedure please refer to
[Connecting devices to the router].

Distributing routable IPv6 prefix
=================================
In Linux, Router Advertisement Daemon (RADVD) can be used to distribute prefixes
in the network, hance configure routable IPv6 address.

To configure RADVD create `/etc/radvd.conf` file and paste the following contents:

	interface bt0
	{
	    AdvSendAdvert on;
	    prefix 2001:db8::/64
	    {
	        AdvOnLink off;
	        AdvAutonomous on;
	        AdvRouterAddr on;
	    };
	};

Next, start RADVD daemon:

	# Set IPv6 forwarding (must be present).
	sudo echo 1 > /proc/sys/net/ipv6/conf/all/forwarding
	# Run radvd daemon.
	sudo service radvd restart
	
If successful then all devices connected to the host will receive
a routable `2001:db8` prefix.

This can be verified by sending echo request to the full address:

	ping6 -I bt0 2001:db8::2aa:bbff:fexx:yyzz

where `aa:bbff:fexx:yyzz` is device Bluetooth address.

If you'd like to learn more about the procedure please refer to
[Distributing a global IPv6 prefix].

* [Connecting devices to the router]: http://developer.nordicsemi.com/nRF5_IoT_SDK/doc/0.9.0/html/a00089.html
* [Distributing a global IPv6 prefix]: http://developer.nordicsemi.com/nRF5_IoT_SDK/doc/0.9.0/html/a00090.html
