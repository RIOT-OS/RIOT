/*
 * Copyright (c) 2023 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       nanocbor helper for CoAP block requests
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "checksum/fletcher32.h"
#include "nanocbor/nanocbor.h"
#include "net/nanocoap.h"
#include "net/nanocoap/nanocbor_helper.h"

static bool _nanocbor_fits_block(nanocbor_encoder_t *enc, void *ctx, size_t len)
{
    (void)enc;
    (void)ctx;
    (void)len;
    return true; /* Always more space on the block */
}

static bool _nanocbor_fits_simple(nanocbor_encoder_t *enc, void *ctx, size_t len)
{
    (void)enc;
    const coap_nanocbor_slicer_helper_t *helper = ctx;
    return len + helper->payload_len <= helper->buf_len;
}

static inline bool _is_odd(size_t len)
{
    return len & 1;
}

static void _update_fletcher(coap_nanocbor_slicer_helper_t *helper, const uint8_t *buf, size_t len)
{
    if (_is_odd(helper->full_len)) {
        uint16_t tmp = (helper->fletcher_tmp << 8) | buf[0];
        fletcher32_update(&helper->fletcher_ctx, &tmp, 1);
        len--;
        buf++;
        helper->full_len++;
    }
    fletcher32_update(&helper->fletcher_ctx, buf, len / 2);
}

static void _nanocbor_append_blockwise(nanocbor_encoder_t *enc, void *ctx, const uint8_t *buf, size_t len)
{
    (void)enc;
    coap_nanocbor_slicer_helper_t *helper = ctx;
    if (len == 0) {
        return;
    }

    helper->payload_len +=
        coap_blockwise_put_bytes(helper->slicer,
                                 helper->buf + helper->payload_len,
                                 buf, len);

    _update_fletcher(helper, buf, len);

    helper->full_len += len;
    if (_is_odd(helper->full_len)) {
        /* store the last byte for the next round */
        helper->fletcher_tmp = buf[len - 1];
    }
}

static void _nanocbor_append_simple(nanocbor_encoder_t *enc, void *ctx, const uint8_t *buf, size_t len)
{
    (void)enc;
    coap_nanocbor_slicer_helper_t *helper = ctx;
    if (len == 0) {
        return;
    }

    _update_fletcher(helper, buf, len);

    /* This should have been checked before */
    assert(len + helper->payload_len <= helper->buf_len);

    memcpy(helper->buf + helper->payload_len, buf, len);
    helper->payload_len += len;
}

void coap_nanocbor_slicer_helper_init(coap_nanocbor_slicer_helper_t *helper, const coap_pkt_t *pkt,
                                      coap_block_slicer_t *slicer)
{
    memset(helper, 0, sizeof(coap_nanocbor_slicer_helper_t));
    if (slicer) {
        helper->slicer = slicer;
    }
    else {
        helper->buf_len = pkt->payload_len;
    }
    helper->buf = pkt->payload;
}

void coap_nanocbor_encoder_init(nanocbor_encoder_t *encoder,
                                coap_nanocbor_slicer_helper_t *helper)
{
    if (helper->slicer) {
        nanocbor_encoder_stream_init(encoder, helper, _nanocbor_append_blockwise, _nanocbor_fits_block);
    }
    else {
        nanocbor_encoder_stream_init(encoder, helper, _nanocbor_append_simple, _nanocbor_fits_simple);
    }
}

static ssize_t _etag_finish(coap_pkt_t *pdu, coap_nanocbor_slicer_helper_t *helper)
{
    /* Pad with an extra 0xff, which should not occur on its own at the end of a cbor stream */
    if (_is_odd(helper->full_len)) {
        uint8_t extra = 0xFF;
        _update_fletcher(helper, &extra, sizeof(extra));
    }
    /* And add the total length for good measure */
    _update_fletcher(helper, (uint8_t*)&helper->full_len, sizeof(helper->full_len));

    uint32_t etag = fletcher32_finish(&helper->fletcher_ctx);
    coap_opt_replace_etag(pdu, &etag, sizeof(etag));
    size_t header_options_len = pdu->payload - (uint8_t *)pdu->hdr;
    return header_options_len + helper->payload_len;
}

ssize_t coap_nanocbor_block2_finish(coap_pkt_t *pdu, coap_nanocbor_slicer_helper_t *helper)
{
    coap_block2_finish(helper->slicer);
    return _etag_finish(pdu, helper);
}

ssize_t coap_nanocbor_block1_finish(coap_pkt_t *pdu, coap_nanocbor_slicer_helper_t *helper)
{
    coap_block1_finish(helper->slicer);
    return _etag_finish(pdu, helper);
}

ssize_t coap_nanocbor_finish(coap_pkt_t *pdu, coap_nanocbor_slicer_helper_t *helper)
{
    return _etag_finish(pdu, helper);
}
