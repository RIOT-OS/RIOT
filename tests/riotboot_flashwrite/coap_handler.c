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
#include "riotboot/flashwrite.h"

static riotboot_flashwrite_t _writer;

ssize_t _flashwrite_handler(coap_pkt_t* pkt, uint8_t *buf, size_t len, void *context)
{
    riotboot_flashwrite_t *writer = context;

    uint32_t result = COAP_CODE_204;

    coap_block1_t block1;
    int blockwise = coap_get_block1(pkt, &block1);

    printf("_flashwrite_handler(): received data: offset=%u len=%u blockwise=%i more=%i\n", \
            (unsigned)block1.offset, pkt->payload_len, blockwise, block1.more);

    uint8_t *payload_start = pkt->payload;
    size_t payload_len = pkt->payload_len;
    size_t offset;
    if (block1.offset == 0) {
        printf("_flashwrite_handler(): init len=%u\n", pkt->payload_len);
        riotboot_flashwrite_init(writer, riotboot_slot_other());
    }

    /* skip first RIOTBOOT_FLASHWRITE_SKIPLEN bytes, but handle the case where
     * payload_len is smaller than RIOTBOOT_FLASHWRITE_SKIPLEN
     */
    if (block1.offset <= RIOTBOOT_FLASHWRITE_SKIPLEN) {
        size_t skip = RIOTBOOT_FLASHWRITE_SKIPLEN - block1.offset;
        skip = (payload_len > skip) ? skip : payload_len;
        payload_start += skip;
        payload_len -= skip;
        offset = block1.offset + skip;
    }
    else {
        offset = block1.offset;
    }

    if (offset == writer->offset) {
        riotboot_flashwrite_putbytes(writer, payload_start, payload_len, block1.more);
    }
    else {
        printf("_flashwrite_handler(): skipping invalid offset (data=%u, writer=%u)\n", (unsigned)offset, (unsigned)writer->offset);
    }

    if (block1.more == 1) {
        result = COAP_CODE_CONTINUE;
    }

    if (!blockwise || !block1.more) {
        puts("_flashwrite_handler(): finish");
        riotboot_flashwrite_finish(writer);
    }

    ssize_t reply_len = coap_build_reply(pkt, result, buf, len, 0);
    uint8_t *pkt_pos = (uint8_t*)pkt->hdr + reply_len;
    pkt_pos += coap_put_block1_ok(pkt_pos, &block1, 0);

    return pkt_pos - (uint8_t*)pkt->hdr;
}

/* must be sorted by path (ASCII order) */
const coap_resource_t coap_resources[] = {
    COAP_WELL_KNOWN_CORE_DEFAULT_HANDLER,
    { "/flashwrite", COAP_POST, _flashwrite_handler, &_writer },
};

const unsigned coap_resources_numof = ARRAY_SIZE(coap_resources);
