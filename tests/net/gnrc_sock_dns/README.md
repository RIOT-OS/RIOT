# Overview

This folder contains a test application for RIOT's sock-based DNS client.

# How to test with native

Setup up a tap interface:

    $ sudo ip tuntap add dev tap0 mode tap user $(id -u -n)
    $ sudo ip a a 2001:db8::1/64 dev tap0
    $ sudo ip link set up dev tap0

Start dnsmasq (in another console):

    $ sudo dnsmasq -d -2 -z -i tap0 -q --no-resolv \
        --dhcp-range=::1,constructor:tap0,ra-only \
        --host-record=example.org,10.0.0.1,2001:db8::1

(NetworkManager is known to start an interfering dnsmasq instance. It needs to
be stopped before this test.)

Then run the test application

    $ make term

Now use the RIOT shell to configure the DNS server and request `example.org`
from it

    > dns server 2001:db8::1
    > dns server
    DNS server: [2001:db8::1]:53
    > dns request example.org
    example.org resolves to 2001:db8::1
