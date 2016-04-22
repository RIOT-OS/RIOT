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

#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/hdr.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/pktbuf.h"
#include "net/protnum.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG && defined(MODULE_IPV6_ADDR)
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

/* For independent testing */
#ifdef MODULE_GNRC_IPV6
#define HDR_NETTYPE (GNRC_NETTYPE_IPV6)
#else
#define HDR_NETTYPE (GNRC_NETTYPE_UNDEF)
#endif

gnrc_pktsnip_t *gnrc_ipv6_hdr_build(gnrc_pktsnip_t *payload, const ipv6_addr_t *src,
                                    const ipv6_addr_t *dst)
{
    gnrc_pktsnip_t *ipv6;
    ipv6_hdr_t *hdr;

    ipv6 = gnrc_pktbuf_add(payload, NULL, sizeof(ipv6_hdr_t), HDR_NETTYPE);

    if (ipv6 == NULL) {
        DEBUG("ipv6_hdr: no space left in packet buffer\n");
        return NULL;
    }

    hdr = (ipv6_hdr_t *)ipv6->data;

    if (src != NULL) {
#ifdef MODULE_IPV6_ADDR
        DEBUG("ipv6_hdr: set packet source to %s\n",
              ipv6_addr_to_str(addr_str, (ipv6_addr_t *)src,
                               sizeof(addr_str)));
#endif
        memcpy(&hdr->src, src, sizeof(ipv6_addr_t));
    }
    else {
        DEBUG("ipv6_hdr: set packet source to ::\n");
        ipv6_addr_set_unspecified(&hdr->src);
    }

    if (dst != NULL) {
#ifdef MODULE_IPV6_ADDR
        DEBUG("ipv6_hdr: set packet destination to %s\n",
              ipv6_addr_to_str(addr_str, (ipv6_addr_t *)dst,
                               sizeof(addr_str)));
#endif
        memcpy(&hdr->dst, dst, sizeof(ipv6_addr_t));
    }
    else {
        DEBUG("ipv6_hdr: set packet destination to ::1\n");
        ipv6_addr_set_loopback(&hdr->dst);
    }

    hdr->v_tc_fl = byteorder_htonl(0x60000000); /* set version, tc and fl in one go*/
    hdr->nh = PROTNUM_RESERVED;
    hdr->hl = 0;

    return ipv6;
}

/** @} */
