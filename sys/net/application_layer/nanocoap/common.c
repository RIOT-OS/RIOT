/*
 * Copyright (C) 2016-17 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       common internal functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <errno.h>

#include "net/nanocoap.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

int _decode_value(unsigned val, uint8_t **pkt_pos_ptr, uint8_t *pkt_end)
{
    uint8_t *pkt_pos = *pkt_pos_ptr;
    size_t left = pkt_end - pkt_pos;
    int res;

    switch (val) {
        case 13:
        {
            /* An 8-bit unsigned integer follows the initial byte and
               indicates the Option Delta minus 13. */
            if (left < 1) {
                return -ENOSPC;
            }
            uint8_t delta = *pkt_pos++;
            res = delta + 13;
            break;
        }
        case 14:
        {
            /* A 16-bit unsigned integer in network byte order follows
             * the initial byte and indicates the Option Delta minus
             * 269. */
            if (left < 2) {
                return -ENOSPC;
            }
            uint16_t delta;
            uint8_t *_tmp = (uint8_t *)&delta;
            *_tmp++ = *pkt_pos++;
            *_tmp++ = *pkt_pos++;
            res = ntohs(delta) + 269;
            break;
        }
        case 15:
            /* Reserved for the Payload Marker.  If the field is set to
             * this value but the entire byte is not the payload
             * marker, this MUST be processed as a message format
             * error. */
            return -EBADMSG;
        default:
            res = val;
    }

    *pkt_pos_ptr = pkt_pos;
    return res;
}

uint8_t *_parse_option(coap_pkt_t *pkt, uint8_t *pkt_pos, uint16_t *delta, int *opt_len)
{
    uint8_t *hdr_end = pkt->payload;

    if (pkt_pos == hdr_end) {
        return NULL;
    }

    uint8_t option_byte = *pkt_pos++;

    *delta = _decode_value(option_byte >> 4, &pkt_pos, hdr_end);
    *opt_len = _decode_value(option_byte & 0xf, &pkt_pos, hdr_end);

    return pkt_pos;
}
