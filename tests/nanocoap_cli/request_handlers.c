/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       request handlers for test server
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ken Bannister <kb2ma@runbox.com>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "fmt.h"
#include "net/nanocoap.h"

#define _MAX_PAYLOAD_LEN  (16)

/* internal value that can be read/written via CoAP */
static uint8_t internal_value = 0;

static ssize_t _value_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, void *context)
{
    (void) context;

    ssize_t p = 0;
    char rsp[16];
    unsigned code;

    /* read coap method type in packet */
    unsigned method_flag = coap_method2flag(coap_get_code_detail(pkt));

    switch(method_flag) {
    case COAP_GET:
        /* write the response buffer with the internal value */
        p += fmt_u32_dec(rsp, internal_value);
        code = COAP_CODE_205;
        break;
    case COAP_PUT:
    case COAP_POST:
    {
        /* convert the payload to an integer and update the internal value */
        if (pkt->payload_len <= _MAX_PAYLOAD_LEN) {
            char payload[_MAX_PAYLOAD_LEN+1] = { 0 };
            memcpy(payload, (char*)pkt->payload, pkt->payload_len);
            internal_value = strtol(payload, NULL, 10);
        }
        else {
            internal_value = 0;
        }
        code = COAP_CODE_CHANGED;
        break;
    }
    default:
        code = COAP_CODE_METHOD_NOT_ALLOWED;
    }

    return coap_reply_simple(pkt, code, buf, len,
            COAP_FORMAT_TEXT, (uint8_t*)rsp, p);
}

/* must be sorted by path (ASCII order) */
const coap_resource_t coap_resources[] = {
    COAP_WELL_KNOWN_CORE_DEFAULT_HANDLER,
    { "/value", COAP_GET | COAP_PUT | COAP_POST, _value_handler, NULL },
};

const unsigned coap_resources_numof = ARRAY_SIZE(coap_resources);
