# Introduction

This test application allows to test the riotboot_flashwrite module over coap.

WARNING: never use this code for anything else. It allows updating the device
over network without *any* kind of authentication or other security!

Please see the README of examples/nanocoap_server for instructions on how to
set up a network for testing.

# How to test over Ethos

First set up the network:

    $ sudo dist/tools/ethos/setup_network.sh riot0 2001:db8::/64

Then provide de device:

    $ BOARD=<board> make flash test

# How to test over the air (802.15.4)

On another device setup a BR and start `start_network.sh` on that device serial
port.

    $ BOARD=<board> make -C examples/gnrc_border_router flash

    $ sudo dist/tools/ethos/start_network.sh /dev/ttyACMx riot0 2001:db8::/64

Then provide de device to test:

    $ BOARD=<board> make flash test
