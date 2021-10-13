# IPv6-over-BLE: Connecting Linux with RIOT using BLE

This README contains information how to establish an IPv6-over-BLE connection
between Linux and RIOT (using GNRC and NimBLE).

**NOTE 1:** IPv6-over-BLE between RIOT and Linux is **highly experimental** at
the moment and does suffer stability issues!

**NOTE 2:** Currently, Linux does not support 6LoWPAN neighbor discovery (which
RIOT uses per default with BLE), so RIOT needs to be compiled to use stateless
address auto configuration (SLAAC) -> `CFLAGS=-DCONFIG_GNRC_IPV6_NIB_SLAAC=1`.

## Prerequisites

You need a BLE (NimBLE) capable RIOT board (currently only `nrf52`-based boards
are supported), and a Linux host with at least Bluetooth 4.0 capabilities.

Any Linux distribution should do fine, however this guide was tested with the
following:
- Mint 18.3
- Kernel 4.15.0
- bluez 5.37
- [optional] radvd 2.11


## Preparing the RIOT node

First, you compile and flash the `examples/gnrc_networking` application to your
RIOT device. When doing this, make sure to enable SLAAC
(`CFLAGS=-DCONFIG_GNRC_IPV6_NIB_SLAAC=1`), see note above.

Once the firmware is running, you can verify it by typing

    ble info

This should give you some information about your device's BLE address and
status information about open connections. It should look similar to the
following:

    Own Address: UU:VV:WW:XX:YY:ZZ -> [FE80::UUVV:WWFF:FEXX:YYZZ]
     Free slots: 3/3
    Advertising: no
       Contexts:
    [ 0] state: 0x8000 - unused
    [ 1] state: 0x8000 - unused
    [ 2] state: 0x8000 - unused

Once this is working, you need to tell the node to accept incoming connections
and start advertising itself:

    ble adv

That's it, the node can now be seen and connected to from Linux.


## Establishing a connection from Linux

### Preparing Linux

First of all, the Linux kernel needs to be configured to use 6lowpan over BLE.
This needs to be done only once per session and consists of the following steps:

    # Log in as a root user (IMPORTANT, as the following steps will not work
    # using explicit `sudo` for each step...)
    sudo su

    # Mount debugfs file system, already the case on many distributions
    mount -t debugfs none /sys/kernel/debug

    # Load 6LoWPAN module
    modprobe bluetooth_6lowpan

    # Enable the bluetooth 6lowpan module.
    echo 1 > /sys/kernel/debug/bluetooth/6lowpan_enable

Next, we should verify that our Bluetooth device is configured properly:

    # Look for available HCI devices.
    hciconfig

This should show us some information about the available Bluetooth devices. If no
device is listed here, something is wrong...


### Connecting to the RIOT node

When connecting to the RIOT device, you need to know the BLE address of the
device. There are two ways to find out:

a) use `ble info` or `ifconfig` on the RIOT device directly and copy the address

b) use `bluetoothctl` on Linux to scan for the device. Once `bluetoothctl` has
   started, issue `scan on` to start scanning. The default name for the RIOT
   device is set to `RIOT-GNRC`, so you should see it pop up.

c) use `hcitool lescan` on the Linux host (older tool, predecessor of
   `bluetoothctl`...)

Once you have the address, you simply connect Linux to RIOT using the following
command:

    # Put your device address here...
    # Note: the 2 after the address denotes a BLE public random address, default
    #       used by `nimble_netif`
    echo "connect UU:VV:WW:XX:YY:ZZ 2" > /sys/kernel/debug/bluetooth/6lowpan_control

That's it, you now have a IPv6 connection to your RIOT node. You can verify this
using `ifconfig`, where something like the following should be visible:

    bt0       Link encap:UNSPEC  HWaddr 00-19-86-00-16-CA-00-00-00-00-00-00-00-00-00-00
          inet6 addr: fe80::19:86ff:fe00:16ca/64 Scope:Link
          UP RUNNING MULTICAST  MTU:1280  Metric:1
          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
          TX packets:18 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:0 (0.0 B)  TX bytes:1624 (1.6 KB)

You should also be able to ping your RIOT node. The devices link local address
is also printed when running `ble info` on your RIOT device.

    # Substitute the actual device address,
    ping6 fe80::uuvv:wwff:fexx:yyzz%bt0

Now everything should be fine :-)


## [optional] Distributing a routeable Prefix

You can use the Router Advertisement Daemon (`radvd`) in Linux to automatically
distribute prefixes in your BLE network. For the following, you need to make
sure that `radvd` is installed on your Linux host.

As a first step, we need to enable IPv6 forwarding in Linux:

    sudo echo 1 > /proc/sys/net/ipv6/conf/all/forwarding

Next, we configure `radvd` (`etc/radvc.conf`) by using a configuration like
this:

    interface bt0
    {
        AdvSendAdvert on;
        prefix 2001:db8::/64
        {
            AdvOnLink off;
            AdvAutonomous on;
            AdvRouterAddr on;
        };
        abro 2001:db8::zzyy:xxff:feuu:vvww
        {
            AdvVersionLow 10;
            AdvVersionHigh 2;
            AdvValidLifeTime 2;
        };
    };

This will tell Linux to advertise the prefix `2001:db8::/64`. Do not forget to
substitute the suffix of global address given in the `abro` section
with the one of the BLE device on your Linux host.

**NOTE:** The `abro` section is needed as otherwise the RIOT node will discard
router advertisements, as it is in 6LN configuration
(see https://tools.ietf.org/html/rfc6775#section-4.3).

With this, simply (re-)start the daemon:

    sudo service radvd restart

or

    sudo systemctl restart radvd

Again, that's it. Your RIOT node should now have an address using the above
prefix assigned. Simply verify with `ifconfig` on the RIOT node.

Also you should be able to ping the RIOT node from Linux:

    # make sure to use the actual devices address here...
    ping6 -I bt0 2001:db8::uuvv:wwff:fexx:yyzz
