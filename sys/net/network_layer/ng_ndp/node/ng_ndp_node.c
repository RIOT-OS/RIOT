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
#include "net/ng_ipv6.h"
#include "net/ng_ndp.h"
#include "net/ng_pkt.h"
#include "net/ng_pktqueue.h"

#include "net/ng_ndp/internal.h"

#include "net/ng_ndp/node.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

static ng_pktqueue_t _pkt_nodes[NG_IPV6_NC_SIZE * 2];

/**
 * @brief   Allocates a node for the packet queue.
 *
 * @param[in] pkt   Packet to add to the packet queue.
 *
 * @return  A packet queue node.
 */
static ng_pktqueue_t *_alloc_pkt_node(ng_pktsnip_t *pkt)
{
    for (size_t i = 0; i < sizeof(_pkt_nodes) / sizeof(ng_pktqueue_t); i++) {
        if ((_pkt_nodes[i].pkt == NULL) && (_pkt_nodes[i].next == NULL)) {
            _pkt_nodes[i].pkt = pkt;

            return &(_pkt_nodes[i]);
        }
    }

    return NULL;
}

kernel_pid_t ng_ndp_node_next_hop_l2addr(uint8_t *l2addr, uint8_t *l2addr_len,
                                         kernel_pid_t iface, ipv6_addr_t *dst,
                                         ng_pktsnip_t *pkt)
{
    ipv6_addr_t *next_hop_ip = NULL, *prefix = NULL;

#ifdef MODULE_NG_IPV6_EXT_RH
    next_hop_ip = ng_ipv6_ext_rh_next_hop(hdr);
#endif
#ifdef MODULE_FIB
    size_t next_hop_size = sizeof(ipv6_addr_t);
    uint32_t next_hop_flags = 0;
    ipv6_addr_t next_hop_actual;    /* FIB copies address into this variable */

    if ((next_hop_ip == NULL) &&
        (fib_get_next_hop(&iface, next_hop_actual.u8, &next_hop_size,
                          &next_hop_flags, (uint8_t *)dst,
                          sizeof(ipv6_addr_t), 0) >= 0) &&
        (next_hop_size == sizeof(ipv6_addr_t))) {
        next_hop_ip = &next_hop_actual;
    }

#endif

    if ((next_hop_ip == NULL)) {            /* no route to host */
        if (iface == KERNEL_PID_UNDEF) {
            /* ng_ipv6_netif_t doubles as prefix list */
            iface = ng_ipv6_netif_find_by_prefix(&prefix, dst);
        }
        else {
            /* ng_ipv6_netif_t doubles as prefix list */
            prefix = ng_ipv6_netif_match_prefix(iface, dst);
        }

        if ((prefix != NULL) &&             /* prefix is on-link */
            (ng_ipv6_netif_addr_get(prefix)->flags &
             NG_IPV6_NETIF_ADDR_FLAGS_NDP_ON_LINK)) {
            next_hop_ip = dst;
#ifdef MODULE_FIB
            /* We don't care if FIB is full, this is just for efficiency
             * for later sends */
            fib_add_entry(iface, (uint8_t *)dst, sizeof(ipv6_addr_t), 0,
                          (uint8_t *)next_hop_ip, sizeof(ipv6_addr_t), 0,
                          FIB_LIFETIME_NO_EXPIRE);
#endif
        }
    }

    if (next_hop_ip == NULL) {
        next_hop_ip = ng_ndp_internal_default_router();
#ifdef MODULE_FIB
        /* We don't care if FIB is full, this is just for efficiency for later
         * sends */
        fib_add_entry(iface, (uint8_t *)dst, sizeof(ipv6_addr_t), 0,
                      (uint8_t *)next_hop_ip, sizeof(ipv6_addr_t), 0,
                      FIB_LIFETIME_NO_EXPIRE);
#endif
    }

    if (next_hop_ip != NULL) {
        ng_ipv6_nc_t *nc_entry = ng_ipv6_nc_get(iface, next_hop_ip);

        if ((nc_entry != NULL) && ng_ipv6_nc_is_reachable(nc_entry)) {
            DEBUG("ndp node: found reachable neighbor (%s => ",
                  ipv6_addr_to_str(addr_str, &nc_entry->ipv6_addr, sizeof(addr_str)));
            DEBUG("%s)\n",
                  ng_netif_addr_to_str(addr_str, sizeof(addr_str),
                                       nc_entry->l2_addr, nc_entry->l2_addr_len));

            if (ng_ipv6_nc_get_state(nc_entry) == NG_IPV6_NC_STATE_STALE) {
                ng_ndp_internal_set_state(nc_entry, NG_IPV6_NC_STATE_DELAY);
            }

            memcpy(l2addr, nc_entry->l2_addr, nc_entry->l2_addr_len);
            *l2addr_len = nc_entry->l2_addr_len;
            /* TODO: unreachability check */
            return nc_entry->iface;
        }
        else if (nc_entry == NULL) {
            ng_pktqueue_t *pkt_node;
            ipv6_addr_t dst_sol;

            nc_entry = ng_ipv6_nc_add(iface, next_hop_ip, NULL, 0,
                                      NG_IPV6_NC_STATE_INCOMPLETE << NG_IPV6_NC_STATE_POS);

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
                ng_pktbuf_hold(pkt_node->pkt, 1);
                ng_pktqueue_add(&nc_entry->pkts, pkt_node);
            }

            /* address resolution */
            ipv6_addr_set_solicited_nodes(&dst_sol, next_hop_ip);

            if (iface == KERNEL_PID_UNDEF) {
                timex_t t = { 0, NG_NDP_RETRANS_TIMER };
                kernel_pid_t ifs[NG_NETIF_NUMOF];
                size_t ifnum = ng_netif_get(ifs);

                for (size_t i = 0; i < ifnum; i++) {
                    ng_ndp_internal_send_nbr_sol(ifs[i], next_hop_ip, &dst_sol);
                }

                vtimer_remove(&nc_entry->nbr_sol_timer);
                vtimer_set_msg(&nc_entry->nbr_sol_timer, t, ng_ipv6_pid,
                               NG_NDP_MSG_NBR_SOL_RETRANS, nc_entry);
            }
            else {
                ng_ipv6_netif_t *ipv6_iface = ng_ipv6_netif_get(iface);

                ng_ndp_internal_send_nbr_sol(iface, next_hop_ip, &dst_sol);

                mutex_lock(&ipv6_iface->mutex);
                vtimer_remove(&nc_entry->nbr_sol_timer);
                vtimer_set_msg(&nc_entry->nbr_sol_timer,
                               ipv6_iface->retrans_timer, ng_ipv6_pid,
                               NG_NDP_MSG_NBR_SOL_RETRANS, nc_entry);
                mutex_unlock(&ipv6_iface->mutex);
            }
        }
    }

    return KERNEL_PID_UNDEF;
}


/** @} */
