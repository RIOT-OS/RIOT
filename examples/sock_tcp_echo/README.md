TCP Echo Server / Client
========================

This is a simple TCP echo server / client that uses the SOCK API.
It can make use of both the GNRC and the LWIP network stack.
The default is GNRC, to chose LWIP set `LWIP=1` when compiling the example.

## Echo Server

The echo server will echo back any data that it receives.
To start the echo server on port 12345 run

    listen 12345

## Echo Client

The echo client will connect to a server, send some data and wait for the reply.
To send data with the echo client to a server running on 2001:db8::1 run

    send 2001:db8::1 12345 some data
