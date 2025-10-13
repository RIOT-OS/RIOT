/*
 * SPDX-FileCopyrightText: 2025 Lasse Rosenow
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

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
