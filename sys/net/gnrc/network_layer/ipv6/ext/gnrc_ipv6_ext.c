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
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/ipv6.h"

#include "net/gnrc/ipv6/ext.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifdef MODULE_GNRC_RPL_SRH

enum gnrc_ipv6_ext_demux_status {
    GNRC_IPV6_EXT_OK,
    GNRC_IPV6_EXT_FORWARDED,
    GNRC_IPV6_EXT_ERROR,
};

static enum gnrc_ipv6_ext_demux_status _handle_rh(gnrc_pktsnip_t *current, gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *ipv6;
    ipv6_ext_t *ext = (ipv6_ext_t *) current->data;
    size_t current_offset;
    ipv6_hdr_t *hdr;

    /* check seg_left early to avoid duplicating the packet */
    if (((ipv6_ext_rh_t *)ext)->seg_left == 0) {
        return GNRC_IPV6_EXT_OK;
    }

    /* We cannot use `gnrc_pktbuf_start_write` since it duplicates only
       the head. `ipv6_ext_rh_process` modifies the IPv6 header as well as
       the extension header */

    current_offset = gnrc_pkt_len_upto(current->next, GNRC_NETTYPE_IPV6);

    if (pkt->users != 1) {
        if ((ipv6 = gnrc_pktbuf_duplicate_upto(pkt, GNRC_NETTYPE_IPV6)) == NULL) {
            DEBUG("ipv6: could not get a copy of pkt\n");
            gnrc_pktbuf_release(pkt);
            return GNRC_IPV6_EXT_ERROR;
        }
        pkt = ipv6;
        hdr = ipv6->data;
        ext = (ipv6_ext_t *)(((uint8_t *)ipv6->data) + current_offset);
    }
    else {
        ipv6 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6);
        hdr = ipv6->data;
    }

    switch (ipv6_ext_rh_process(hdr, (ipv6_ext_rh_t *)ext)) {
        case EXT_RH_CODE_ERROR:
            /* TODO: send ICMPv6 error codes */
            gnrc_pktbuf_release(pkt);
            return GNRC_IPV6_EXT_ERROR;

        case EXT_RH_CODE_FORWARD:
            /* forward packet */
            if (!gnrc_netapi_dispatch_receive(GNRC_NETTYPE_IPV6, GNRC_NETREG_DEMUX_CTX_ALL, pkt)) {
                DEBUG("ipv6: could not dispatch packet to the ipv6 thread\n");
                gnrc_pktbuf_release(pkt);
            }
            return GNRC_IPV6_EXT_FORWARDED;

        case EXT_RH_CODE_OK:
            /* this should not happen since we checked seg_left early */
            gnrc_pktbuf_release(pkt);
            return GNRC_IPV6_EXT_ERROR;
    }

    return GNRC_IPV6_EXT_OK;
}

#endif

/*
 *         current                 pkt
 *         |                       |
 *         v                       v
 * IPv6 <- IPv6_EXT <- IPv6_EXT <- UNDEF
 */
void gnrc_ipv6_ext_demux(kernel_pid_t iface,
                         gnrc_pktsnip_t *current,
                         gnrc_pktsnip_t *pkt,
                         uint8_t nh)
{
    gnrc_pktsnip_t *ext_snip, *tmp, *next;
    ipv6_ext_t *ext;
    size_t offset = 0;

    ext = (ipv6_ext_t *) current->data;

    switch (nh) {
        case PROTNUM_IPV6_EXT_RH:
#ifdef MODULE_GNRC_RPL_SRH
            switch (_handle_rh(current, pkt)) {
                case GNRC_IPV6_EXT_OK:
                    /* We are the final destination. So proceeds like normal packet. */
                    nh = ext->nh;
                    DEBUG("ipv6_ext: next header = %" PRIu8 "\n", nh);
                    offset = ((ext->len * IPV6_EXT_LEN_UNIT) + IPV6_EXT_LEN_UNIT);
                    break;

                case GNRC_IPV6_EXT_ERROR:
                    /* already released by _handle_rh, so no release here */
                    return;

                case GNRC_IPV6_EXT_FORWARDED:
                    return;
            }

            break;
#endif

        case PROTNUM_IPV6_EXT_HOPOPT:
        case PROTNUM_IPV6_EXT_DST:
        case PROTNUM_IPV6_EXT_FRAG:
        case PROTNUM_IPV6_EXT_AH:
        case PROTNUM_IPV6_EXT_ESP:
        case PROTNUM_IPV6_EXT_MOB:
            /* TODO: add handling of types */
            nh = ext->nh;
            DEBUG("ipv6_ext: next header = %" PRIu8 "\n", nh);
            offset = ((ext->len * IPV6_EXT_LEN_UNIT) + IPV6_EXT_LEN_UNIT);
            break;

        default:
            DEBUG("ipv6_ext: unknown next header: %" PRIu8 "\n", nh);
            gnrc_pktbuf_release(pkt);
            return;
    }

    if (current == pkt) {
        if ((tmp = gnrc_pktbuf_start_write(pkt)) == NULL) {
            DEBUG("ipv6: could not get a copy of pkt\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
        pkt = tmp;

        ext_snip = gnrc_pktbuf_mark(pkt, offset, GNRC_NETTYPE_IPV6_EXT);
        next = pkt;

        if (ext_snip == NULL) {
            gnrc_pktbuf_release(pkt);
            return;
        }
    }
    else {
        /* the header is already marked */

        next = NULL;

        for (tmp = pkt; tmp != NULL; tmp = tmp->next) {
            if (tmp->next == current) {
                next = tmp;
                break;
            }
        }

        assert(next != NULL);
    }

    gnrc_ipv6_demux(iface, next, pkt, nh);    /* demultiplex next header */

    return;
}

gnrc_pktsnip_t *gnrc_ipv6_ext_build(gnrc_pktsnip_t *ipv6, gnrc_pktsnip_t *next,
                                    uint8_t nh, size_t size)
{
    gnrc_pktsnip_t *prev = NULL, *snip;
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

    snip = gnrc_pktbuf_add(next, NULL, size, GNRC_NETTYPE_IPV6);

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
