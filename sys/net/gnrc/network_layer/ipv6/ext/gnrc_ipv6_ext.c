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

bool gnrc_ipv6_ext_demux(kernel_pid_t iface, gnrc_pktsnip_t *pkt,
                         uint8_t nh)
{
    gnrc_pktsnip_t *ext_snip, *tmp;
    ipv6_ext_t *ext;
    unsigned int offset = 0;
    ipv6_hdr_t *hdr;
    int res;

    ext = ((ipv6_ext_t *)(((uint8_t *)pkt->data) + sizeof(ipv6_hdr_t)));

    bool c = true;

    while (c) {
        switch (nh) {
            case PROTNUM_IPV6_EXT_HOPOPT:
            case PROTNUM_IPV6_EXT_DST:
            case PROTNUM_IPV6_EXT_RH:
                if ((tmp = gnrc_pktbuf_start_write(pkt)) == NULL) {
                    DEBUG("ipv6: could not get a copy of pkt\n");
                    gnrc_pktbuf_release(pkt);
                    return false;
                }
                pkt = tmp;
                hdr = pkt->data;
                ext = (ipv6_ext_t *) (((uint8_t *) pkt->data) + sizeof(ipv6_hdr_t) + offset);
                res = ipv6_ext_rh_process(hdr, (ipv6_ext_rh_t *)ext);
                if (res == EXT_RH_CODE_ERROR) {
                    /* TODO: send ICMPv6 error codes */
                    gnrc_pktbuf_release(pkt);
                    return false;
                }
                else if (res == EXT_RH_CODE_FORWARD) {
                    /* forward packet */
                    if (!gnrc_netapi_dispatch_receive(GNRC_NETTYPE_IPV6, GNRC_NETREG_DEMUX_CTX_ALL,
                                                      pkt)) {
                        DEBUG("ipv6: could not dispatch packet to the ipv6 thread\n");
                        gnrc_pktbuf_release(pkt);
                    }
                    return false;
                }
                else if (res == EXT_RH_CODE_OK) {
                    nh = ext->nh;
                    offset += ((ext->len * IPV6_EXT_LEN_UNIT) + IPV6_EXT_LEN_UNIT);
                    ext = ipv6_ext_get_next((ipv6_ext_t *)ext);
                }
                break;
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

    ext_snip = gnrc_pktbuf_mark(pkt, offset, GNRC_NETTYPE_IPV6);

    if (ext_snip == NULL) {
        gnrc_pktbuf_release(pkt);
        return false;
    }

    gnrc_ipv6_demux(iface, pkt, nh);    /* demultiplex next header */

    return true;
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
