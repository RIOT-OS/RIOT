/*
 * Copyright (C) 2018 Ken Bannister <kb2ma@runbox.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_nanocoap
 * @{
 *
 * @file
 * @brief       nanocoap options sorting functions
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 *
 * @}
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "net/nanocoap.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

extern uint8_t *_parse_option(coap_pkt_t *pkt, uint8_t *pkt_pos, uint16_t *delta, int *opt_len);

/*
 * Sort the stored message options by option number. Rewrite the buffer and
 * update the options attributes in the pkt struct.
 *
 * pkt[inout]     Packet for sort
 * returns        0 on success
 */
ssize_t _sort_opts(coap_pkt_t *pkt)
{
    if (pkt->options_len <= 1) {
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
