nanocoap server example
=======================

This application is meant to get you started with implementing a CoAP server on RIOT.
It uses the GNRC or LWIP (including IPv4) network stack through RIOT's
[sock API](http://doc.riot-os.org/group__net__sock.html).

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
RIOT native interrupts/signals initialized.
RIOT native board initialized.
RIOT native hardware initialization complete.

main(): This is RIOT! (Version: 2024.04-devel-2305-g5f08f5-examples-nanocoap-lwip)
RIOT nanocoap example application
Waiting for address autoconfiguration...
{"IPv6 addresses": ["fe80::8c9d:7dff:fea1:fdcd"]}
```

The link-layer address in this case is "fe80::8c9d:7dff:fea1:fdcd".

Usage - IPv4
============
Enable LWIP IPv4 setting `LWIP_IPV4 ?= 1` in `Makefile`.

Run program and set IPv4 will be presented in the output:

```
RIOT native interrupts/signals initialized.
RIOT native board initialized.
RIOT native hardware initialization complete.

main(): This is RIOT! (Version: 2024.04-devel-2305-g5f08f5-examples-nanocoap-lwip)
RIOT nanocoap example application
Waiting for address autoconfiguration...
{"IPv4 addresses": ["192.168.100.150"]}
```

Testing
=======

The CoAP server exposes 6 different resources:

* `/.well-known/core`: returns the list of available resources on the server.
This is part of the CoAP specifications. It works only with GET requests.
* `/echo/`: will match any request that begins with '/echo/' and will echo
  the remaining part of the URI path. Meant to show how the prefix works. It
  works only with GET requests.
* `/riot/board`: returns the name of the board running the server. It works
only with GET requests.
* `/riot/value`: returns the value of an internal variable of the server. It
works with GET requests and also with PUT and POST requests, which means that
this value can be updated from a client.
* `/riot/ver`: returns the current RIOT version. Meant to show a block2 reply.
  It works only with GET requests.
* `/sha256`: creates a hash with the received payloads. It is meant to show
  block1 support. It returns the hash when no more blocks are pending. Only
  works with POST.

There are multiple external CoAP clients you can use to easily test the server
running on native.

libcoap CLI
-----------

(replace "fe80::e42a:1aff:feca:10ec" with your link-layer address)

* Get the name of the board:
```
    # coap-client -m get coap://[fe80::e42a:1aff:feca:10ec%tap0]/riot/board
```

* Update and get the internal value:
```
    # coap-client -m put coap://[fe80::e42a:1aff:feca:10ec%tap0]/riot/value -e 42
    # coap-client -m get coap://[fe80::e42a:1aff:feca:10ec%tap0]/riot/value
```

Copper (Firefox Plugin)
-----------------------

The Copper plugin for Firefox provides you with a nice graphical interface, but
getting it to work with RIOT requires a little setup.

Make sure you've installed

- The [Firefox Copper plugin](https://addons.mozilla.org/en-US/firefox/addon/copper-270430/)
- The Router Advertisement Daemon (radvd)

And build the application again using `make`.

Enter the following into your `/etc/radvd.conf` (if it doesn't exist yet, create one):

```
interface tap0
{
    AdvSendAdvert on;

    MinRtrAdvInterval 3;
    MaxRtrAdvInterval 10;

    AdvDefaultPreference low;

    prefix 2001:db8:1:0::/64
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
sudo radvd
```

Then, run the RIOT binary as usual:

```
make term
```

Note that the output listing all configured interfaces contains a globally scoped
address, which you can now use to reach the RIOT instance via Copper. To do so, enter this:

```
coap://[2001:db8:1:0:e42a:1aff:feca:10ec]/riot/board
```

into your Firefox address bar, where you should replace `2001:db8:1:0:e42a:1aff:feca:10ec`
with your RIOT instance's address marked as "scope: **global**".
If you click the big green `GET` button, the word `native` should appear in the
**Payload** text box at the center of the GUI.

**If this doesn't work,** try manually adding a Global address to the tap0 interface:

```
sudo service radvd start
sudo ip address add 2001:db8:1::a/64 dev tap0
make term
```
