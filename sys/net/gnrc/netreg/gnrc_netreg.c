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
#include <string.h>

#include "assert.h"
#include "clist.h"
#include "utlist.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/icmpv6.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/udp.h"

#define _INVALID_TYPE(type) (((type) < GNRC_NETTYPE_UNDEF) || ((type) >= GNRC_NETTYPE_NUMOF))

/* The registry as lookup table by gnrc_nettype_t */
static gnrc_netreg_entry_t *netreg[GNRC_NETTYPE_NUMOF];

void gnrc_netreg_init(void)
{
    /* set all pointers in registry to NULL */
    memset(netreg, 0, GNRC_NETTYPE_NUMOF * sizeof(gnrc_netreg_entry_t *));
}

int gnrc_netreg_register(gnrc_nettype_t type, gnrc_netreg_entry_t *entry)
{
    /* only threads with a message queue are allowed to register at gnrc */
    assert(sched_threads[entry->pid]->msg_array);

    if (_INVALID_TYPE(type)) {
        return -EINVAL;
    }

    LL_PREPEND(netreg[type], entry);

    return 0;
}

void gnrc_netreg_unregister(gnrc_nettype_t type, gnrc_netreg_entry_t *entry)
{
    if (_INVALID_TYPE(type)) {
        return;
    }

    LL_DELETE(netreg[type], entry);
}

gnrc_netreg_entry_t *gnrc_netreg_lookup(gnrc_nettype_t type, uint32_t demux_ctx)
{
    gnrc_netreg_entry_t *res;

    if (_INVALID_TYPE(type)) {
        return NULL;
    }

    LL_SEARCH_SCALAR(netreg[type], res, demux_ctx, demux_ctx);

    return res;
}

int gnrc_netreg_num(gnrc_nettype_t type, uint32_t demux_ctx)
{
    int num = 0;
    gnrc_netreg_entry_t *entry;

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

gnrc_netreg_entry_t *gnrc_netreg_getnext(gnrc_netreg_entry_t *entry)
{
    uint32_t demux_ctx;

    if (entry == NULL) {
        return NULL;
    }

    demux_ctx = entry->demux_ctx;

    LL_SEARCH_SCALAR(entry->next, entry, demux_ctx, demux_ctx);

    return entry;
}

int gnrc_netreg_calc_csum(gnrc_pktsnip_t *hdr, gnrc_pktsnip_t *pseudo_hdr)
{
    if (pseudo_hdr == NULL) {
        /* XXX: Might be allowed for future checksums.
         *      If this is the case: move this to the branches were it
         *      is needed. */
        return -EINVAL;
    }

    switch (hdr->type) {
#ifdef MODULE_GNRC_ICMPV6
        case GNRC_NETTYPE_ICMPV6:
            return gnrc_icmpv6_calc_csum(hdr, pseudo_hdr);
#endif
#ifdef MODULE_GNRC_TCP
        case GNRC_NETTYPE_TCP:
            return gnrc_tcp_calc_csum(hdr, pseudo_hdr);
#endif
#ifdef MODULE_GNRC_UDP
        case GNRC_NETTYPE_UDP:
            return gnrc_udp_calc_csum(hdr, pseudo_hdr);
#endif
        default:
            return -ENOENT;
    }
}

/** @} */
