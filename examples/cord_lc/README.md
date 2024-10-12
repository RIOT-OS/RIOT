CoRE Resource Directory: Lookup Example
=========================================

This example application demonstrates the usage of RIOT's Resource Directory
(RD) lookup module, called `cord_lc`. This module supports the lookup of
resources and endpoints as defined in
[RFC 9176](https://datatracker.ietf.org/doc/html/rfc9176).
The lookup can be done in two modes: a) raw: result of the lookup is returned
as is. b) pre-parsed: result of the lookup is parsed and only one link is
returned for each call.

Usage
=====
The examples includes a shell command that you can use to interact with a given
RD, called `cord_lc`. Simply use that shell command without parameters for
more information on its usage.
