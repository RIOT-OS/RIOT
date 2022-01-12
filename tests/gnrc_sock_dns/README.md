# Overview

This folder contains a test application for RIOT's sock-based DNS client.

## Testing via test script

### Requirements

Scapy must be installed on the system to run the python test script.

    $ pip3 install scapy

Setup up a tap interface:

    $ sudo ip tuntap add dev tap0 mode tap user $(id -u -n)
    $ sudo ip a a 2001:db8::1/64 dev tap0
    $ sudo ip link set up dev tap0

### Execution

Run the automated test with

    $ make flash test-with-config

## Testing manually with native

Setup up a tap interface:

    $ sudo ip tuntap add dev tap0 mode tap user $(id -u -n)
    $ sudo ip a a 2001:db8::1/64 dev tap0
    $ sudo ip link set up dev tap0

Start dnsmasq (in another console):

    $ sudo dnsmasq -d -2 -z -i tap0 -q --no-resolv \
        --dhcp-range=::1,constructor:tap0,ra-only \
        --host-record=testdomain.riot,1.2.3.4,1:2:3:4:5:6:7:8

If the above command fails with the following message

    dnsmasq: failed to create listening socket for port 53: Address already in use

you can check which program is already running with

    $ sudo ss -lp "sport = :domain"

Probably it will be `systemd-resolved`. You can stop the service with

    $ sudo systemctl stop systemd-resolved

Afterwards you can retry the `dnsmasq` command from above.

Then run the test application

    $ make flash term

## Shell commands

Set up a DNS server:

    > dns server <4 or 6 (IPv4/IPv6)> <DNS server addr> [DNS server port]

Query an IP for a domain name:

    > dns request <0, 4 or 6 (Unspecified/IPv4/IPv6)> <name>

Examples:

    > dns server 6 2001:db8::1
    > dns server
    DNS server: [2001:db8::1]:53
    > dns query 6 testdomain.riot
    testdomain.riot resolves to 1:2:3:4:5:6:7:8
