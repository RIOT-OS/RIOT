# gnrc_networking example

This example shows you how to try out the code in two different ways: Either by communicating
between the RIOT machine and its Linux host, or by communicating between two RIOT instances.
Note that the former only works with native, i.e. if you run RIOT on your Linux machine.

## Connecting RIOT native and the Linux host

> **Note:** RIOT does not support IPv4, so you need to stick to IPv6 anytime. To
establish a connection between RIOT and the Linux host, you will need `netcat`
(with IPv6 support). Ubuntu 14.04 comes with netcat IPv6 support pre-installed.
On Debian it's available in the package `netcat-openbsd`. Be aware that many
programs require you to add an option such as -6 to tell them to use IPv6, otherwise they
will fail. If you're using a _Raspberry Pi_, run `sudo modprobe ipv6` before trying
this example, because raspbian does not load the IPv6 module automatically.
On some systems (openSUSE for example), the _firewall_ may interfere, and prevent
some packets to arrive at the application (they will however show up in Wireshark,
which can be confusing). So be sure to adjust your firewall rules, or turn it off
(who needs security anyway).

First, create a tap interface:

    sudo ip tuntap add tap0 mode tap user ${USER}
    sudo ip link set tap0 up

Now you can start the `gnrc_networking` example by invoking `make term`. This should
automatically connect to the `tap0` interface. If this doesn't work for any reason,
run make term with the tap0 interface as the PORT environment variable:

    PORT=tap0 make term

To verify that there is connectivity between RIOT and Linux, go to the RIOT console and run `ifconfig`:

    > ifconfig
    Iface  7   HWaddr: ce:f5:e1:c5:f7:5a
               inet6 addr: ff02::1/128  scope: local [multicast]
               inet6 addr: fe80::ccf5:e1ff:fec5:f75a/64  scope: local
               inet6 addr: ff02::1:ffc5:f75a/128  scope: local [multicast]

Copy the [link-local address](https://en.wikipedia.org/wiki/Link-local_address)
of the RIOT node (prefixed with `fe80`) and try to ping it **from the Linux node**:

    ping6 fe80::ccf5:e1ff:fec5:f75a%tap0

Note that the interface on which to send the ping needs to be appended to the IPv6
address, `%tap0` in the above example. When talking to the RIOT node, you always want
to send to/receive from the `tap0` interface.

If the pings succeed you can go on to send UDP packets. To do that, first start a
UDP server on the RIOT node:

    > udp server start 8808
    Success: started UDP server on port 8808

Now, on the Linux host, you can run netcat to connect with RIOT's UDP server:

    nc -6uv fe80::ccf5:e1ff:fec5:f75a%tap0 8808

The `-6` option is necessary to tell netcat to use IPv6 only, the `-u` option tells
it to use UDP only, and the `-v` option makes it give more verbose output (this one is optional).

You should now see that UDP messages are received on the RIOT side. Opening a UDP
server on the Linux side is also possible. To do that, write down the IP address
of the host (run on Linux):

    ifconfig tap0
    tap0     Link encap:Ethernet  HWaddr ce:f5:e1:c5:f7:59
            inet6 addr: fe80::4049:5fff:fe17:b3ae/64 Scope:Link
            UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
            RX packets:6 errors:0 dropped:0 overruns:0 frame:0
            TX packets:36 errors:0 dropped:0 overruns:0 carrier:0
            collisions:0 txqueuelen:0
            RX bytes:488 (488.0 B)  TX bytes:3517 (3.5 KB)

Then open a UDP server on Linux (the `-l` option makes netcat listen for incoming connections):

    nc -6ul 8808

Now, on the RIOT side, send a UDP packet using:

    udp send fe80::4049:5fff:fe17:b3ae 8808 testmessage

You should see `testmessage` appear in netcat. Instead of using netcat, you can of
course write your own software, but you may have to bind the socket to a specific
interface (tap0 in this case). For an example that shows how to do so, see
[here](https://gist.github.com/backenklee/dad5e80b764b3b3d0d3e).

## Connecting two RIOT instances

When using native (i.e. when you're trying this on your Linux machine), you first
need to set up two tap devices and a bridge that connects them. This constitutes a
virtual network that the RIOT instances can use to communicate.

    ./../../dist/tools/tapsetup/tapsetup --create 2

Then, make sure you've compiled the application by calling `make` and start the
first RIOT instance by invoking `make term`. In the RIOT shell, get to know the
IP address of this node:

    > ifconfig
    Iface  7   HWaddr: ce:f5:e1:c5:f7:5a
               inet6 addr: ff02::1/128  scope: local [multicast]
               inet6 addr: fe80::ccf5:e1ff:fec5:f75a/64  scope: local
               inet6 addr: ff02::1:ffc5:f75a/128  scope: local [multicast]

and start a UDP server.

    > udp server start 8808

This node is now ready to receive data on port `8808`.

In a second terminal, start a second RIOT instance, this time listening on `tap1`:

    PORT=tap1 make term

In the RIOT shell, you can now send a message to the first RIOT instance:

    > udp send fe80::ccf5:e1ff:fec5:f75a 8808 testmessage

*(Make sure to copy the actual
[link-local address](https://en.wikipedia.org/wiki/Link-local_address) of your first
RIOT instance into the above command)*

In your first terminal, you should now see output that looks like this.

    > PKTDUMP: data received:
    ~~ SNIP  0 - size:  11 byte, type: NETTYPE_UNDEF (0)
    000000 74 65 73 74 6d 65 73 73 61 67 65
    ~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (3)
       src-port:  8808  dst-port:  8808
       length: 19  cksum: 0x4d95f
    ~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (1)
    traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)
    flow label: 0x00000
    length: 19  next header: 17  hop limit: 64
    source address: fe80::a08a:84ff:fe68:544f
    destination address: fe80::60fc:3cff:fe5e:40df
    ~~ SNIP  3 - size:  20 byte, type: NETTYPE_NETIF (-1)
    if_pid: 6  rssi: 0  lqi: 0
    src_l2addr: a2:8a:84:68:54:4f
    dst_l2addr: 62:fc:3c:5e:40:df
    ~~ PKT    -  4 snips, total size:  79 byte
