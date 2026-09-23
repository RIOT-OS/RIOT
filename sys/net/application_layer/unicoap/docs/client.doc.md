@defgroup net_unicoap_client CoAP Client
@ingroup net_unicoap
@brief Send CoAP requests synchronously or asynchronously

Module. Specify `USEMODULE += unicoap_client` in your application's Makefile.

## Constructing a request

Begin by constructing a message to send to the server.

```c
unicoap_message_t request;
unicoap_request_init_empty(&request, UNICOAP_METHOD_GET);
```

`unicoap` supports multiple ways of addressing a server. @ref unicoap_destination_t supports
Uniform Resource Identifiers (URIs) and sock endpoints. To use URI and DNS functionality in
`unicoap`, import @ref net_unicoap_client_uri.

```c
/* Set destination to URI passed over CLI. */
unicoap_destination_t destination =
    unicoap_destination_uri_string("coap://example.arpa/foo/bar?flag=1&x=X");

unicoap_endpoint_t endpoint = {
    .proto = UNICOAP_PROTO_UDP,
    .udp_ep = {
        .family = AF_INET6,
        .port = UNICOAP_DEFAULT_COAP_PORT,
        .addr.ipv6 = { 0xfe, 0x80, /* ... */ }
    }
};
unicoap_destination_t destination =
    unicoap_destination_endpoint(&endpoint);
```

@attention If you are using @ref net_unicoap_drivers_dtls and are sending a request over DTLS
(`coaps://`): Due to the current DTLS implementation, @ref pkg_tinydtls, `unicoap` must
set a network interface (netif) identifier to make sure the DTLS session handshake works.
If your RIOT instance has only a single netif (you can check this by running `ifconfig` in
the `examples/basic/default` shell), `unicoap` will experimentally assume this interface.
**If there're two or more interfaces, you MUST provide a netif, either as a zone identifier in
the URI (e.g., `%7` if 7 is the netif id), or by setting
@ref unicoap_endpoint_t `.udp_ep.netif` if you are using
@ref unicoap_destination_t::unicoap_destination_endpoint.

## Sending requests and handling responses

You will also need to define a response handler.

```c
static int my_response_handler(
    const unicoap_message_t* response,
    const unicoap_aux_t* aux, int error, void* arg
) {
    /* Check if success or failure. Only proceed if no error occurred. */
    if (error) {
        return error;
    }

    /* Handle response */

    return 0;
}
```

Then, send a request. You can choose between several versions:

- @ref unicoap_send_request_async Sends a request, and asynchronously calls callback on processing
  thread.

- @ref unicoap_send_request_sync Sends a request, blocks calling thread, calls callback
  synchronously on calling thread.

- @ref unicoap_send_request_sync_copy Sends a request, blocks calling thread, and copies response
  into provided buffer. Use this version with care as it incurs a significant copying overhead.

You should generally aim to use @ref unicoap_send_request_async in most instances, and only
fall back to @ref unicoap_send_request_sync if your application is really unable to make any
progress until the response has come in (or an error occured). Both callback-based versions
also call the callback if the request fails. Use the `aux` parameter to obtain additional
info about endpoints and messaging-layer properties (e.g., token and RFC 7252 message ID).

```c
if ((res = unicoap_send_request_async(
        &request, &destination, my_response_handler,
        /* params: */ NULL,
        /* flags: */ UNICOAP_REQUEST_FLAG_RELIABLE
)) < 0) {
    return res;
}
```

## Customizing request behavior

The client API provides two ways of customizing timing, reliability, etc.
Use the flags parameter to toggle boolean switches: At the moment,
only @ref UNICOAP_REQUEST_FLAG_RELIABLE is supported, which instructs the messaging layer
to turn its reliable transmission mode while sending the request. If the transport layer is
already inherently reliably, this flag has no effect; but over UDP/DTLS, the RFC 7252 messaging
implementation will use a confirmable (`CON`) message rather than a non-confirmable (`NON`).

@ref unicoap_request_parameters_t are optional parameters.
Set @ref unicoap_request_parameters_t::timeout_ms to set a custom timeout for the request
that differs from @ref CONFIG_UNICOAP_TIMEOUT_CLIENT_RESPONSE_MS. You can also pass a custom
argument to the response handler using @ref unicoap_request_parameters_t::callback_arg
to provide additional context, e.g., when you use the response handler across multiple requests.

Requests will be sent from the same default port the server uses (@ref CONFIG_UNICOAP_UDP_PORT
and @ref CONFIG_UNICOAP_DTLS_PORT). Client and server can be used simultaneously, and will by
default use the same port. In the future, we add the ability to specify the local endpoint
the request will be sent from. If you do have such a use case, please
[file an issue](https://github.com/RIOT-OS/RIOT/issues).

## Cancelling a request

You can cancel pending requests. Refer to @ref net_unicoap_client_cancellation.
