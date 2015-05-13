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

#include "net/ng_ipv6/addr.h"
#include "net/ng_ipv6/hdr.h"
#include "net/ng_nettype.h"
#include "net/ng_pktbuf.h"
#include "net/ng_protnum.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG && defined(MODULE_NG_IPV6_ADDR)
static char addr_str[NG_IPV6_ADDR_MAX_STR_LEN];
#endif

/* For independent testing */
#ifdef MODULE_NG_IPV6
#define HDR_NETTYPE (NG_NETTYPE_IPV6)
#else
#define HDR_NETTYPE (NG_NETTYPE_UNDEF)
#endif

ng_pktsnip_t *ng_ipv6_hdr_build(ng_pktsnip_t *payload,
                                uint8_t *src, uint8_t src_len,
                                uint8_t *dst, uint8_t dst_len)
{
    ng_pktsnip_t *ipv6;
    ng_ipv6_hdr_t *hdr;

    if (((src_len != 0) && (src_len != sizeof(ng_ipv6_addr_t))) ||
        ((dst_len != 0) && (dst_len != sizeof(ng_ipv6_addr_t)))) {
        DEBUG("ipv6_hdr: Address length was not 0 or %zu byte.\n",
              sizeof(ng_ipv6_addr_t));
        return NULL;
    }

    ipv6 = ng_pktbuf_add(payload, NULL, sizeof(ng_ipv6_hdr_t), HDR_NETTYPE);

    if (ipv6 == NULL) {
        DEBUG("ipv6_hdr: no space left in packet buffer\n");
        return NULL;
    }

    hdr = (ng_ipv6_hdr_t *)ipv6->data;

    if ((src != NULL) && (src_len != 0)) {
#ifdef MODULE_NG_IPV6_ADDR
        DEBUG("ipv6_hdr: set packet source to %s\n",
              ng_ipv6_addr_to_str(addr_str, (ng_ipv6_addr_t *)src,
                                  sizeof(addr_str)));
#endif
        memcpy(&hdr->src, src, src_len);
    }
    else {
        DEBUG("ipv6_hdr: set packet source to ::\n");
        ng_ipv6_addr_set_unspecified(&hdr->src);
    }

    memset(&hdr->dst + dst_len, 0, sizeof(ng_ipv6_addr_t) - dst_len);

    if ((dst != NULL) && (dst_len != 0)) {
#ifdef MODULE_NG_IPV6_ADDR
        DEBUG("ipv6_hdr: set packet destination to %s\n",
              ng_ipv6_addr_to_str(addr_str, (ng_ipv6_addr_t *)dst,
                                  sizeof(addr_str)));
#endif
        memcpy(&hdr->dst, dst, dst_len);
    }
    else {
        DEBUG("ipv6_hdr: set packet destination to ::1\n");
        ng_ipv6_addr_set_loopback(&hdr->dst);
    }

    hdr->v_tc_fl = byteorder_htonl(0x60000000); /* set version, tc and fl in one go*/
    hdr->nh = NG_PROTNUM_RESERVED;
    hdr->hl = 0;

    return ipv6;
}

/** @} */
