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

#include <stdint.h>
#include <stdlib.h>

#include "kernel_types.h"
#include "net/fib.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/ndp.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/pktqueue.h"

#include "net/gnrc/ndp/internal.h"

#include "net/gnrc/ndp/node.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

static gnrc_pktqueue_t _pkt_nodes[GNRC_IPV6_NC_SIZE * 2];

/**
 * @brief   Allocates a node for the packet queue.
 *
 * @param[in] pkt   Packet to add to the packet queue.
 *
 * @return  A packet queue node.
 */
static gnrc_pktqueue_t *_alloc_pkt_node(gnrc_pktsnip_t *pkt)
{
    for (size_t i = 0; i < sizeof(_pkt_nodes) / sizeof(gnrc_pktqueue_t); i++) {
        if ((_pkt_nodes[i].pkt == NULL) && (_pkt_nodes[i].next == NULL)) {
            _pkt_nodes[i].pkt = pkt;

            return &(_pkt_nodes[i]);
        }
    }

    return NULL;
}

bool gnrc_ndp_node_next_hop_ipv6_addr(ipv6_addr_t *next_hop_ip,
                                    kernel_pid_t *iface,
                                    ipv6_addr_t *dst)
{
#ifdef MODULE_GNRC_IPV6_EXT_RH
    next_hop_ip = gnrc_ipv6_ext_rh_next_hop(hdr);
    if (next_hop_ip != NULL) {
        return true;
    }
#endif
#ifdef MODULE_FIB
    size_t next_hop_size = sizeof(ipv6_addr_t);
    uint32_t next_hop_flags = 0;
    ipv6_addr_t next_hop_actual;

    if ((fib_get_next_hop(iface, next_hop_actual.u8, &next_hop_size,
                          &next_hop_flags, (uint8_t *)dst,
                          sizeof(ipv6_addr_t), 0) >= 0) &&
        (next_hop_size == sizeof(ipv6_addr_t))) {
            next_hop_ip = &next_hop_actual;
            return true;
    }
#endif
    next_hop_ip = gnrc_ndp_internal_default_router();
    if (next_hop_ip != NULL) {
        return true;
    }
    return false;
}

kernel_pid_t gnrc_ndp_node_next_hop_l2addr(uint8_t *l2addr, uint8_t *l2addr_len,
                                         kernel_pid_t iface, ipv6_addr_t *dst,
                                         gnrc_pktsnip_t *pkt)
{
    assert(dst != NULL);

    gnrc_ipv6_nc_t *nc_entry = gnrc_ipv6_nc_get(iface, dst);

    if ((nc_entry != NULL) && gnrc_ipv6_nc_is_reachable(nc_entry)) {
        DEBUG("ndp node: found reachable neighbor (%s => ",
              ipv6_addr_to_str(addr_str, &nc_entry->ipv6_addr, sizeof(addr_str)));
        DEBUG("%s)\n",
              gnrc_netif_addr_to_str(addr_str, sizeof(addr_str),
                                     nc_entry->l2_addr, nc_entry->l2_addr_len));

        if (gnrc_ipv6_nc_get_state(nc_entry) == GNRC_IPV6_NC_STATE_STALE) {
            gnrc_ndp_internal_set_state(nc_entry, GNRC_IPV6_NC_STATE_DELAY);
        }

        memcpy(l2addr, nc_entry->l2_addr, nc_entry->l2_addr_len);
        *l2addr_len = nc_entry->l2_addr_len;
        /* TODO: unreachability check */
        return nc_entry->iface;
    }
    else if (nc_entry == NULL) {
        gnrc_pktqueue_t *pkt_node;
        ipv6_addr_t dst_sol;

        nc_entry = gnrc_ipv6_nc_add(iface, dst, NULL, 0,
                                    GNRC_IPV6_NC_STATE_INCOMPLETE << GNRC_IPV6_NC_STATE_POS);

        if (nc_entry == NULL) {
            DEBUG("ndp node: could not create neighbor cache entry\n");
            return KERNEL_PID_UNDEF;
        }

        pkt_node = _alloc_pkt_node(pkt);

        if (pkt_node == NULL) {
            DEBUG("ndp node: could not add packet to packet queue\n");
        }
        else {
            /* prevent packet from being released by IPv6 */
            gnrc_pktbuf_hold(pkt_node->pkt, 1);
            gnrc_pktqueue_add(&nc_entry->pkts, pkt_node);
        }

        /* address resolution */
        ipv6_addr_set_solicited_nodes(&dst_sol, dst);

        if (iface == KERNEL_PID_UNDEF) {
            timex_t t = { 0, GNRC_NDP_RETRANS_TIMER };
            kernel_pid_t ifs[GNRC_NETIF_NUMOF];
            size_t ifnum = gnrc_netif_get(ifs);

            for (size_t i = 0; i < ifnum; i++) {
                gnrc_ndp_internal_send_nbr_sol(ifs[i], dst, &dst_sol);
            }

            vtimer_remove(&nc_entry->nbr_sol_timer);
            vtimer_set_msg(&nc_entry->nbr_sol_timer, t, gnrc_ipv6_pid,
                           GNRC_NDP_MSG_NBR_SOL_RETRANS, nc_entry);
        }
        else {
            gnrc_ipv6_netif_t *ipv6_iface = gnrc_ipv6_netif_get(iface);

            gnrc_ndp_internal_send_nbr_sol(iface, dst, &dst_sol);

            mutex_lock(&ipv6_iface->mutex);
            vtimer_remove(&nc_entry->nbr_sol_timer);
            vtimer_set_msg(&nc_entry->nbr_sol_timer,
                           ipv6_iface->retrans_timer, gnrc_ipv6_pid,
                           GNRC_NDP_MSG_NBR_SOL_RETRANS, nc_entry);
            mutex_unlock(&ipv6_iface->mutex);
        }
    }

    return KERNEL_PID_UNDEF;
}


/** @} */
