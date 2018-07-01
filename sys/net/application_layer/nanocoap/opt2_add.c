/*
 * Copyright (C) 2018 Ken Bannister <kb2ma@runbox.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "net/nanocoap.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifdef MODULE_NANOCOAP_OPT2_SORT
ssize_t _sort_opts(coap_pkt_t *pkt);
#endif

extern size_t _encode_uint(uint32_t *val);
extern uint8_t *_parse_option(coap_pkt_t *pkt, uint8_t *pkt_pos, uint16_t *delta,
                              int *opt_len);

/* Common functionality for addition of an option */
static ssize_t _add_opt_pkt(coap_pkt_t *pkt, uint16_t optnum, uint8_t *val,
                            size_t val_len)
{
    assert(pkt->options_len < NANOCOAP_NOPTS_MAX);

    uint16_t lastonum = (pkt->options_len)
            ? pkt->options[pkt->options_len - 1].opt_num : 0;

    size_t optlen = coap_put_option(pkt->payload, lastonum, optnum, val, val_len);
    assert(pkt->payload_len > optlen);

    pkt->options[pkt->options_len].opt_num = optnum;
    pkt->options[pkt->options_len].offset = pkt->payload - (uint8_t *)pkt->hdr;
    pkt->options_len++;
    pkt->payload += optlen;
    pkt->payload_len -= optlen;

    return optlen;
}

ssize_t coap_opt_add_string(coap_pkt_t *pkt, uint16_t optnum, const char *string,
                           char separator)
{
    size_t unread_len = strlen(string);
    if (!unread_len) {
        return 0;
    }
    char *uripos = (char *)string;
    size_t write_len = 0;

    while (unread_len) {
        size_t part_len;
        uripos++;
        uint8_t *part_start = (uint8_t *)uripos;

        while (unread_len--) {
            if ((*uripos == separator) || (*uripos == '\0')) {
                break;
            }
            uripos++;
        }

        part_len = (uint8_t *)uripos - part_start;

        if (part_len) {
            if (pkt->options_len == NANOCOAP_NOPTS_MAX) {
                return -ENOSPC;
            }
            write_len += _add_opt_pkt(pkt, optnum, part_start, part_len);
        }
    }

    return write_len;
}

ssize_t coap_opt_add_uint(coap_pkt_t *pkt, uint16_t optnum, uint32_t value)
{
    uint32_t tmp = value;
    unsigned tmp_len = _encode_uint(&tmp);
    return _add_opt_pkt(pkt, optnum, (uint8_t *)&tmp, tmp_len);
}

ssize_t coap_opt_finish(coap_pkt_t *pkt, uint16_t flags)
{
#ifdef MODULE_NANOCOAP_OPT2_SORT
    _sort_opts(pkt);
#endif
    if (flags & COAP_OPT_FINISH_PAYLOAD) {
        assert(pkt->payload_len > 1);

        *pkt->payload++ = 0xFF;
        pkt->payload_len--;
    }
    else {
        pkt->payload_len = 0;
    }

    return pkt->payload - (uint8_t *)pkt->hdr;
}

void coap_pkt_init(coap_pkt_t *pkt, uint8_t *buf, size_t len, size_t header_len)
{
    memset(pkt, 0, sizeof(coap_pkt_t));
    pkt->hdr = (coap_hdr_t *)buf;
    pkt->token = buf + sizeof(coap_hdr_t);
    pkt->payload = buf + header_len;
    pkt->payload_len = len - header_len;
}

#ifdef MODULE_NANOCOAP_OPT2_SORT
static bool _sorted(coap_pkt_t *pkt)
{
    for (unsigned i = 1; i < pkt->options_len; i++) {
        if (pkt->options[i].opt_num < pkt->options[i-1].opt_num) {
            return false;
        }
    }
    return true;
}

/*
 * Sort the stored message options by option number. Rewrite the buffer and
 * update the options attributes in the pkt struct.
 *
 * pkt[inout]     Packet for sort
 * returns        0 on success
 */
ssize_t _sort_opts(coap_pkt_t *pkt)
{
    if (_sorted(pkt)) {
        return 0;
    }

    uint8_t *options = (uint8_t *)pkt->hdr + coap_get_total_hdr_len(pkt);
    /* scratch buffer to hold sorted options, with extra space to handle any
     * large option header */
    uint8_t scratch[(pkt->payload - options) + 4];
    uint8_t *scratch_pos = &scratch[0];

    unsigned sorted   = 0;        /* count of sorted elements */
    uint8_t *next_pos = options;

    do {
        // find next option to write
        unsigned i, best_i = sorted;
        for (i = sorted+1; i < pkt->options_len; i++) {
            if (pkt->options[i].opt_num < pkt->options[best_i].opt_num) {
                best_i = i;
            }
        }
        i = best_i;
        /* prepare to relocate option memo currently at sorted location */
        coap_optpos_t bubble_opt;
        bubble_opt.opt_num = pkt->options[sorted].opt_num;
        bubble_opt.offset  = pkt->options[sorted].offset;

        /* read selected option */
        uint16_t delta;      // unused
        int option_len;
        uint8_t *val = _parse_option(pkt, (uint8_t *)pkt->hdr + pkt->options[i].offset,
                                     &delta, &option_len);

        /* write option to scratch buffer */
        uint8_t *last_pos = next_pos;
        next_pos += coap_put_option(scratch_pos,
                                    sorted ? pkt->options[sorted-1].opt_num : 0,
                                    pkt->options[i].opt_num, val, option_len);
        scratch_pos += (next_pos - last_pos);

        /* update pkt->options record and save bubbled option */
        pkt->options[sorted].opt_num = pkt->options[i].opt_num;
        pkt->options[sorted].offset  = last_pos - (uint8_t *)pkt->hdr;
        if (i != sorted) {
            pkt->options[i].opt_num = bubble_opt.opt_num;
            pkt->options[i].offset  = bubble_opt.offset;
        }
    } while (++sorted < pkt->options_len);

    int len_delta = next_pos - pkt->payload;
    DEBUG("sort moves payload by %d\n", len_delta);

    memcpy(options, &scratch[0], next_pos - options);

    pkt->payload     += len_delta;
    pkt->payload_len -= len_delta;
    return 0;
}
#endif    /* MODULE_NANOCOAP_OPT2_SORT */
