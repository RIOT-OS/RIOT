@defgroup net_unicoap_drivers_rfc7252_pdu RFC 7252 Framing
@ingroup  net_unicoap_drivers
@brief    Parse and serialize RFC 7252 PDUs
@{

Module. Specify `USEMODULE += unicoap_driver_rfc7252_pdu` in your application's Makefile.

This module allows you to use the RFC 7252 parser without having to import the
@ref net_unicoap_drivers_udp or @ref net_unicoap_drivers_dtls, i.e., without a network backend.
This is particularly handy if you want to experiment with `unicoap` or write tests that don't
need the ability to do network I/O.

## PDU Format

```
0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Ver| T |  TKL  |      Code     |          Message ID           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   Token (if any, TKL bytes) ...
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   Options (if any) ...
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|1 1 1 1 1 1 1 1|    Payload (if any) ...
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

@see [RFC 7252, Message Format](https://datatracker.ietf.org/doc/html/rfc7252#section-3)
@}
