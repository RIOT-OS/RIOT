/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "net/nanocoap.h"
#include "suit/transport/coap.h"
#include "kernel_defines.h"

static ssize_t _riot_board_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len,
                                   coap_request_ctx_t *context)
{
    (void)context;
    return coap_reply_simple(pkt, COAP_CODE_205, buf, len,
            COAP_FORMAT_TEXT, (uint8_t*)RIOT_BOARD, strlen(RIOT_BOARD));
}

NANOCOAP_RESOURCE(board) {
    .path = "/riot/board", .methods = COAP_GET, .handler = _riot_board_handler,
};

NANOCOAP_RESOURCE(suit) SUIT_COAP_SUBTREE;
