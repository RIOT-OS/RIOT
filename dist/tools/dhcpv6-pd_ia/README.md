# DHCPv6 PD_IA server

This provides tooling to bootstrap a [DHCPv6] server with [prefix delegation]
support ([Kea]) to provide a prefix for the [`gnrc_border_router` example]'s
or similar applications' subnets.

# Usage
Just run the script `dhcpv6-pd_ia.py` with sudo, e.g.

```sh
sudo ./dhcpv6-pd_ia.py tap0 2001:db8::/32
```

For more information on the arguments, have a look at the usage information of
the script

```sh
./dhcpv6-pd_ia.py -h
```


[DHCPv6]: https://tools.ietf.org/html/rfc8415
[prefix delegation]: https://en.wikipedia.org/wiki/Prefix_delegation
[Kea]: http://kea.isc.org
[`gnrc_border_router` example]: ../../../examples/networking/gnrc_networking/gnrc_borader_router
