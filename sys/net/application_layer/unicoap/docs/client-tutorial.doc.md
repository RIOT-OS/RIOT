@defgroup net_unicoap_client_example Writing a Client Application
@ingroup  net_unicoap_client
@brief    Learn to write a simple client application with `unicoap`

Sample code. Find the sample code in `examples/networking/coap/unicoap_client`.

In this tutorial, you will learn how you can create a simple CoAP client with `unicoap`.
We will support CoAP over UDP and DTLS and try out the application using RIOT's native board.
Hence, you will need a Linux host and support for tuntap (tap interfaces).

## Getting Started

To start, we will create a Makefile and add `unicoap` as a dependency. Because we want to support UDP
and DTLS, we need to add the respective drivers in the `Makefile`. We will build a small shell
application that accepts a URI and a request payload.

```makefile
USEMODULE += unicoap
USEMODULE += unicoap_client
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

## Setting up the shell

Because we're using the shell to accept a URI, we need to import the shell:

```Makefile
USEMODULE += shell
```

```c
#include "shell.h"
```


At the bottom of `main()`, let's initialize the shell. `shell_run` will run in a loop, accepting
commands.

```c
char line_buf[SHELL_DEFAULT_BUFSIZE];
shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
return 0;
```

We will define a custom command called `unicoap` (You can import support for more,
predefined shell commands, e.g., to see a list of network interfaces, specify
`USEMODULE += shell_cmds_default` in your Makefile, and type `ifconfig` in the shell).
Place this static definition for the command at the top-level (outside any functions) in your
`main.c`.

```c
SHELL_COMMAND(unicoap, "unicoap sample client", _cli);
```

The `unicoap` shell command and its arguments will be handled by a function called `_cli`, but
you can choose any name you would like. `_cli` is very similar to the way `main()` handles
command-line arguments: it accepts an `argc` function parameter (number of whitespace-separated
command-line arguments) and an `argv` function parameter (pointer to an array of strings).
For brevity, we will omit all the sanity checks on arguments, but you can look up the full
implementation in `examples/networking/coap/unicoap_client`.

```c
static int _cli(int argc, char** argv) {}
```

## Creating a custom command-line interface

Let's make our `unicoap` shell command accept three positional arguments. Followed by the request
method and URI, we want to optionally accept a payload string for the request. Trivially, this
only makes sense for, e.g., the `PUT` and `POST` methods, as `GET` request do not have a payload.

```
unicoap <method> <uri> [<payload>]
```

First, we ned to translate the method string to `unicoap`'s @ref unicoap_method_t type.
Note that `argv[0]` will be the name of the command itself, i.e., the first overall command-line
argument. Hence, the `method` argument will be at `argv[1]`.

```c
unicoap_method_t method;
if (strcmp(argv[1], "get") == 0) {
    method = UNICOAP_METHOD_GET;
} else if (strcmp(argv[1], "post") == 0) {
    method = UNICOAP_METHOD_POST;
} else if (strcmp(argv[1], "put") == 0) {
    method = UNICOAP_METHOD_PUT;
} else {
    printf("error: unsupported method '%s'\n", argv[1]);
    return -1;
}
```

Next, we will extract the URI and payload.

```c
const char* uri = argv[2];
const char* payload = argc > 3 ? argv[3] : NULL;

if (payload && method == UNICOAP_METHOD_GET) {
    printf("error: GET request has payload?\n");
    return -1;
}
```

## Preparing a request

Let's prepare a request.

```c
unicoap_message_t request;
unicoap_request_init_string(&request, method, payload);
```

We will specify to which server to send the request by passing a Uniform Resource Identifier (URI).
To make that work, we need to import the @ref net_unicoap_client_uri module in our `Makefile`.

```makefile
USEMODULE += unicoap_client_uri
```

`unicoap` supports multiple ways of addressing a server resource. You can create a
`unicoap_endpoint_t`, manually set the IP address, or even resolve a Fully Qualified Domain Name
(FQDN) using the Domain Name System (DNS), but we want users of our shell command to be able
to type a string like they would in a web browser starting with the URI scheme `coap://`.
The URI way of addressing a resource also saves us the trouble of having to add the URI path
manually as CoAP options to the message, e.g., using
@ref unicoap_options_t::unicoap_options_add_uri_path.

As we will only be sending requests through a tap interface to the Python server script
running on your linux host, we will use an IP address. If you want to use a domain name instead
in the URI, please refer to the instructions in @ref net_unicoap_client_uri.

