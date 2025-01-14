## About

This application provides command line access to gcoap, a high-level API for
CoAP messaging. See the [CoAP spec][1] for background, and the
Modules>Networking>CoAP topic in the source documentation for detailed usage
instructions and implementation notes.

We support two setup options for this example:

### Native networking

Build with the standard `Makefile`. Follow the setup [instructions][2] for
the gnrc_networking example.

### SLIP-based border router

Build with `Makefile.slip`. Follow the setup instructions in README-slip.md,
which are based on the [SLIP instructions][3] for the gnrc_border_router
example. We also plan to provide or reference the ethos/UHCP instructions,
but we don't have it working yet.


## Example Use

This example uses gcoap as a server on RIOT native. Then we send a request
from a libcoap example client on the Linux host.

### Verify setup from RIOT terminal

    > coap info

Expected response:

    CoAP server is listening on port 5683
     CLI requests sent: 0
    CoAP open requests: 0

### Query from libcoap example client

gcoap does not provide any output to the CoAP terminal when it handles a
request. We recommend use of Wireshark to see the request and response. You
also can add some debug output in the endpoint function callback.

    ./coap-client -N -m get -p 5683 coap://[fe80::1843:8eff:fe40:4eaa%tap0]/.well-known/core

Example response:

    v:1 t:NON c:GET i:0daa {} [ ]
    </cli/stats>

The response shows the endpoint registered by the gcoap CLI example.

### Send query to libcoap example server

Start the libcoap example server with the command below.

    ./coap-server

\
Below are some example queries to enter into the RIOT CLI. Ports can be omitted.
The port defaults to 5683 for the `gcoap` and to 5684 for the `gcoap_dtls` example.

- For IPv6 setup (network interface can be omitted)

    > coap get coap://[fe80::d8b8:65ff:feee:121b%6]:5683/.well-known/core

- For IPv4 setup

    > coap get coap://192.168.2.135:5683/.well-known/core

- When including the module `sock_dns` for domain resolution

    > coap get coap://example.com:5683/.well-known/core

CLI output:

    gcoap_cli: sending msg ID 743, 75 bytes
    > gcoap: response Success, code 2.05, 105 bytes
    </>;title="General Info";ct=0,</time>;if="clock";rt="Ticks";title="Internal Clock";ct=0;obs,</async>;ct=0


## Other available CoAP implementations and applications

RIOT also provides package imports and test applications for other CoAP
implementations:

* [Nanocoap](../nanocoap_server): a very lightweight CoAP server based on the
  [nanocoap library](https://github.com/kaspar030/sock/tree/master/nanocoap)
  implementation

* [Microcoap](../../tests/pkg/microcoap): another lightweight CoAP server based
  on the [microcoap library](https://github.com/1248/microcoap) implementation


[1]: https://tools.ietf.org/html/rfc7252    "CoAP spec"
[2]: https://github.com/RIOT-OS/RIOT/tree/master/examples/gnrc_networking    "instructions"
[3]: https://github.com/RIOT-OS/RIOT/tree/master/examples/gnrc_border_router    "SLIP instructions"
