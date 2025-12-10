/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdlib.h>
#include <stdio.h>

#include "net/nanocoap.h"
#include "riotboot/flashwrite.h"

static riotboot_flashwrite_t _writer;

ssize_t _flashwrite_handler(coap_pkt_t* pkt, uint8_t *buf, size_t len, coap_request_ctx_t *ctx)
{
    riotboot_flashwrite_t *writer = coap_request_ctx_get_context(ctx);

    uint32_t result = COAP_CODE_204;

    coap_block1_t block1;
    int blockwise = coap_get_block1(pkt, &block1);

    printf("_flashwrite_handler(): received data: offset=%" PRIuSIZE
           " len=%u blockwise=%i more=%i\n",
           block1.offset, pkt->payload_len, blockwise, block1.more);

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
        printf("_flashwrite_handler(): skipping invalid offset (data=%" PRIuSIZE
               ", writer=%" PRIuSIZE ")\n", offset, writer->offset);
    }

    if (block1.more == 1) {
        result = COAP_CODE_CONTINUE;
    }

    if (!blockwise || !block1.more) {
        puts("_flashwrite_handler(): finish");
        riotboot_flashwrite_finish(writer);
    }

    ssize_t reply_len = coap_build_reply(pkt, result, buf, len, 0);
    if (reply_len <= 0) {
        return reply_len;
    }

    uint8_t *pkt_pos = pkt->buf + reply_len;
    pkt_pos += coap_put_block1_ok(pkt_pos, &block1, 0);

    return pkt_pos - pkt->buf;
}

NANOCOAP_RESOURCE(flashwrite) {
    .path = "/flashwrite",
    .methods = COAP_POST,
    .handler = _flashwrite_handler,
    .context = &_writer
};
