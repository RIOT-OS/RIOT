@defgroup net_unicoap_client_uri URI Client Support
@ingroup  net_unicoap_client
@brief    Use Uniform Resource Identifiers (URIs) to address remote CoAP resources


Module. Specify `USEMODULE += unicoap_client_uri` in your application's Makefile.

You do not need to import a CoAP driver to use the CoAP URI parser. The path, host, and URI queries
will automatically be added as CoAP options to the request. IPv4 and IPv6 address literals are
supported. Zone identifiers are supported too.

```c
unicoap_destination_t = unicoap_destination_uri("coap://[2008:db8::1]/foo/bar?a=1&b=2");
```

## DNS

To be able to use the Domain Name System (DNS) to resolve names such as
example.org with the client API, import RIOT's DNS module (and a DNS implementation).

For example, to send a request to the @ref unicoap_destination_t
`unicoap_destination_uri("coap://example.org/foo/bar")`, import `dns` and, e.g., `sock_dns`.

```c
USEMODULE += dns
USEMODULE += sock_dns
```
