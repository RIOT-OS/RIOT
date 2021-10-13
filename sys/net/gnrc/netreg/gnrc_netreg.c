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
#include "log.h"
#include "utlist.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/icmpv6.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/udp.h"
#ifdef MODULE_GNRC_TCP
#include "net/gnrc/tcp.h"
#endif

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
#if DEVELHELP
# if defined(MODULE_GNRC_NETAPI_MBOX) || defined(MODULE_GNRC_NETAPI_CALLBACKS)
    bool has_msg_q = (entry->type != GNRC_NETREG_TYPE_DEFAULT) ||
                     thread_has_msg_queue(thread_get(entry->target.pid));
# else
    bool has_msg_q = thread_has_msg_queue(thread_get(entry->target.pid));
# endif

    /* only threads with a message queue are allowed to register at gnrc */
    if (!has_msg_q) {
        LOG_ERROR("\n!!!! gnrc_netreg: initialize message queue of thread %u "
                  "using msg_init_queue() !!!!\n\n", entry->target.pid);
    }
    assert(has_msg_q);
#endif /* DEVELHELP */

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

/**
 * @brief   Searches the next entry in the registry that matches given
 *          parameters, start lookup from beginning or given entry.
 *
 * @param[in] from      A registry entry to lookup from or NULL to start fresh
 * @param[in] type      Type of the protocol.
 * @param[in] demux_ctx The demultiplexing context for the registered thread.
 *                      See gnrc_netreg_entry_t::demux_ctx.
 *
 * @return  The first entry fitting the given parameters on success
 * @return  NULL if no entry can be found.
 */
static gnrc_netreg_entry_t *_netreg_lookup(gnrc_netreg_entry_t *from,
                                           gnrc_nettype_t type,
                                           uint32_t demux_ctx)
{
    gnrc_netreg_entry_t *res = NULL;

    if (from || !_INVALID_TYPE(type)) {
        gnrc_netreg_entry_t *head = (from) ? from->next : netreg[type];
        LL_SEARCH_SCALAR(head, res, demux_ctx, demux_ctx);
    }

    return res;
}

gnrc_netreg_entry_t *gnrc_netreg_lookup(gnrc_nettype_t type, uint32_t demux_ctx)
{
    return _netreg_lookup(NULL, type, demux_ctx);
}

int gnrc_netreg_num(gnrc_nettype_t type, uint32_t demux_ctx)
{
    int num = 0;
    gnrc_netreg_entry_t *entry = NULL;

    while((entry = _netreg_lookup(entry, type, demux_ctx)) != NULL) {
        num++;
    }
    return num;
}

gnrc_netreg_entry_t *gnrc_netreg_getnext(gnrc_netreg_entry_t *entry)
{
    return (entry ? _netreg_lookup(entry, 0, entry->demux_ctx) : NULL);
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
