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
 * @file    ng_netreg.c
 */

#include <errno.h>
#include <string.h>

#include "clist.h"
#include "utlist.h"
#include "net/ng_netreg.h"
#include "net/ng_nettype.h"
#include "net/ng_pkt.h"
#include "net/ng_ipv6.h"

#define _INVALID_TYPE(type) (((type) < NG_NETTYPE_UNDEF) || ((type) >= NG_NETTYPE_NUMOF))

/* The registry as lookup table by ng_nettype_t */
static ng_netreg_entry_t *netreg[NG_NETTYPE_NUMOF];

void ng_netreg_init(void)
{
    /* set all pointers in registry to NULL */
    memset(netreg, 0, NG_NETTYPE_NUMOF * sizeof(ng_netreg_entry_t *));
}

int ng_netreg_register(ng_nettype_t type, ng_netreg_entry_t *entry)
{
    if (_INVALID_TYPE(type)) {
        return -EINVAL;
    }

    LL_PREPEND(netreg[type], entry);

    return 0;
}

void ng_netreg_unregister(ng_nettype_t type, ng_netreg_entry_t *entry)
{
    if (_INVALID_TYPE(type)) {
        return;
    }

    LL_DELETE(netreg[type], entry);
}

ng_netreg_entry_t *ng_netreg_lookup(ng_nettype_t type, uint32_t demux_ctx)
{
    ng_netreg_entry_t *res;

    if (_INVALID_TYPE(type)) {
        return NULL;
    }

    LL_SEARCH_SCALAR(netreg[type], res, demux_ctx, demux_ctx);

    return res;
}

int ng_netreg_num(ng_nettype_t type, uint32_t demux_ctx)
{
    int num = 0;
    ng_netreg_entry_t *entry;

    if (_INVALID_TYPE(type)) {
        return 0;
    }

    entry = netreg[type];

    while (entry != NULL) {
        if (entry->demux_ctx == demux_ctx) {
            num++;
        }

        entry = entry->next;
    }

    return num;
}

ng_netreg_entry_t *ng_netreg_getnext(ng_netreg_entry_t *entry)
{
    uint32_t demux_ctx;

    if (entry == NULL) {
        return NULL;
    }

    demux_ctx = entry->demux_ctx;

    LL_SEARCH_SCALAR(entry->next, entry, demux_ctx, demux_ctx);

    return entry;
}

int ng_netreg_calc_csum(ng_pktsnip_t *hdr, ng_pktsnip_t *pseudo_hdr)
{
    if (pseudo_hdr == NULL) {
        /* XXX: Might be allowed for future checksums.
         *      If this is the case: move this to the branches were it
         *      is needed. */
        return -EINVAL;
    }

    switch (hdr->type) {
#ifdef MODULE_NG_ICMPV6
        case NG_NETTYPE_ICMPV6:
            return ng_icmpv6_calc_csum(hdr, pseudo_hdr);
#endif
#ifdef MODULE_NG_TCP
        case NG_NETTYPE_TCP:
            return ng_tcp_calc_csum(hdr, pseudo_hdr);
#endif
#ifdef MODULE_NG_UDP
        case NG_NETTYPE_UDP:
            return ng_udp_calc_csum(hdr, pseudo_hdr);
#endif
        default:
            return -ENOENT;
    }
}

ng_pktsnip_t *ng_netreg_hdr_build(ng_nettype_t type, ng_pktsnip_t *payload,
                                  uint8_t *src, uint8_t src_len,
                                  uint8_t *dst, uint8_t dst_len)
{
    switch (type) {
#ifdef MODULE_NG_IPV6

        case NG_NETTYPE_IPV6:
            return ng_ipv6_hdr_build(payload, src, src_len, dst, dst_len);
#endif
#ifdef MODULE_NG_TCP

        case NG_NETTYPE_TCP:
            return ng_tcp_hdr_build(payload, src, src_len, dst, dst_len);
#endif
#ifdef MODULE_NG_UDP

        case NG_NETTYPE_UDP:
            return ng_udp_hdr_build(payload, src, src_len, dst, dst_len);
#endif

        default:
            (void)payload;
            (void)src;
            (void)src_len;
            (void)dst;
            (void)dst_len;
            return NULL;
    }
}

/** @} */
