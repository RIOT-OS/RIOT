CoRE Resource Directory: Simple Registration Example
====================================================

This example shows how a node can register with a CoRE resource directory using
the simple registration process as described in
draft-ietf-core-resource-directory-11, section 5.3.2.

The registration process needs an endpoint name as well as a lifetime for the
registry entry. You can edit these values by overriding `RDCLI_EP` and
`RDCLI_LT`:
```
CFLAGS="-DRDCLI_LT=\"7200\" -DRDCLI_EP=\"MyNode\"" make all
```

Per default, the node is looking for the CoRE RD at the all nodes link-local
multicast address [FF02::1]:5683. To change the target address or port, simply
redefine `RDCLI_SERVER_ADDR` and `RDCLI_SERVER_PORT`, e.g.:
```
CFLAGS="-DRDCLI_SERVER_ADDR=IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL" make all
```
