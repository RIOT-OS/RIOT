# Introduction

This test application allows to test the riotboot_flashwrite module over coap.

WARNING: never use this code for anything else. It allows updating the device
over network without *any* kind of authentication or other security!

Please see the README of examples/nanocoap_server for instructions on how to
set up a network for testing.

# How to test

First, compile and flash with riotboot enabled:

    $ BOARD=<board> make riotboot/flash

Confirm it booted from slot 0 (it should print "Current slot=0"), then
recompile in order to get an image for the second slot with a newer version
number:

    $ BOARD=<board> make riotboot

Then send via CoAP, for example, with libcoap's coap_client:

    $ coap-client -m post coap://[<ip address of node>]/flashwrite \
       -f bin/<board>/tests_riotboot_flashwrite-slot1.riot.bin -b 64

Then reboot the node manually, confirming that it booted from slot 1.
