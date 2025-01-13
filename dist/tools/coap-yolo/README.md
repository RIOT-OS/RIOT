CoAP over YOLO Utils
====================

CoAP over YOLO is using the CoAP over WebSocket serialization but sending the
messages over UDP. The CoAP over WebSocket format depends on a reliable,
order-preserving, duplication-free message transport; which UDP clearly is not.
Hence the name YOLO.

However, if the RIOT note is connected to a Linux host with a single reliable,
order-preserving and duplication-free link, this should work.

This folder contains WebSocket to UDP proxy that forwards messages received
on the WebSocket to a UDP endpoint specified by the command line, and forwards
any replies received via UDP back to the WebSocket. This allows
exposing a CoAP over YOLO as a CoAP over WebSocket. With this you can use any
CoAP over WebSocket implementation such as e.g. `coap-client` from
[libcoap](https://libcoap.net/) to connect to CoAP over YOLO.
