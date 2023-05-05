microcoap server test
=====================

This application is meant to get you started with implementing a CoAP server
on RIOT. It uses the GNRC network stack through RIOT's conn socket API.

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
Waiting for address autoconfiguration...
Configured network interfaces:
Iface  5   HWaddr: 96:3c:18:1e:26:f7

           MTU:1500  HL:64  RTR  RTR_ADV
           Source address length: 6
           Link type: wired
           inet6 addr: ff02::1/128  scope: local [multicast]
           inet6 addr: fe80::e42a:1aff:feca:10ec/64  scope: local
           inet6 addr: ff02::1:ffca:10ec/128  scope: local [multicast]
           inet6 addr: ff02::2/128  scope: local [multicast]
           inet6 addr: 2001:db8:1:0:e42a:1aff:feca:10ec/64  scope: global

Waiting for incoming UDP packet...
```

The link-layer address in this case is "fe80::e42a:1aff:feca:10ec", the only
"scope: local" address set.

Testing
=======
There are multiple external CoAP clients you can use to test the server on native.

libcoap CLI
-----------

(replace "fe80::e42a:1aff:feca:10ec" with your link-layer address)

```
# coap-client "coap://[fe80::e42a:1aff:feca:10ec%tap0]/riot/board"
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
