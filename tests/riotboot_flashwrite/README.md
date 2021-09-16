# Introduction

This test application allows to test the riotboot_flashwrite module over coap.

WARNING: never use this code for anything else. It allows updating the device
over network without *any* kind of authentication or other security!

Please see the README of examples/nanocoap_server for instructions on how to
set up a network for testing.

## Requirements

This test uses [aiocoap](https://pypi.org/project/aiocoap/) to send the firmware to the device over coap.

### How to test over Ethos

First set up the network:

    $ sudo dist/tools/ethos/setup_network.sh riot0 2001:db8::/64

Then provide de device and test:

    $ BOARD=<board> make flash test-with-config

### How to test over the air (802.15.4)

On another device setup a BR and start `start_network.sh` on that device serial
port.

    $ BOARD=<board> make -C examples/gnrc_border_router flash

    $ sudo dist/tools/ethos/start_network.sh /dev/ttyACMx riot0 2001:db8::/64

Then provide the device and test:

    $ USE_ETHOS=0 BOARD=<board> make flash test-with-config

### Manual test

First, compile and flash with riotboot enabled:

    $ BOARD=<board> make flash

Confirm it booted from slot 0 (it should print "Current slot=0"), then
recompile in order to get an image for the second slot with a newer version
number:

    $ BOARD=<board> make riotboot

Then send via CoAP, for example, with libcoap's coap_client:

    $ coap-client -m post coap://[<ip address of node>]/flashwrite \
       -f bin/<board>/riotboot_files/slot1.bin -b 64

Then reboot the node manually, confirming that it booted from slot 1.
