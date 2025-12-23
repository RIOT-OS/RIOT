Tests for lwIP's sock_udp port
==============================

This tests the `sock_udp` port of lwIP. There is no network device needed since
a [virtual device](http://api.riot-os.org/group__sys__netdev__test.html) is
provided at the backend.

These tests test both IPv4 and IPv6 capabilities. They can be activated by
the `LWIP_IPV4` and `LWIP_IPV6` environment variables to a non-zero value.
IPv6 is activated by default:

```sh
make all test
# or
LWIP_IPV6=1 make all test
```

To just test IPv4 set the `LWIP_IPV4` to a non-zero value (IPv6 will be
deactivated automatically):

```sh
LWIP_IPV4=1 make all test
```

To test both set the `LWIP_IPV4` and `LWIP_IPV6` to a non-zero value:

```sh
LWIP_IPV4=1 LWIP_IPV6=1 make all test
```

Since lwIP uses a lot of macro magic to activate/deactivate these capabilities
it is advisable to **test all three configurations individually** (just IPv4,
just IPv6, IPv4/IPv6 dual stack mode).
