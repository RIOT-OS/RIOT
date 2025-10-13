---
title: CoAP Networking
description: Learn how to use CoAP with RIOT
code_folder: examples/guides/coap
---

CoAP (Constrained Application Protocol) is a protocol to implement REST APIs
for constrained devices and networks.
It is designed for IoT applications and is similar to HTTP but much lighter.

This guide walks you through creating a simple CoAP
application using RIOT's gcoap module. You will build both a server that responds
to requests and a client that sends requests, demonstrating basic CoAP communication.

:::note
This tutorial explains how to use `gcoap`, RIOT's CoAP implementation.
Note that we are currently developing `unicoap`, a new CoAP implementation
that will feature a more streamlined API and additional functionality.
:::

## Creating the CoAP Server

### Step 1: Project Structure

Create a new directory for your project:

```text
coap-hello-server
├── Makefile
└── main.c
```

### Step 2: Create the Makefile

For CoAP support we need to include the `gcoap` module.
For the IPv6 network stack we include the `gnrc_ipv6_default` module.
To initialize the network device we include the `netdev_default` module.
And to easily find our server from the client we set the static local
IPv6 address to `fe80::cafe:cafe:cafe:1`.

```makefile title=Makefile {9-22}
APPLICATION = coap_hello_server

# Board selection - change this to your target board
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../RIOT

# Include packages that pull up and auto-init the link layer
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif

# Network stack
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo

# Include gcoap module
USEMODULE += gcoap

# Static local IPv6 address
CFLAGS += -DCONFIG_GNRC_IPV6_STATIC_LLADDR='"fe80::cafe:cafe:cafe:1"'
CFLAGS += -DCONFIG_GNRC_IPV6_STATIC_LLADDR_IS_FIXED=1

include $(RIOTBASE)/Makefile.include
```

### Step 3: Implement the CoAP Server

```c title=main.c
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "net/gcoap.h"

/* Response message */
static const char RESPONSE_MSG[] = "World";

/* CoAP resource handler for /hello */
static ssize_t _hello_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                              coap_request_ctx_t *ctx)
{
    (void)ctx; /* unused parameter */

    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);

    /* Set content format to plain text */
    coap_opt_add_format(pdu, COAP_FORMAT_TEXT);

    /* Finalize options and get payload pointer */
    size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);

    /* Add the response message */
    if (pdu->payload_len >= sizeof(RESPONSE_MSG)) {
        memcpy(pdu->payload, RESPONSE_MSG, sizeof(RESPONSE_MSG) - 1);
        resp_len += sizeof(RESPONSE_MSG) - 1;
    }
    else {
        puts("gcoap: msg buffer too small");
        return gcoap_response(pdu, buf, len, COAP_CODE_INTERNAL_SERVER_ERROR);
    }

    return resp_len;
}

/* CoAP resources array */
static const coap_resource_t _resources[] = {
    { "/hello", COAP_GET, _hello_handler, NULL },
};

/* gcoap listener structure */
static gcoap_listener_t _listener = {
    &_resources[0],
    ARRAY_SIZE(_resources),
    GCOAP_SOCKET_TYPE_UDP,
    NULL,
    NULL,
    NULL
};

int main(void)
{
    /* Initialize CoAP server */
    gcoap_register_listener(&_listener);

    puts("CoAP server initialized");
    puts("Listening on /hello");

    return 0;
}
```

### Step 5: Build and Run the Server

Run the following command from the terminal:

```bash
make all term
```

Or run the following command to flash it to a board:

```bash
BOARD=arduino-feather-nrf52840-sense make all flash term
```

## Creating the CoAP Client

### Step 1: Project Structure

Create a new directory for your project:

```text
coap-hello-client
├── Makefile
└── main.c
```

### Step 2: Create the Makefile

```makefile title=Makefile {7-23}
APPLICATION = coap_hello_client

BOARD ?= native

RIOTBASE ?= $(CURDIR)/../RIOT

# Include packages that pull up and auto-init the link layer
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif

# Network stack
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo

# Include gcoap module
USEMODULE += gcoap

# Include shell for interactive commands
USEMODULE += shell
USEMODULE += shell_cmds_default

# CoAP Server Address
CFLAGS += -DSERVER_ADDRESS='"fe80::cafe:cafe:cafe:1"'

include $(RIOTBASE)/Makefile.include
```

