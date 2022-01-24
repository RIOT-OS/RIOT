/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "fmt.h"
#include "net/nanocoap.h"
#include "hashes/sha256.h"
#include "kernel_defines.h"

/* internal value that can be read/written via CoAP */
static uint8_t internal_value = 0;

static const uint8_t block2_intro[] = "This is RIOT (Version: ";
static const uint8_t block2_board[] = " running on a ";
static const uint8_t block2_mcu[] = " board with a ";

static ssize_t _echo_handler(coap_pkt_t *pkt, coap_build_pkt_t *response, void *context)
{
    (void)context;
    char uri[CONFIG_NANOCOAP_URI_MAX];

    if (coap_get_uri_path(pkt, (uint8_t *)uri) <= 0) {
        return coap_reply_simple(pkt, COAP_CODE_INTERNAL_SERVER_ERROR, response,
                                 COAP_FORMAT_TEXT, NULL, 0);
    }
    char *sub_uri = uri + strlen("/echo/");
    size_t sub_uri_len = strlen(sub_uri);
    return coap_reply_simple(pkt, COAP_CODE_CONTENT, response, COAP_FORMAT_TEXT,
                             (uint8_t *)sub_uri, sub_uri_len);
}

static ssize_t _riot_board_handler(coap_pkt_t *pkt, coap_build_pkt_t *response, void *context)
{
    (void)context;
    return coap_reply_simple(pkt, COAP_CODE_205, response, COAP_FORMAT_TEXT,
                             RIOT_BOARD, strlen(RIOT_BOARD));
}

static ssize_t _riot_block2_handler(coap_pkt_t *pkt, coap_build_pkt_t *response, void *context)
{
    (void)context;
    coap_block_slicer_t slicer;
    coap_block2_init(pkt, &slicer);

    response->cur += coap_put_option_ct(response->cur, 0, COAP_FORMAT_TEXT);
    response->cur += coap_opt_put_block2(response->cur, COAP_OPT_CONTENT_FORMAT, &slicer, 1);
    *response->cur++ = 0xff;

    /* Add actual content */
    coap_blockwise_put_bytes(&slicer, response, block2_intro, sizeof(block2_intro)-1);
    coap_blockwise_put_bytes(&slicer, response, (uint8_t*)RIOT_VERSION, strlen(RIOT_VERSION));
    coap_blockwise_put_char(&slicer, response, ')');
    coap_blockwise_put_bytes(&slicer, response, block2_board, sizeof(block2_board)-1);
    coap_blockwise_put_bytes(&slicer, response, (uint8_t*)RIOT_BOARD, strlen(RIOT_BOARD));
    coap_blockwise_put_bytes(&slicer, response, block2_mcu, sizeof(block2_mcu)-1);
    coap_blockwise_put_bytes(&slicer, response, (uint8_t*)RIOT_MCU, strlen(RIOT_MCU));
    /* To demonstrate individual chars */
    coap_blockwise_put_char(&slicer, response, ' ');
    coap_blockwise_put_char(&slicer, response, 'M');
    coap_blockwise_put_char(&slicer, response, 'C');
    coap_blockwise_put_char(&slicer, response, 'U');
    coap_blockwise_put_char(&slicer, response, '.');

    return coap_block2_build_reply(pkt, COAP_CODE_205, response, &slicer);
}

static ssize_t _riot_value_handler(coap_pkt_t *pkt, coap_build_pkt_t *response, void *context)
{
    (void) context;

    ssize_t p = 0;
    char rsp[16];
    unsigned code = COAP_CODE_EMPTY;

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
        if (pkt->payload_len < 16) {
            /* convert the payload to an integer and update the internal value */
            char payload[16] = { 0 };
            memcpy(payload, (char*)pkt->payload, pkt->payload_len);
            internal_value = strtol(payload, NULL, 10);
            code = COAP_CODE_CHANGED;
        }
        else {
            code = COAP_CODE_REQUEST_ENTITY_TOO_LARGE;
        }
    }

    return coap_reply_simple(pkt, code, response, COAP_FORMAT_TEXT, (uint8_t*)rsp, p);
}

ssize_t _sha256_handler(coap_pkt_t* pkt, coap_build_pkt_t *response, void *context)
{
    (void)context;

    /* using a shared sha256 context *will* break if two requests are handled
     * at the same time.  doing it anyways, as this is meant to showcase block1
     * support, not proper synchronisation. */
    static sha256_context_t sha256;

    uint8_t digest[SHA256_DIGEST_LENGTH];

    uint32_t result = COAP_CODE_204;

    coap_block1_t block1;
    int blockwise = coap_get_block1(pkt, &block1);

    printf("_sha256_handler(): received data: offset=%u len=%u blockwise=%i more=%i\n", \
            (unsigned)block1.offset, pkt->payload_len, blockwise, block1.more);

    if (block1.offset == 0) {
        puts("_sha256_handler(): init");
        sha256_init(&sha256);
    }

    sha256_update(&sha256, pkt->payload, pkt->payload_len);

    if (block1.more == 1) {
        result = COAP_CODE_CONTINUE;
    }

    size_t result_len = 0;
    if (!blockwise || !block1.more) {
        puts("_sha256_handler(): finish");
        sha256_final(&sha256, digest);
        result_len = SHA256_DIGEST_LENGTH * 2;
    }

    ssize_t reply_len = coap_build_reply(pkt, result, response);
    uint8_t *pkt_pos = (uint8_t*)pkt->hdr + reply_len;
    if (blockwise) {
        pkt_pos += coap_opt_put_block1_control(pkt_pos, 0, &block1);
    }
    if (result_len) {
        *pkt_pos++ = 0xFF;
        pkt_pos += fmt_bytes_hex((char *)pkt_pos, digest, sizeof(digest));
    }

    return pkt_pos - (uint8_t*)pkt->hdr;
}

/* must be sorted by path (ASCII order) */
const coap_resource_t coap_resources[] = {
    COAP_WELL_KNOWN_CORE_DEFAULT_HANDLER,
    { "/echo/", COAP_GET | COAP_MATCH_SUBTREE, _echo_handler, NULL },
    { "/riot/board", COAP_GET, _riot_board_handler, NULL },
    { "/riot/value", COAP_GET | COAP_PUT | COAP_POST, _riot_value_handler, NULL },
    { "/riot/ver", COAP_GET, _riot_block2_handler, NULL },
    { "/sha256", COAP_POST, _sha256_handler, NULL },
};

const unsigned coap_resources_numof = ARRAY_SIZE(coap_resources);
