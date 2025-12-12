/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "buildinfo/board.h"
#include "kernel_defines.h"
#include "log.h"
#include "net/nanocoap.h"
#include "suit.h"
#include "suit/transport/coap.h"

#ifdef MODULE_RIOTBOOT_SLOT
#include "riotboot/slot.h"
#endif

static ssize_t _riot_board_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len,
                                   coap_request_ctx_t *context)
{
    (void)context;
    return coap_reply_simple(pkt, COAP_CODE_205, buf, len,
            COAP_FORMAT_TEXT, (uint8_t*)RIOT_BOARD, strlen(RIOT_BOARD));
}

static ssize_t _version_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len,
                                coap_request_ctx_t *context)
{
    (void)context;
    return coap_reply_simple(pkt, COAP_CODE_205, buf, len,
                             COAP_FORMAT_TEXT, (uint8_t *)"NONE", 4);
}

static ssize_t _trigger_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len,
                                coap_request_ctx_t *context)
{
    (void)context;
    unsigned code;
    size_t payload_len = pkt->payload_len;
    if (payload_len) {
        if (payload_len >= CONFIG_SOCK_URLPATH_MAXLEN) {
            code = COAP_CODE_REQUEST_ENTITY_TOO_LARGE;
        }
        else {
            code = COAP_CODE_CREATED;
            LOG_INFO("suit: received URL: \"%s\"\n", (char *)pkt->payload);
            suit_worker_trigger((char *)pkt->payload, strlen((char *)pkt->payload));
        }
    }
    else {
        code = COAP_CODE_REQUEST_ENTITY_INCOMPLETE;
    }

    return coap_reply_simple(pkt, code, buf, len,
                             COAP_FORMAT_NONE, NULL, 0);
}

#ifdef MODULE_RIOTBOOT_SLOT
static ssize_t _slot_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len,
                             coap_request_ctx_t *context)
{
    /* context is passed either as NULL or 0x1 for /active or /inactive */
    char c = '0';

    if (coap_request_ctx_get_context(context)) {
        c += riotboot_slot_other();
    }
    else {
        c += riotboot_slot_current();
    }

    return coap_reply_simple(pkt, COAP_CODE_205, buf, len,
                             COAP_FORMAT_TEXT, (uint8_t *)&c, 1);
}
#endif

#ifdef MODULE_RIOTBOOT_SLOT
NANOCOAP_RESOURCE(slot_active) {
    .path = "/suit/slot/active", .methods = COAP_METHOD_GET, .handler = _slot_handler,
};
NANOCOAP_RESOURCE(slot_inactive) {
   .path = "/suit/slot/inactive", .methods = COAP_METHOD_GET, .handler = _slot_handler, .context = (void *)0x1,
};
#endif
NANOCOAP_RESOURCE(trigger) {
    .path = "/suit/trigger", .methods = COAP_METHOD_PUT | COAP_METHOD_POST, .handler = _trigger_handler,
};
NANOCOAP_RESOURCE(version) {
    .path = "/suit/version", .methods = COAP_METHOD_GET, .handler = _version_handler,
};
NANOCOAP_RESOURCE(board) {
    .path = "/riot/board", .methods = COAP_GET, .handler = _riot_board_handler,
};