### Step 3: Implement the CoAP Client

```c title=main.c
#include <stdio.h>
#include <string.h>
#include "net/gcoap.h"
#include "net/ipv6/addr.h"
#include "shell.h"
#include "msg.h"

#define MAIN_QUEUE_SIZE (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

/* Buffer for the request */
static uint8_t buf[CONFIG_GCOAP_PDU_BUF_SIZE];

/* Response handler callback */
static void _resp_handler(const gcoap_request_memo_t *memo, coap_pkt_t *pdu,
                          const sock_udp_ep_t *remote)
{
    (void)remote;

    if (memo->state == GCOAP_MEMO_TIMEOUT) {
        puts("Request timed out");
        return;
    }
    else if (memo->state == GCOAP_MEMO_RESP) {
        char *class_str = (coap_get_code_class(pdu) == COAP_CLASS_SUCCESS) ?
                              "Success" :
                              "Error";

        printf("Response: %s (Code: %u.%02u)\n", class_str,
               coap_get_code_class(pdu), coap_get_code_detail(pdu));

        /* Print payload if present */
        if (pdu->payload_len) {
            printf("Payload: %.*s\n", pdu->payload_len, (char *)pdu->payload);
        }
    }
}

/* Send CoAP GET request to /hello endpoint */
static int _send_coap_request(void)
{
    sock_udp_ep_t remote;
    coap_pkt_t pdu;
    size_t len;

    /* Parse IPv6 address */
    remote.family = AF_INET6;
    remote.netif = SOCK_ADDR_ANY_NETIF;
    remote.port = CONFIG_GCOAP_PORT;

    if (ipv6_addr_from_str((ipv6_addr_t *)&remote.addr.ipv6, SERVER_ADDRESS) ==
        NULL) {
        puts("Error: unable to parse destination address");
        return -1;
    }

    /* Initialize CoAP request */
    len = gcoap_req_init(&pdu, buf, CONFIG_GCOAP_PDU_BUF_SIZE, COAP_METHOD_GET,
                         "/hello");

    /* Set content format option */
    coap_opt_add_format(&pdu, COAP_FORMAT_TEXT);

    /* Finish options */
    len = coap_opt_finish(&pdu, COAP_OPT_FINISH_NONE);

    /* Send request */
    ssize_t res = gcoap_req_send(buf, len, &remote, NULL, _resp_handler, NULL,
                                 GCOAP_SOCKET_TYPE_UDP);

    if (res <= 0) {
        puts("Error sending request");
        return -1;
    }

    printf("Request sent (%d bytes)\n", (int)res);
    return 0;
}

/* Shell command to send CoAP request */
static int _shell_command_hello(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    return _send_coap_request();
}

SHELL_COMMAND(hello, "Send CoAP request to /hello endpoint",
              _shell_command_hello);

int main(void)
{
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("RIOT CoAP Hello Client");
    puts("Use 'hello' to send CoAP requests");

    char buffer[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, buffer, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
```

### Step 4: Build and Run the Client

Run the following command from the terminal:

```bash
make all term
```

Or run the following command to flash it to a board:

```bash
BOARD=arduino-feather-nrf52840-sense make all flash term
```

## Testing Both Together

### Using a Board

When flashing to a board you must first make sure that the network stack is correctly configured.
Then run the following shell command after flashing and connecting to the shell of the
CoAP client board:

```bash
coap_get fe80::cafe:cafe:cafe:1 /hello
```

You should see:

```text
Request sent (XX bytes)
Response: Success (Code: 2.05)
Payload: World
```

### Using Native (TAP) Network

#### Step 1: Create the TAP Bridge on the Linux Host

From the root of the RIOT source directory run the following command:

```shell
sudo ./dist/tools/tapsetup/tapsetup -c 2
```

#### Step 2: Flash and Run the Server

Run the following command to build, flash and run the CoAP server:

```bash
cd coap-hello-server
make BOARD=native PORT=tap0 all term
```

#### Step 3: Flash and Run the Client

Now run the following command in a separate window:

```bash
cd coap-hello-client
make BOARD=native PORT=tap1 all term
```

Now from the RIOT shell run the following command:

```bash
coap_get fe80::cafe:cafe:cafe:1 /hello
```

You should see:

```text
Request sent (XX bytes)
Response: Success (Code: 2.05)
Payload: World
```
