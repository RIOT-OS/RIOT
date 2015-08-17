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

#include <errno.h>

#include "utlist.h"
#include "net/ng_pktbuf.h"
#include "net/ng_ipv6.h"

#include "net/ng_ipv6/ext.h"

bool ng_ipv6_ext_demux(kernel_pid_t iface, ng_pktsnip_t *pkt,
                       uint8_t nh)
{
    ng_pktsnip_t *ext_snip;
    ipv6_ext_t *ext;
    unsigned int offset = 0;

    ext = ((ipv6_ext_t *)(((uint8_t *)pkt->data) + sizeof(ipv6_hdr_t)));

    bool c = true;

    while (c) {
        switch (nh) {
            case PROTNUM_IPV6_EXT_HOPOPT:
            case PROTNUM_IPV6_EXT_DST:
            case PROTNUM_IPV6_EXT_RH:
            case PROTNUM_IPV6_EXT_FRAG:
            case PROTNUM_IPV6_EXT_AH:
            case PROTNUM_IPV6_EXT_ESP:
            case PROTNUM_IPV6_EXT_MOB:
                /* TODO: add handling of types */
                nh = ext->nh;
                offset += ((ext->len * IPV6_EXT_LEN_UNIT) + IPV6_EXT_LEN_UNIT);
                ext = ipv6_ext_get_next((ipv6_ext_t *)ext);
                break;

            default:
                c = false;
                offset += ((ext->len * IPV6_EXT_LEN_UNIT) + IPV6_EXT_LEN_UNIT);
                ext = ipv6_ext_get_next((ipv6_ext_t *)ext);
                break;
        }
    }

    ext_snip = ng_pktbuf_mark(pkt, offset, NG_NETTYPE_IPV6);

    if (ext_snip == NULL) {
        return false;
    }

    ng_ipv6_demux(iface, pkt, nh); /* demultiplex next header */

    return true;
}

ng_pktsnip_t *ng_ipv6_ext_build(ng_pktsnip_t *ipv6, ng_pktsnip_t *next,
                                uint8_t nh, size_t size)
{
    ng_pktsnip_t *prev = NULL, *snip;
    ipv6_ext_t *ext;

    if (size < IPV6_EXT_LEN_UNIT) {
        return NULL;
    }

    if (ipv6 != NULL) {
        LL_SEARCH_SCALAR(ipv6, prev, next, next);

        if (prev == NULL) {
            return NULL;
        }
    }

    snip = ng_pktbuf_add(next, NULL, size, NG_NETTYPE_IPV6);

    if (snip == NULL) {
        return NULL;
    }

    ext = snip->data;

    ext->nh = nh;

    if (size & 0x7) { /* not divisible by eight */
        ext->len = (size / IPV6_EXT_LEN_UNIT);
    }
    else {
        ext->len = (size / IPV6_EXT_LEN_UNIT) - 1;
    }

    if (prev != NULL) {
        prev->next = snip;
    }

    return snip;
}


/** @} */
