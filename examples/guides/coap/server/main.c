/*
 * SPDX-FileCopyrightText: 2025 Lasse Rosenow
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

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
