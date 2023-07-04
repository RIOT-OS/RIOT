Tests for lwIP's sock_udp port
==============================

This tests the `sock_udp` port of lwIP. There is no network device needed since
a [virtual device](http://doc.riot-os.org/group__sys__netdev__test.html) is
provided at the backend.

These tests test both IPv4 and IPv6 capabilities. They can be deactivated
individually by the `LWIP_IPV4` and `LWIP_IPV6` environment variables by setting
them to zero.

By default both IPv4 and IPv6 are tested.

    $ make all test

To test only IPv4 run:

    $ LWIP_IPV6=0 make all test

To test only IPv6 run:

    $ LWIP_IPV4=0 make all test

Since lwIP uses a lot of macro magic to activate/deactivate these capabilities
it is advisable to **test all three configurations individually** (just IPv4,
just IPv6, IPv4/IPv6 dual stack mode).
