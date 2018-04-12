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
override the `RD_ADDR` and `RD_PORT` variables, e.g.:
```
RD_ADDR=\\\"::1\\\" RD_PORT=12345 make all
```
or
```
RD_ADDR=\\\"abc:0815::123\\\" make all
```
