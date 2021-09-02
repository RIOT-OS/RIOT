# Overview

This folder contains a test application for RIOT's DHCPv6 client.

# How to test

The test script requires [Kea] as DHCPv6 server. It is available as
`kea-dhcp6-server` on Ubuntu since Ubuntu 16.04:

```sh
apt-get install kea-dhcp6-server
```

On Arch Linux it is available in the `kea` package:

```sh
pacman -Syu kea
```

If you use any platform other than `native`, you need to use `ethos`, otherwise
`netdev_tap` is chosen.

An instance of Kea that configured via [kea-dhcp6.conf](kea-dhcp6.conf) is
started in parallel to `make term`/`make test-with-config`.

Read the [Kea documentation] on the configuration file for more information.

The default set-up is configured so a `2001:db8::/32` is delegated via the
`tapbr0` bridge as created with the `dist/tools/tapsetup/tapsetup` script.
If you created your interface and without the script, please reconfigure Kea by
search & replacing "`tapbr0`" in the configuration file.

```sh
BOARD=samr21-xpro make flash test-as-root
```

[Kea]: http://kea.isc.org
[Kea documentation]: https://kea.readthedocs.io/en/latest/arm/config.html
