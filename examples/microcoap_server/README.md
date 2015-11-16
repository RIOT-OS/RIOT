microcoap server example
========================

This application is meant to get you started with impementing a CoAP server on RIOT.
It uses the gnrc network stack through RIOT's conn socket API.


Usage
=====

To try on native, compile with

```
# make all
```

Then run the resulting binary, e.g.,

```
# make term
```

The application is now listening on all it's configured IP addresses.

Now find out it's link\_layer address:


```
$ make term
/home/kaspar/src/riot/examples/microcoap_server/bin/native/microcoap_server.elf tap0
RIOT native interrupts/signals initialized.
LED_GREEN_OFF
LED_RED_ON
RIOT native board initialized.
RIOT native hardware initialization complete.

main(): This is RIOT! (Version: 2015.12-devel-632-g8f451-booze-add_conn_coap_example)
RIOT microcoap example application
Configured network interfaces:
Iface  5   HWaddr: fa:bf:2b:01:9e:a3
           MTU:1280  HL:64
           Source address length: 6
           Link type: wired
           inet6 addr: ff02::1/128  scope: local [multicast]
           inet6 addr: fe80::f8bf:2bff:fe01:9ea3/64  scope: local
           inet6 addr: ff02::1:ff01:9ea3/128  scope: local [multicast]
           inet6 addr: ff02::2/128  scope: local [multicast]

Waiting for incoming UDP packet...
```

The link-layer address in this case is "fe80::f8bf:2bff:fe01:9ea3", the only
"scope: local" address set.

Connect using libcoap CLI
=========================

(replace "fe80::f8bf:2bff:fe01:9ea3" with your link-layer address)

```
# coap-client coap://[fe80::f8bf:2bff:fe01:9ea3%tap0]/riot/board
```
