CoRE Resource Directory: Endpoint Example
=========================================

This example application demonstrates the usage of RIOT's Resource Directory
(RD) endpoint module, called `cord_ep`. This module supports the registration,
update, and removal procedures as defined in
[draft-ietf-core-resource-directory-15](https://tools.ietf.org/html/draft-ietf-core-resource-directory-15).

Usage
=====
The examples includes a shell command that you can use to interact with a given
RD, called `cord_ep`. Simply use that shell command without parameters for
more information on its usage.

Some connection parameters are configured statically during compile time,
namely the lifetime (`CORD_LT`) and the node's endpoint name (`CORD_EP`). You
can change these values at compile time by overriding their defines using
command line arguments:
```
CFLAGS="-DCORD_EP=\\\"your_ep_name_here\\\"" make all
```
or by setting their values in the application's Makefile:
```
CFLAGS += "-DCORD_EP=\"MyNewEpName\""
```
