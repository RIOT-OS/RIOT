@defgroup net_unicoap_server_example Writing a Server Application
@ingroup  net_unicoap_server
@brief    Learn to write a simple server application with `unicoap`
@{

Sample code. Find the sample code in `examples/networking/coap/unicoap_server`.

In this tutorial, you will learn how you can create a simple CoAP server with `unicoap`.
We will support CoAP over UDP and DTLS and try out the application using RIOT's native board.
Hence, you will need a Linux host and support for tuntap (tap interfaces). Our goal is to create
a server that greets its users.

## Getting Started

To start, we will create a Makefile and add `unicoap` as a dependency. Because we want to support UDP
and DTLS, we need to add the respective drivers in the `Makefile`.

```makefile
USEMODULE += unicoap
USEMODULE += unicoap_driver_udp
USEMODULE += unicoap_driver_dtls
```

Because RIOT allows you to switch the network backend, we need to specify one.
In this tutorial, we choose @ref net_gnrc.

```makefile
# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
# Automatically initialize GNRC upon startup
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules
USEMODULE += gnrc_ipv6_default
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
```

In `main.c`, we include `unicoap`.

```c
#include "net/unicoap.h"
```

## Implementing a Simple CoAP resource

Let's define a CoAP resource. To define a new resource statically (at compile-time), we
use the @ref UNICOAP_RESOURCE macro. To make that work, we need to import the
@ref net_unicoap_resources_xfa module in our `Makefile`.

```makefile
USEMODULE += unicoap_resources_xfa
```

```c
UNICOAP_RESOURCE(hello) {
    // ...
};
```

Note that you will need to supply an identifier. Let's
call it `hello`. You can give it any name you want, as long as it is a unique C
variable name. Each resource must be assigned a path. This will be the part that follows the
host/domain and port in the CoAP URI. To assign a path, set the @ref unicoap_resource_t.path
property to a static null-terminated UTF-8 string literal that starts with a leading slash.
Since we want to define a root instance, we set it to `/`.

```c
UNICOAP_RESOURCE(hello) {
    .path = "/",
};
```

Next, we must specify what CoAP methods (akin to HTTP methods) we want to allow. Therefore, we list
all methods permitted as parameters to the @ref UNICOAP_METHODS macro. You cannot leave
@ref unicoap_resource_t.methods empty. If you later send a request that does not match the set of
methods, `unicoap` will reject that request with a `Method Not Allowed` CoAP response.

```c
UNICOAP_RESOURCE(hello) {
    .path = "/",
    .methods = UNICOAP_METHODS(UNICOAP_METHOD_GET, UNICOAP_METHOD_FETCH),
};
```

Optionally, you can also restrict the resource to a set of transports. This may be useful if you
consider encryption to be mandatory. In our case, the greeting will also be reachable over
unencrypted UDP.

```c
UNICOAP_RESOURCE(hello) {
    // ...
    .protocols = UNICOAP_PROTOCOLS(UNICOAP_PROTO_DTLS, UNICOAP_PROTO_UDP),
    // ...
};
```

Next, we want to make sure our greeting arrives a the client. Since we are using CoAP over UDP (or
CoAP over DTLS, which relies on UDP for that matter), we instruct `unicoap` to send confirmable
responses. `unicoap` will retransmit our response until the client has acknowledged it.
To do this, we pass the @ref UNICOAP_RESOURCE_FLAG_RELIABLE flag.


```c
UNICOAP_RESOURCE(hello) {
    .path = "/",
    .methods = UNICOAP_METHODS(UNICOAP_METHOD_GET, UNICOAP_METHOD_FETCH),
    .flags = UNICOAP_RESOURCE_FLAG_RELIABLE,
};
```

Finally, we specify a function that is going to handle requests to `/`. We're going to implement
`handle_hello_request` next.

```c
UNICOAP_RESOURCE(hello) {
    .path = "/",
    .methods = UNICOAP_METHODS(UNICOAP_METHOD_GET, UNICOAP_METHOD_FETCH),
    .flags = UNICOAP_RESOURCE_FLAG_RELIABLE,
    .handler = handle_hello_request
};
```

`handle_hello_request` will take four arguments: the request, auxiliary information like the client
address, a context and an [optional argument](@ref unicoap_resource_t.handler_arg).

```c
static int handle_hello_request(unicoap_message_t* message, const unicoap_aux_t* aux,
                                unicoap_request_context_t* ctx, void* arg) {

    // ...
}
```

Inside `handle_hello_request` we're going to log the request initially. We use
@ref unicoap_string_from_method to get a string representation of the CoAP method, i.e., the CoAP
message code, and @ref unicoap_message_payload_get_size for the number of payload bytes.
This will log a messaging like `GET /, 0 bytes`.

```c
printf(
    "app: %s /, %" PRIuSIZE " bytes\n",
    unicoap_string_from_method(message->method),
    unicoap_message_payload_get_size(message)
);
```

This version of the resource is going to be very simple. The root resource will simply respond
with a `Hello, World!` string. For static null-terminated strings, `unicoap` provides
convenience initializers. Note that we can reuse the memory of the `message` parameter.
Finally, we respond. You do not necessarily need to return the result of @ref unicoap_send_response.

```c
unicoap_response_init_string(message, UNICOAP_STATUS_CONTENT, "Hello, World!");
return unicoap_send_response(message, ctx);
```

## Creating a More Dynamic CoAP Resource

The resource we implemented in the previous section was quite simple. Surely, visitors would enjoy
a personalized greeting, wouldn't they? Let's develop a `/greeting` resource that accepts a `name`
query parameter with their name. So if someone asks for `coap://...host.../greeting?name=RIOTeer`,
we would respond with `Hello, RIOTeer! Welcome to our itsy bitsy tiny CoAP server!`.

The first step is to define a new resource.

```c
UNICOAP_RESOURCE(greeting) {
    .path = "/greeting",
    .flags = UNICOAP_RESOURCE_FLAG_RELIABLE,
    .methods = UNICOAP_METHODS(UNICOAP_METHOD_GET),
    .handler = handle_greeting_request,
};
```

In `handle_greeting_request`, we need to retrieve the `name` query parameter and return a
`Bad Request` response if our visitor did not leave their name in the request.
To do this, we use @ref unicoap_options_t::unicoap_options_get_first_uri_query_by_name.
As a simple shorthand you can return a CoAP status from the request handler as follows.
This will send a response without any payload attached.

```c
static int handle_greeting_request(unicoap_message_t* message, const unicoap_aux_t* aux,
                                   unicoap_request_context_t* ctx, void* arg) {
    (void)aux;
    (void)arg;

    ssize_t res = 0;
    const char* name = NULL;

    if ((res = unicoap_options_get_first_uri_query_by_name(message->options, "name", &name)) < 0) {
        printf("error: could not get 'name' query: %" PRIdSIZE " (%s)\n", res, strerror(-res));
        return UNICOAP_STATUS_BAD_REQUEST;
    }

    // ...
}
```

We don't really trust our visitor yet, so let's validate their name. Only this time, we tell the
client the query value was invalid by leveraging the @ref unicoap_send_response function and a
dedicated error message.

```c
// Validate any input. Here, we apply a 30 UTF-8 character limit.
if (res > 30 || !is_valid_name(name, res)) {
    unicoap_response_init_string(message, UNICOAP_STATUS_BAD_REQUEST, "invalid 'name' query");
    return unicoap_send_response(message, ctx);
}
```

Now we can craft our response. Proper responses have their `Content-Format` option set accordingly,
in our case `text/plain`. Setting options can fail, e.g., due to insufficient buffer capacity, hence
we do that first.
We allocate options through @ref UNICOAP_OPTIONS_ALLOC and set the format with
@ref unicoap_options_set_content_format.

@remark
Note that the buffer capacity we specify when allocating
should be an upper estimate. If you do know how CoAP options are represented in memory, you can
set the capacity to exactly the size in bytes (Which, in this case, would prevent us from adding
another option. But since we only need the `Content-Format` option, we set the capacity to exactly
2 bytes.).

```c
UNICOAP_OPTIONS_ALLOC(options, 2);

// Set Content-Format option to text/plain
if (unicoap_options_set_content_format(&options, UNICOAP_FORMAT_TEXT) < 0) {
    return UNICOAP_STATUS_INTERNAL_SERVER_ERROR;
}

message->options = &options;
```

The next step is to create the greeting string. In theory, you could write the three parts
`Hello, `, then the name, and finally the suffix `! Welcome ......` into a buffer and send that.
But that would be too easy. In some cases, when you would need to create a large buffer, the
following technique might be preferable. Instead of a buffer, we create a vector, that is, a list
of payload _chunks_. The trick is that the network backend will copy these chunks into a
transmission buffer on its own, which is why we can avoid additional copy operations this way.

These are going to be the first and last chunks:

```c
#define PREFIX "Hello, "
iolist_t list = {
    .iol_base = PREFIX,
    .iol_len = static_strlen(PREFIX),
};

#define SUFFIX "! Welcome to our itsy bitsy tiny CoAP server!"
iolist_t suffix = {
    .iol_base = SUFFIX,
    .iol_len = static_strlen(SUFFIX)
};
```

@note
`static_strlen` is defined in the sample code.

Next, we add the dynamic middle part.

```c
iolist_t name_chunk = {
    .iol_next = &suffix,
    .iol_base = (void*)name,
    .iol_len = res
};
list.iol_next = &name_chunk;
```

Through `.iol_next = &suffix`, we chain the suffix behind the middle part and using
`list.iol_next = &name_chunk` appends this chain of two chunks to the first chunk. Our vector is
done and we can finally send the response after having set the status and message payload.

```c
unicoap_message_payload_set_chunks(message, &list);
unicoap_response_set_status(message, UNICOAP_STATUS_CONTENT);

return unicoap_send_response(message, ctx);
```

## Supporting DTLS

To enable Datagram Transport Layer Security, we already imported the
@ref net_unicoap_drivers_dtls.
However, we need to include additional headers to add a DTLS credential to `unicoap`.

```c
#if IS_USED(MODULE_UNICOAP_DRIVER_DTLS)
# include "net/sock/dtls/creds.h"
# include "net/credman.h"
# include "net/dsm.h"
# include "unicoap_example_dtls.h"

# define EXAMPLE_DTLS_CREDENTIAL_TAG 42

static const uint8_t psk_id_0[] = PSK_DEFAULT_IDENTITY;
static const uint8_t psk_key_0[] = PSK_DEFAULT_KEY;
static const credman_credential_t credential = {
    .type = CREDMAN_TYPE_PSK,
    .tag = EXAMPLE_DTLS_CREDENTIAL_TAG,
    .params = {
        .psk = {
            .key = { .s = psk_key_0, .len = sizeof(psk_key_0) - 1, },
            .id = { .s = psk_id_0, .len = sizeof(psk_id_0) - 1, },
        }
    },
};
#endif
```

In the `main` function, we then need to add this credential to the DTLS socket which we can retrieve
using @ref unicoap_transport_dtls_get_socket.

```c
int main(void) {
#  if IS_USED(MODULE_UNICOAP_DRIVER_DTLS)
    int res = credman_add(&credential);
    if (res < 0 && res != CREDMAN_EXIST) {
        /* ignore duplicate credentials */
        printf("app: cannot add credential to system: %d\n", res);
        return 1;
    }
    sock_dtls_t* dtls_socket = unicoap_transport_dtls_get_socket();
    assert(dtls_socket);

    if ((res = sock_dtls_add_credential(dtls_socket, EXAMPLE_DTLS_CREDENTIAL_TAG)) < 0) {
        printf("app: cannot add credential to DTLS sock: %d\n", res);
        return 1;
    }
#  endif
}
```

Now, your server is also reachable over an encrypted DTLS connection. Time to test it!

## Testing the CoAP server

The sample code includes a `client.py` script that uses [aiocoap](https://aiocoap.readthedocs.io).
To use it we need to compile the server, run it and then send a CoAP request to it. We're going to
use RIOT's `native` board for this, i.e., both the client and server will run on your linux host
and there will be no IEEE 802.15.4 network involved — no antenna needed.
To compile and run the app, run this shell command:

```sh
cd RIOT/examples/networking/coap/unicoap_server
BOARD=native make -j flash term
```

This should log something similar to this:

```
RIOT/dist/tools/pyterm/pyterm
    -ps RIOT/examples/networking/coap/unicoap_server/bin/native64/unicoap_server.elf
    --process-args tap0
Welcome to pyterm!
Type '/exit' to exit.
# RIOT native interrupts/signals initialized.
# RIOT native64 board initialized.
# RIOT native hardware initialization complete.
# coap: registered 2 XFA resources
# coap.transport.udp: zero_copy_guarantees=1 creating UDP sock, port=5683 if=0 family=inet6
# coap.transport.dtls: creating DTLS sock, port=5684 if=0 family=inet6
# main(): This is RIOT! (Version: 2025.04-devel-634-RED-unicoap-02-server-minimal)
# app: listening at UDP <sock_tl_ep port=5683 netif=0 ipv6=::>
# app: listening at DTLS <sock_tl_ep port=5684 netif=0 ipv6=::>
# app: using credential: type=PSK id=Client_identity key=secretPSK
```

@remark
You might need to enable the tap interface:
```sh
sudo ip tuntap add tap0 mode tap user ${USER}
sudo ip link set tap0 up
```

@note
`unicoap` will emit debug logs depending on @ref CONFIG_UNICOAP_DEBUG_LOGGING and
@ref CONFIG_UNICOAP_ASSIST. These settings are enabled in the sample code in `app.config`.

In a second terminal session, run

```sh
python3 client.py -m GET -u "coap://[fe80::c0:ff:ee%tap0]/greeting?name=RIOTeer"
```

where `fe80::c0:ff:ee` is the IPv6 address of the RIOT interface behind the tap interface.
If you run the `examples/default` shell application and type `ifconfig` you should see a list of
interfaces and their addresses.

You should see a number of debug logs from that script culminating in:

```
response: 2.05 Content
b'Hello, RIOTeer! Welcome to our itsy bitsy tiny CoAP server!'
```

Congrats, your server is working!

If you want to capture the actual CoAP messages being sent, open a third terminal session
and run `tcpdump -i tap0 -w coap-greeting.pcap`, then execute the client script as described above,
then terminate `tcpdump` with `CTRL+C`. You can now open `coap-greeting.pcap` with
[Wireshark](https://wireshark.org) to inspect CoAP messages. You should see a `NON` request followed
by a `CON` response which elicits another `ACK` message from the client.

@}
