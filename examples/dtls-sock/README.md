# sock_dtls example

This example shows how to use `sock_dtls` using TinyDTLS

## Fast configuration (Between RIOT instances):

Preparing the logical interfaces:

    ./../../dist/tools/tapsetup/tapsetup --create 2

For the server instance:

    make all; PORT=tap1 make term
    dtlss start

The server address will be printed to the shell during start. 
Do not forget to copy the IPv6 addresses!

For the client:

    PORT=tap0 make term
    dtlsc <IPv6's server address[%netif]> <port> "DATA to send under encrypted channel!"
