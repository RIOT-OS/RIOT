microcoap server example
========================

This application is meant to get you started with implementing a CoAP server on RIOT.
It uses the GNRC network stack through RIOT's conn socket API.

Usage
=====

To try out the server on native, compile it with

```
$ make all
```

Then, create a tap interface (to which RIOT will connect):

```
$ sudo ip tuntap add tap0 mode tap user ${USER}
$ sudo ip link set tap0 up
```

Run the resulting RIOT binary by invoking:

```
$ make term
```

The application is now listening on all it's configured IP addresses.

Now find out its link\_layer address:


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

Testing
=======
There are multiple external CoAP clients you can use to test the server on native.

libcoap CLI
-----------

(replace "fe80::f8bf:2bff:fe01:9ea3" with your link-layer address)

```
# coap-client "coap://[fe80::f8bf:2bff:fe01:9ea3%tap0]/riot/board"
```

Copper (Firefox Plugin)
-----------------------

The Copper plugin for Firefox provides you with a nice graphical interface, but
getting it to work with RIOT requires a little setup.

Make sure you've installed

- The [Firefox Copper plugin](https://addons.mozilla.org/en-US/firefox/addon/copper-270430/)
- The Router Advertisement Daemon (radvd) 

Enter the following into your `/etc/radvd.conf` (if it doesn't exist yet, create one):

```
interface tap0
{
        AdvSendAdvert on;
        MinRtrAdvInterval 3;
        MaxRtrAdvInterval 10;
        prefix fc00::/64
        {
                AdvOnLink on;
                AdvAutonomous on;
                AdvRouterAddr off;
        };
};
```

(you can use `radvd -c` to check for syntax errors)

and run

```
/bin/systemctl start radvd.service
```

Now you can enter

```
coap://[fc00::1]/riot/board
```

into your Firefox address bar.
