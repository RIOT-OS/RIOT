# libcoap client example

This example shows how to configure a client to use libcoap

## Fast configuration (Between RIOT instances):

Preparing the logical interfaces:

    sudo ./../../dist/tools/tapsetup/tapsetup --create 2

## Client invocation
For the client:

    PORT=tap0 make term
    coapc coap://[ip6-address]/some/path

The IP address to connect to needs to be that as returned by libcoap_server,
or that of the tap0 interface, etc.

## Handling the static memory allocation

libcoap for RIOT is using the `sys/memarray` module and therefore there
are certain limits. Said resources are defined in `libcoap/src/coap_mem.c`,
but can be overwritten at compile time.
