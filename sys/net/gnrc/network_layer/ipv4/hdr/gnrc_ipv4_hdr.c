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

#include "net/ipv4/addr.h"
#include "net/gnrc/ipv4/hdr.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/pktbuf.h"
#include "net/protnum.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

#if defined(MODULE_IPV4_ADDR)
static char addr_str[IPV4_ADDR_MAX_STR_LEN];
#endif

/* For independent testing */
#ifdef MODULE_GNRC_IPV4
#define HDR_NETTYPE (GNRC_NETTYPE_IPV4)
#else
#define HDR_NETTYPE (GNRC_NETTYPE_UNDEF)
#endif

gnrc_pktsnip_t *gnrc_ipv4_hdr_build(gnrc_pktsnip_t *payload, const ipv4_addr_t *src,
                                    const ipv4_addr_t *dst)
{
    gnrc_pktsnip_t *ipv4;
    ipv4_hdr_t *hdr;

    ipv4 = gnrc_pktbuf_add(payload, NULL, sizeof(ipv4_hdr_t), HDR_NETTYPE);

    if (ipv4 == NULL) {
        DEBUG("ipv4_hdr: no space left in packet buffer\n");
        return NULL;
    }

    hdr = (ipv4_hdr_t *)ipv4->data;

    if (src != NULL) {
#ifdef MODULE_IPV4_ADDR
        DEBUG("ipv4_hdr: set packet source to %s\n",
              ipv4_addr_to_str(addr_str, (ipv4_addr_t *)src,
                               sizeof(addr_str)));
#endif
        memcpy(&hdr->src, src, sizeof(ipv4_addr_t));
    }
    else {
        DEBUG("ipv4_hdr: set packet source to unspecified\n");
        ipv4_addr_set_unspecified(&hdr->src);
    }

    if (dst != NULL) {
#ifdef MODULE_IPV4_ADDR
        DEBUG("ipv4_hdr: set packet destination to %s\n",
              ipv4_addr_to_str(addr_str, (ipv4_addr_t *)dst,
                               sizeof(addr_str)));
#endif
        memcpy(&hdr->dst, dst, sizeof(ipv4_addr_t));
    }
    else {
        DEBUG("ipv4_hdr: set packet destination to ::1\n");
        ipv4_addr_set_loopback(&hdr->dst);
    }

    ipv4_hdr_set_version(hdr);
    ipv4_hdr_set_ihl(hdr, 160);       // No options
    hdr->id = byteorder_htons(0);
    hdr->ttl = 64;
    hdr->ts = 0;
    hdr->csum = byteorder_htons(0);
    hdr->protocol = PROTNUM_RESERVED;
    // TODO: Check default values for other headers

    return ipv4;
}

/** @} */