```c
unicoap_destination_t destination = unicoap_destination_uri_string(uri);
```

Now we can send the request. There are multiple versions `unicoap` provides --- we choose
the asynchronous version that calls a handler function on the internal processing thread
when the response has arrived or the request fails.

```c
if ((res = unicoap_send_request_async(&request, &destination, _on_response, NULL, 0)) < 0) {
    printf("sending request failed (error %i, %s)\n", res, strerror(-res));
    return res;
}
```

## Handling responses

Finally, we specify a function that is going to handle response. We're going to implement
`_on_response` next.
`_on_response` will take three arguments: the response, auxiliary information like the server
address, and an error argument. Check if it is nonzero and bail out if so.

```c
static int _on_response(
    unicoap_message_t* message, 
    const unicoap_aux_t* aux,
    int error
) {
    /* Check if success or failure. Only proceed if no error occurred. */
    if (error) {
        printf("error: request failed (%i, %s)\n", error, strerror(-error));
        return error;
    }
    /* ... */
}
```

Inside `_on_response` we're going to log the request initially. We use
@ref unicoap_print_code to get a string representation of the CoAP status, i.e., the CoAP
message code, and @ref unicoap_message_t::unicoap_message_payload_get_size
for the number of payload bytes.
This will log a string like `response 2.05 Content (42 bytes)`.

```c
/* Print status code in c.dd (class.detail) form. */
printf("response ");
unicoap_print_code(response->code);

/* Print length of response payload in bytes. */
printf(" %s (%" PRIuSIZE " bytes)\n",
    unicoap_string_from_status(unicoap_response_get_status(response)),
    unicoap_message_payload_get_size(response));
```

Next, we check if the response format is `text/plain` and log the response as text.
Otherwise, we dump the response payload as hex. To do that, include the object dump
header and import the `od` module.

```Makefile
USEMODULE += od
```

Top of C file:

```c
#include "od.h"
```

We also need to check if the payload in the response message is contiguous, i.e.,
is not chunked into individual vector elements. Normally, you can expect responses
to always be contiguous, but you **must** check this.

```c
unicoap_content_format_t format;
if ((error = unicoap_options_get_content_format(response->options, &format)) >= 0
     && unicoap_content_format_is_human_readable(format)) {
    printf("text response: '%.*s'\n", 
        (int)unicoap_message_payload_get_size(response), 
        (char*)unicoap_message_payload_get(response));
}
else if (IS_USED(MODULE_OD)
         && response->payload_representation == UNICOAP_PAYLOAD_CONTIGUOUS
         && unicoap_message_payload_get_size(response) > 0
) {
    od_hex_dump(
            unicoap_message_payload_get(response), 
            unicoap_message_payload_get_size(response), 
            16);
}
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

Time to test it!

## Testing the CoAP client

The sample code includes a `server.py` script that uses [aiocoap](https://aiocoap.readthedocs.io).
To use it we need to compile the client application, run it and then send a CoAP request.
We're going to use RIOT's `native` board for this, i.e., both the client and server will run on your
linux host and there will be no wireless network involved — no antenna needed.
To compile and run the app, run this shell command:

```sh
cd RIOT/examples/networking/coap/unicoap_client
BOARD=native make -j flash term
```

This should show something similar to this:

```
RIOT/dist/tools/pyterm/pyterm
    -ps RIOT/examples/networking/coap/unicoap_client/bin/native64/unicoap_client.elf
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

@note `unicoap` will emit debug logs depending on @ref CONFIG_UNICOAP_DEBUG_LOGGING and
@ref CONFIG_UNICOAP_ASSIST. These settings are enabled in the sample code in `app.config`.

In a second terminal session, run

```sh
AIOCOAP_DTLSSERVER_ENABLED=1 python3 server.py --interface tap0
```

You should see output like this:

```
python3 server.py --interface tap0
server bound to tap interface fe80::dead:beef%tap0
using port=5683
```

And then type in the RIOT shell (the running example application)

```
unicoap get coap://[fe80::dead:beef]
```

```
response 2.05 Content (26 bytes)
text response: 'Welcome to the demo server'
```

Congrats, the client shell is working!

If you want to capture the actual CoAP messages being sent, open a third terminal session
and run `tcpdump -i tap0 -w coap-welcome.pcap`, then execute the client script as described above,
then terminate `tcpdump` with `CTRL+C`. You can now open `coap-greeting.pcap` with
[Wireshark](https://wireshark.org) to inspect CoAP messages. You should see a `NON` request followed
by a `CON` response which elicits another `ACK` message from the client.
