## Requirements

- currently, the host side only compiles on Linux

## Usage

To use, add

    #
    USEMODULE += stdio_ethos gnrc_netdev
    CFLAGS += '-DETHOS_UART=UART_DEV(0)' -DETHOS_BAUDRATE=115200

to app Makefile, "make clean all flash", then run this tool as follows:
    # sudo ./ethos <tap-device> <serial>

## setup_network.sh

This script sets up a tap device, configures a prefix and starts a uhcpd server
serving that prefix towards the tap device.
The tap device will be owned by the calling user, or if called with sudo, with
the user that uses sudo. That way, ethos can use it without being root / using
sudo.

E.g., use as follows:

    $ sudo ./setup_network.sh riot0 2001:db8::0/64

Keep that running, then in another shell start ethos:

    $ ethos riot0 <serial>
