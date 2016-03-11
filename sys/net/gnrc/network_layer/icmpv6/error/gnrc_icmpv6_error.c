/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

#include "net/gnrc/pktbuf.h"

#include "net/ipv6.h"
#include "net/gnrc/ipv6/netif.h"
#include "net/gnrc/icmpv6/error.h"
#include "net/gnrc/icmpv6.h"

/* all error messages are basically the same size and format */
#define ICMPV6_ERROR_SZ (sizeof(icmpv6_error_dst_unr_t))
#define ICMPV6_ERROR_SET_VALUE(data, value) \
    ((icmpv6_error_pkt_too_big_t *)(data))->mtu = byteorder_htonl(value)

/* TODO: generalize and centralize (see https://github.com/RIOT-OS/RIOT/pull/3184) */
#define MIN(a, b)   ((a) < (b)) ? (a) : (b)

static inline size_t _fit(gnrc_pktsnip_t *pkt)
{
    /* TODO: replace IPV6_MIN_MTU with known path MTU? */
    return MIN((gnrc_pkt_len(pkt) + ICMPV6_ERROR_SZ), IPV6_MIN_MTU);
}

/* Build a generic error message */
static gnrc_pktsnip_t *_icmpv6_error_build(uint8_t type, uint8_t code,
                                           gnrc_pktsnip_t *orig_pkt, uint32_t value)
{
    gnrc_pktsnip_t *pkt = gnrc_icmpv6_build(NULL, type, code, _fit(orig_pkt));

    /* copy as much of the originating packet into error message as fits the message's size */
    if (pkt != NULL) {
        size_t offset = ICMPV6_ERROR_SZ;
        uint8_t *data = pkt->data;
        ICMPV6_ERROR_SET_VALUE(data, value);
        while ((orig_pkt != NULL) && (offset < pkt->size)) {
            memcpy(data + offset, orig_pkt->data,
                   MIN(pkt->size - offset, orig_pkt->size));
            offset += MIN(pkt->size - offset, orig_pkt->size);
            orig_pkt = orig_pkt->next;
        }
    }

    return pkt;
}

gnrc_pktsnip_t *gnrc_icmpv6_error_dst_unr_build(uint8_t code, gnrc_pktsnip_t *orig_pkt)
{
    return _icmpv6_error_build(ICMPV6_DST_UNR, code, orig_pkt, 0);
}

gnrc_pktsnip_t *gnrc_icmpv6_error_pkt_too_big_build(uint32_t mtu, gnrc_pktsnip_t *orig_pkt)
{
    return _icmpv6_error_build(ICMPV6_PKT_TOO_BIG, 0, orig_pkt, mtu);
}

gnrc_pktsnip_t *gnrc_icmpv6_error_time_exc_build(uint8_t code, gnrc_pktsnip_t *orig_pkt)
{
    return _icmpv6_error_build(ICMPV6_TIME_EXC, code, orig_pkt, 0);
}

static inline bool _in_range(uint8_t *ptr, uint8_t *start, size_t sz)
{
    return (ptr >= start) && (ptr < (start + sz));
}

gnrc_pktsnip_t *gnrc_icmpv6_error_param_prob_build(uint8_t code, void *ptr,
                                                   gnrc_pktsnip_t *orig_pkt)
{
    gnrc_pktsnip_t *pkt = gnrc_icmpv6_build(NULL, ICMPV6_PARAM_PROB, code,
                                            _fit(orig_pkt));

    /* copy as much of the originating packet into error message and
     * determine relative *ptr* offset */
    if (pkt != NULL) {
        size_t offset = sizeof(icmpv6_error_param_prob_t);
        uint8_t *data = pkt->data;
        uint32_t ptr_offset = 0U;
        bool found_offset = false;

        while (orig_pkt != NULL) {
            /* copy as long as it fits into packet */
            if (offset < pkt->size) {
                memcpy(data + offset, orig_pkt->data,
                       MIN(pkt->size - offset, orig_pkt->size));
                offset += MIN(pkt->size - offset, orig_pkt->size);
            }

            if (_in_range(ptr, orig_pkt->data, orig_pkt->size)) {
                ptr_offset += (uint32_t)(((uint8_t *)ptr) - ((uint8_t *)orig_pkt->data));
                found_offset = true;
            }
            else if (!found_offset) {
                ptr_offset += (uint32_t)orig_pkt->size;
            }

            orig_pkt = orig_pkt->next;

            if ((offset < pkt->size) && found_offset) {
                break;
            }
        }

        /* set "pointer" field to relative pointer offset */
        ((icmpv6_error_param_prob_t *)data)->ptr = byteorder_htonl(ptr_offset);
    }

    return pkt;
}

/** @} */
