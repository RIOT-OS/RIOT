# gnrc_networking example

This example shows you how to try out the code

## Connecting two RIOT instances

When using native (i.e. when you're trying this on your Linux machine),
you first need to set up two tap devices and a bridge that connects
them. This constitutes a virtual network that the RIOT instances can
use to communicate.

    sudo ./../../dist/tools/tapsetup/tapsetup --create 2

Then, make sure you've compiled the application by calling `make` and
start the first RIOT instance by invoking `make term`. In the RIOT
shell, get to know the IP address of this node:

    > ifconfig
    Iface  7   HWaddr: ce:f5:e1:c5:f7:5a
               inet6 addr: ff02::1/128  scope: local [multicast]
               inet6 addr: fe80::ccf5:e1ff:fec5:f75a/64  scope: local
               inet6 addr: ff02::1:ffc5:f75a/128  scope: local [multicast]

and start a UDP server.

    > udp server start 8808

This node is now ready to receive data on port `8808`.

In a second terminal, start a second RIOT instance, this time listening
on `tap1`:

    PORT=tap1 make term

In the RIOT shell, you can now send a message to the first RIOT
instance:

    > udp send fe80::ccf5:e1ff:fec5:f75a 8808 testmessage

*(Make sure to copy the actual
[link-local address](https://en.wikipedia.org/wiki/Link-local_address)
of your first RIOT instance into the above command)*

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
