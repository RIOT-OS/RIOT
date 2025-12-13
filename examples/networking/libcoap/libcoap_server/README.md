# libcoap server example

This example shows how to configure a server to use libcoap

## Fast configuration (Between RIOT instances):

Preparing the logical interfaces:

    sudo ./../../dist/tools/tapsetup/tapsetup --create 2

## Server invocation
For the server:

    PORT=tap0 make term
    coaps {stop|start}

The server supports requests for
 coap://[ip]/time?ticks
 coap://[ip]/.well-known/core

## Handling the static memory allocation

libcoap for RIOT is using the `sys/memarray` module and therefore there
are certain limits. Said resources are defined in `libcoap/src/coap_mem.c`,
but can be overwritten at compile time.
