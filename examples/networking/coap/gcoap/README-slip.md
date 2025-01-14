`Makefile.slip` assumes use of a SAMR21 Xplained Pro. This file describes how we use the board as a border router. See the [SLIP instructions][1] for the gnrc_border_router example for more background.

## Background -- Building
Build commands, for reference:

    make clean BOARD="samr21-xpro"

    # Use -f only for border router!
    make -f Makefile.slip BOARD="samr21-xpro"

    ../../dist/tools/usb-serial/list-ttys.sh
    # Use -f only for border router!
    make -f Makefile.slip flash BOARD="samr21-xpro" SERIAL="???"

    make term BOARD="samr21-xpro" PORT="/dev/ttyACM???"

## USB serial port
The USB serial port requires a USB-TTL converter cable with 3.3 V output.

Pin connections for SAMR21 board:

* PA23 (RX) -- TXD wire
* PA22 (TX) -- RXD wire
* GND -- GND wire

## Network configuration
We will define two networks:

1. `bbbb` Between Border Router (BR) and Ubuntu host over TUN
2. `aaaa` Between RIOT router node and BR over 6LoWPAN

We include two approaches to configuration below, automated via RPL, and manual via fib.

First, define a TUN interface on an Ubuntu host with tunslip, in the `dist/tools/tunslip` directory. In the example below, the tun interface is host 1.

    cd ../../dist/tools/tunslip/
    sudo ./tunslip6 -s ttyUSB0 -t tun0 bbbb::1/64
    sudo ip -6 route add aaaa::/64 dev tun0

### RPL-based configuration

Configure the interface to the `bbbb` network on the BR from the RIOT terminal. The example commands below assume SLIP is on interface 8.

    # Set address for SLIP interface
    ifconfig 8 add unicast bbbb::2/64
    # Add the Ubuntu host to the neighbor cache
    ncache add 8 bbbb::1

Next configure the interface for the `aaaa` network on the BR. Strictly speaking, it is not necessary to define this address. However, it is convenient for typing, because RIOT automatically creates a more verbose address based on the MAC address for the interface.

    ifconfig 7 add unicast aaaa::1/64
    rpl init 7

Next, configure the interface on the `aaaa` network on the RIOT router mote.

    ifconfig 7 add unicast aaaa::2/64
    rpl init 7

Finally, set the BR as the RPL DAG root.

    rpl root 1 aaaa::1

Ping the TUN interface from the router mote, via the BR:

    ping bbbb::1

[1]: https://github.com/RIOT-OS/RIOT/tree/master/examples/gnrc_border_router    "SLIP instructions"