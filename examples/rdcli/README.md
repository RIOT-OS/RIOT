CoRE Resource Directory Client Example
======================================
This example application demonstrates the usage of RIOT's Resource Directory
(RD) client module, called `rdcli`. This module supports the registration,
update, and removal processes as defined in the
[Resource Directory Draft](https://tools.ietf.org/html/draft-ietf-core-resource-directory-14).

Usage
=====
The examples includes a shell command that you can use to interact with a given
RD server, called `rdcli`. Simply use that shell command without parameters for
more information on its usage.

Some connection parameters are configured statically during compile time,
namely the lifetime (`RDCLI_LT`) and the node's endpoint name (`RDCLI_EP`). You
can change these values during command line by overriding these values in the
application's Makefile, e.g. add
```
CFLAGS += "-DRDCLI_EP=\"MyNewEpName\""
```
