/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup net_ng_ndp
 * @{
 *
 * @file
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <string.h>

#include "byteorder.h"
#include "net/ng_icmpv6.h"
#include "net/ng_ipv6.h"
#include "net/ng_ipv6/ext/rh.h"
#include "net/ng_netbase.h"
#include "random.h"
#include "utlist.h"
#include "thread.h"
#include "vtimer.h"

#include "net/ng_ndp.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static ng_pktqueue_node_t _pkt_nodes[NG_IPV6_NC_SIZE * 2];
static ng_ipv6_nc_t *_last_router = NULL;   /* last router chosen as default
                                             * router. Only used if reachability
                                             * is suspect (i. e. incomplete or
                                             * not at all) */

/* random helper function */
static inline uint32_t _rand(uint32_t min, uint32_t max)
{
    return (genrand_uint32() % (max - min)) + min;
}

static bool _handle_sl2a_opt(kernel_pid_t iface, ng_pktsnip_t *pkt,
                             ng_ipv6_hdr_t *ipv6, uint8_t icmpv6_type,
                             ng_ndp_opt_t *sl2a_opt);
static bool _handle_tl2a_opt(kernel_pid_t iface, ng_pktsnip_t *pkt,
                             ng_ipv6_hdr_t *ipv6, uint8_t icmpv6_type,
                             ng_ndp_opt_t *tl2a_opt, ng_ipv6_addr_t *tgt,
                             uint8_t adv_flags);

/* send address resolution messages */
static void _send_nbr_sol(kernel_pid_t iface, ng_ipv6_addr_t *tgt,
                          ng_ipv6_addr_t *dst);
static void _send_nbr_adv(kernel_pid_t iface, ng_ipv6_addr_t *tgt,
                          ng_ipv6_addr_t *dst, bool supply_tl2a);

static void _set_state(ng_ipv6_nc_t *nc_entry, uint8_t state);

/* special netapi helper */
static inline void _send_delayed(vtimer_t *t, timex_t interval, ng_pktsnip_t *pkt)
{
    vtimer_set_msg(t, interval, ng_ipv6_pid, NG_NETAPI_MSG_TYPE_SND, pkt);
}

/* packet queue node allocation */
static ng_pktqueue_node_t *_alloc_pkt_node(ng_pktsnip_t *pkt);

void ng_ndp_nbr_sol_handle(kernel_pid_t iface, ng_pktsnip_t *pkt,
                           ng_ipv6_hdr_t *ipv6, ng_ndp_nbr_sol_t *nbr_sol,
                           size_t icmpv6_size)
{
    uint16_t opt_offset = 0;
    uint8_t *buf = ((uint8_t *)nbr_sol) + sizeof(ng_ndp_nbr_sol_t);
    ng_ipv6_addr_t *tgt;
    int sicmpv6_size = (int)icmpv6_size;

    /* check validity */
    if ((ipv6->hl != 255) || (nbr_sol->code != 0) ||
        (icmpv6_size < sizeof(ng_ndp_nbr_sol_t)) ||
        ng_ipv6_addr_is_multicast(&nbr_sol->tgt) ||
        (ng_ipv6_addr_is_unspecified(&ipv6->src) &&
         ng_ipv6_addr_is_solicited_node(&ipv6->dst))) {
        DEBUG("ndp: neighbor solicitation was invalid.\n");
        /* ipv6 releases */
        return;
    }

    if ((tgt = ng_ipv6_netif_find_addr(iface, &nbr_sol->tgt)) == NULL) {
        DEBUG("ndp: Target address is not to interface %" PRIkernel_pid "\n",
              iface);
        /* ipv6 releases */
        return;
    }

    sicmpv6_size -= sizeof(ng_ndp_nbr_sol_t);

    while (sicmpv6_size > 0) {
        ng_ndp_opt_t *opt = (ng_ndp_opt_t *)(buf + opt_offset);

        switch (opt->type) {
            case NG_NDP_OPT_SL2A:
                if (!_handle_sl2a_opt(iface, pkt, ipv6, nbr_sol->type, opt)) {
                    /* invalid source link-layer address option */
                    return;
                }

                break;

            default:
                /* silently discard all other options */
                break;
        }

        opt_offset += (opt->len * 8);
        sicmpv6_size -= (opt->len * 8);
    }

    _send_nbr_adv(iface, tgt, &ipv6->src,
                  ng_ipv6_addr_is_multicast(&ipv6->dst));

    return;
}

void ng_ndp_nbr_adv_handle(kernel_pid_t iface, ng_pktsnip_t *pkt,
                           ng_ipv6_hdr_t *ipv6, ng_ndp_nbr_adv_t *nbr_adv,
                           size_t icmpv6_size)
{
    uint16_t opt_offset = 0;
    uint8_t *buf = ((uint8_t *)nbr_adv) + sizeof(ng_ndp_nbr_adv_t);
    bool tl2a_supplied = false;
    int sicmpv6_size = (int)icmpv6_size;

    /* check validity */
    if ((ipv6->hl != 255) || (nbr_adv->code != 0) ||
        (icmpv6_size < sizeof(ng_ndp_nbr_adv_t)) ||
        ng_ipv6_addr_is_multicast(&nbr_adv->tgt)) {
        DEBUG("ndp: neighbor advertisement was invalid.\n");
        /* ipv6 releases */
        return;
    }

    if (ng_ipv6_nc_get(iface, &nbr_adv->tgt) == NULL) {
        DEBUG("ndp: no neighbor cache entry found for advertisement's target\n");
        /* ipv6 releases */
        return;
    }


    sicmpv6_size -= sizeof(ng_ndp_nbr_adv_t);

    while (sicmpv6_size > 0) {
        ng_ndp_opt_t *opt = (ng_ndp_opt_t *)(buf + opt_offset);

        switch (opt->type) {
            case NG_NDP_OPT_TL2A:
                if (!_handle_tl2a_opt(iface, pkt, ipv6, nbr_adv->type, opt,
                                      &nbr_adv->tgt, nbr_adv->flags)) {
                    /* invalid target link-layer address option */
                    return;
                }

                tl2a_supplied = true;

                break;

            default:
                /* silently discard all other options */
                break;
        }

        opt_offset += (opt->len * 8);
        sicmpv6_size -= (opt->len * 8);
    }

    if (!tl2a_supplied) {
        if (nbr_adv->flags & NG_NDP_NBR_ADV_FLAGS_O) {
            ng_ipv6_nc_t *nc_entry = ng_ipv6_nc_get(iface, &nbr_adv->tgt);

            if (nc_entry != NULL) {
                if (nbr_adv->flags & NG_NDP_NBR_ADV_FLAGS_S) {
                    _set_state(nc_entry, NG_IPV6_NC_STATE_REACHABLE);
                }
                else {
                    _set_state(nc_entry, NG_IPV6_NC_STATE_STALE);
                }
            }
        }
    }

    return;
}

void ng_ndp_retrans_nbr_sol(ng_ipv6_nc_t *nc_entry)
{
    if ((nc_entry->probes_remaining > 1) &&
        ((ng_ipv6_nc_get_state(nc_entry) == NG_IPV6_NC_STATE_INCOMPLETE) ||
         (ng_ipv6_nc_get_state(nc_entry) == NG_IPV6_NC_STATE_PROBE))) {
        ng_ipv6_addr_t dst;

        DEBUG("ndp: Retransmit neighbor solicitation for %s\n",
              ng_ipv6_addr_to_str(addr_str, nc_entry->ipv6_addr, sizeof(addr_str)));

        /* retransmit neighbor solicatation */
        if (ng_ipv6_nc_get_state(nc_entry) == NG_IPV6_NC_STATE_INCOMPLETE) {
            ng_ipv6_addr_set_solicited_nodes(&dst, &nc_entry->ipv6_addr);
        }
        else {
            dst.u64[0] = nc_entry->ipv6_addr.u64[0];
            dst.u64[1] = nc_entry->ipv6_addr.u64[1];
        }

        nc_entry->probes_remaining--;

        if (nc_entry->iface == KERNEL_PID_UNDEF) {
            timex_t t = { 0, NG_NDP_RETRANS_TIMER };
            kernel_pid_t *ifs;
            size_t ifnum;

            ifs = ng_netif_get(&ifnum);

            for (size_t i = 0; i < ifnum; i++) {
                _send_nbr_sol(ifs[i], &nc_entry->ipv6_addr, &dst);
            }

            vtimer_set_msg(&nc_entry->nbr_sol_timer, t, ng_ipv6_pid,
                           NG_NDP_MSG_NBR_SOL_RETRANS, nc_entry);
        }
        else {
            ng_ipv6_netif_t *ipv6_iface = ng_ipv6_netif_get(nc_entry->iface);

            _send_nbr_sol(nc_entry->iface, &nc_entry->ipv6_addr, &dst);

            mutex_lock(&ipv6_iface->mutex);
            vtimer_set_msg(&nc_entry->nbr_sol_timer,
                           ipv6_iface->retrans_timer, ng_ipv6_pid,
                           NG_NDP_MSG_NBR_SOL_RETRANS, nc_entry);
            mutex_unlock(&ipv6_iface->mutex);
        }
    }
    else if (nc_entry->probes_remaining <= 1) {
        ng_pktqueue_node_t *queue_node;

        /* No need to call ng_ipv6_nc_remove() we know already were the
         * entry is */

        DEBUG("ndp: Remove nc entry %s for interface %" PRIkernel_pid "\n",
              ng_ipv6_addr_to_str(addr_str, nc_entry->ipv6_addr, sizeof(addr_str)),
              nc_entry->iface);

        while ((queue_node = ng_pktqueue_remove_head(&nc_entry->pkts))) {
            ng_pktbuf_release(queue_node->data);
            queue_node->data = NULL;
        }

        ng_ipv6_addr_set_unspecified(&(nc_entry->ipv6_addr));
        nc_entry->iface = KERNEL_PID_UNDEF;
        nc_entry->flags = 0;
        nc_entry->probes_remaining = 0;
    }
}

void ng_ndp_state_timeout(ng_ipv6_nc_t *nc_entry)
{
    switch (ng_ipv6_nc_get_state(nc_entry)) {
        case NG_IPV6_NC_STATE_REACHABLE:
            _set_state(nc_entry, NG_IPV6_NC_STATE_STALE);
            break;

        case NG_IPV6_NC_STATE_DELAY:
            _set_state(nc_entry, NG_IPV6_NC_STATE_PROBE);
            break;

        default:
            break;
    }
}

void ng_ndp_netif_add(ng_ipv6_netif_t *iface)
{
    uint32_t reach_time = _rand(NG_NDP_MIN_RAND, NG_NDP_MAX_RAND);

    /* set default values */
    mutex_lock(&iface->mutex);
    iface->reach_time_base = NG_NDP_REACH_TIME;
    reach_time = (reach_time * iface->reach_time_base) / 10;
    iface->reach_time = timex_set(0, reach_time);
    timex_normalize(&iface->reach_time);
    iface->retrans_timer = timex_set(0, NG_NDP_RETRANS_TIMER);
    timex_normalize(&iface->retrans_timer);
    mutex_unlock(&iface->mutex);
}

void ng_ndp_netif_remove(ng_ipv6_netif_t *iface)
{
    /* TODO */
}

static ng_ipv6_addr_t *_default_router(void)
{
    ng_ipv6_nc_t *router = ng_ipv6_nc_get_next_router(NULL);

    /* first look if there is any reachable router */
    while (router != NULL) {
        if ((ng_ipv6_nc_get_state(router) != NG_IPV6_NC_STATE_INCOMPLETE) &&
            (ng_ipv6_nc_get_state(router) != NG_IPV6_NC_STATE_UNREACHABLE)) {
            _last_router = NULL;

            return &router->ipv6_addr;
        }

        router = ng_ipv6_nc_get_next_router(router);
    }

    /* else take the first one, but keep round-robin in further selections */
    router = ng_ipv6_nc_get_next_router(_last_router);

    if (router == NULL) {   /* end of router list or there is none => wrap around */
        router = ng_ipv6_nc_get_next_router(router);

        if (router == NULL) {   /* still nothing found => no router in list */
            return NULL;
        }
    }

    _last_router = router;

    return &router->ipv6_addr;
}

kernel_pid_t ng_ndp_next_hop_l2addr(uint8_t *l2addr, uint8_t *l2addr_len,
                                    kernel_pid_t iface, ng_ipv6_addr_t *dst,
                                    ng_pktsnip_t *pkt)
{
    ng_ipv6_addr_t *next_hop_ip = NULL, *prefix = NULL;
#ifdef MODULE_FIB
    size_t next_hop_size;
#endif

#ifdef MODULE_NG_IPV6_EXT_RH
    next_hop_ip = ng_ipv6_ext_rh_next_hop(hdr);
#endif
#ifdef MODULE_FIB
    if ((next_hop_ip == NULL) &&
        (fib_get_next_hop(&iface, (uint8_t *)next_hop_ip, &next_hop_size,
                          (uint8_t *)dst, sizeof(ng_ipv6_addr_t),
                          0) < 0) || (next_hop_ip != sizeof(ng_ipv6_addr_t))) {
        next_hop_ip = NULL;
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
        }
    }

    if (next_hop_ip == NULL) {
        next_hop_ip = _default_router();
    }

    if (next_hop_ip != NULL) {
        ng_ipv6_nc_t *nc_entry = ng_ipv6_nc_get(iface, next_hop_ip);

        if ((nc_entry != NULL) && ng_ipv6_nc_is_reachable(nc_entry)) {
            DEBUG("ndp: found reachable neigbor\n");

            if (ng_ipv6_nc_get_state(nc_entry) == NG_IPV6_NC_STATE_STALE) {
                _set_state(nc_entry, NG_IPV6_NC_STATE_DELAY);
            }

            memcpy(l2addr, nc_entry->l2_addr, nc_entry->l2_addr_len);
            *l2addr_len = nc_entry->l2_addr_len;
            /* TODO: unreachability check */
            return nc_entry->iface;
        }
        else if (nc_entry == NULL) {
            ng_pktqueue_node_t *pkt_node;
            ng_ipv6_addr_t dst_sol;

            nc_entry = ng_ipv6_nc_add(iface, next_hop_ip, NULL, 0,
                                      NG_IPV6_NC_STATE_INCOMPLETE << NG_IPV6_NC_STATE_POS);

            if (nc_entry == NULL) {
                DEBUG("ndp: could not create neighbor cache entry\n");
                return KERNEL_PID_UNDEF;
            }

            pkt_node = _alloc_pkt_node(pkt);

            if (pkt_node == NULL) {
                DEBUG("ndp: could not add packet to packet queue\n");
            }
            else {
                /* prevent packet from being released by IPv6 */
                ng_pktbuf_hold(pkt_node->data, 1);
                ng_pktqueue_add(&nc_entry->pkts, pkt_node);
            }

            /* address resolution */
            ng_ipv6_addr_set_solicited_nodes(&dst_sol, next_hop_ip);

            if (iface == KERNEL_PID_UNDEF) {
                timex_t t = { 0, NG_NDP_RETRANS_TIMER };
                kernel_pid_t *ifs;
                size_t ifnum;

                ifs = ng_netif_get(&ifnum);

                for (size_t i = 0; i < ifnum; i++) {
                    _send_nbr_sol(ifs[i], next_hop_ip, &dst_sol);
                }

                vtimer_set_msg(&nc_entry->nbr_sol_timer, t, ng_ipv6_pid,
                               NG_NDP_MSG_NBR_SOL_RETRANS, nc_entry);
            }
            else {
                ng_ipv6_netif_t *ipv6_iface = ng_ipv6_netif_get(iface);

                _send_nbr_sol(iface, next_hop_ip, &dst_sol);

                mutex_lock(&ipv6_iface->mutex);
                vtimer_set_msg(&nc_entry->nbr_sol_timer,
                               ipv6_iface->retrans_timer, ng_ipv6_pid,
                               NG_NDP_MSG_NBR_SOL_RETRANS, nc_entry);
                mutex_unlock(&ipv6_iface->mutex);
            }
        }
    }

    return KERNEL_PID_UNDEF;
}

ng_pktsnip_t *ng_ndp_nbr_sol_build(ng_ipv6_addr_t *tgt, ng_pktsnip_t *options)
{
    ng_pktsnip_t *pkt;
    ng_ndp_nbr_sol_t *nbr_sol;

    DEBUG("ndp: building neighbor solicitation message\n");

    if (ng_ipv6_addr_is_multicast(tgt)) {
        DEBUG("ndp: tgt must not be multicast\n");
        return NULL;
    }

    pkt = ng_icmpv6_build(options, NG_ICMPV6_NBR_SOL, 0, sizeof(ng_ndp_nbr_sol_t));

    if (pkt != NULL) {
        nbr_sol = pkt->data;
        nbr_sol->resv.u32 = 0;
        memcpy(&nbr_sol->tgt, tgt, sizeof(ng_ipv6_addr_t));
    }

    return pkt;
}

ng_pktsnip_t *ng_ndp_nbr_adv_build(uint8_t flags, ng_ipv6_addr_t *tgt,
                                   ng_pktsnip_t *options)
{
    ng_pktsnip_t *pkt;
    ng_ndp_nbr_adv_t *nbr_adv;

    DEBUG("ndp: building neighbor advertisement message\n");

    if (ng_ipv6_addr_is_multicast(tgt)) {
        DEBUG("ndp: tgt must not be multicast\n");
        return NULL;
    }

    pkt = ng_icmpv6_build(options, NG_ICMPV6_NBR_ADV, 0, sizeof(ng_ndp_nbr_adv_t));

    if (pkt == NULL) {
        return NULL;
    }

    nbr_adv = pkt->data;
    nbr_adv->flags = (flags & NG_NDP_NBR_ADV_FLAGS_MASK);
    nbr_adv->resv[0] = nbr_adv->resv[1] = nbr_adv->resv[2] = 0;
    memcpy(&nbr_adv->tgt, tgt, sizeof(ng_ipv6_addr_t));

    return pkt;
}

static inline size_t _ceil8(uint8_t length)
{
    /* NDP options use units of 8 byte for there length field, so round up */
    return (length + 7U) & 0xf8U;
}

ng_pktsnip_t *ng_ndp_opt_build(uint8_t type, size_t size, ng_pktsnip_t *next)
{
    ng_ndp_opt_t *opt;
    ng_pktsnip_t *pkt = ng_pktbuf_add(next, NULL, _ceil8(size), NG_NETTYPE_UNDEF);

    if (pkt == NULL) {
        DEBUG("ndp: no space left in packet buffer\n");
        return NULL;
    }

    opt = pkt->data;

    opt->type = type;
    opt->len = (uint8_t)(pkt->size / 8);

    return pkt;
}

static uint16_t _get_l2src(uint8_t *l2src, size_t l2src_size, kernel_pid_t iface)
{
    bool try_long = false;
    int res;
    uint16_t l2src_len;

    /* try getting source address */
    if ((ng_netapi_get(iface, NETCONF_OPT_SRC_LEN, 0, &l2src_len,
                       sizeof(l2src_len)) >= 0) &&
        (l2src_len == 8)) {
        try_long = true;
    }

    if ((try_long && ((res = ng_netapi_get(iface, NETCONF_OPT_ADDRESS_LONG, 0,
                                           l2src, l2src_size)) < 0)) ||
        ((res = ng_netapi_get(iface, NETCONF_OPT_ADDRESS, 0, l2src,
                              l2src_size)) < 0)) {
        DEBUG("ndp: no link-layer address found.\n");
        l2src_len = 0;
    }
    else {
        l2src_len = (uint16_t)res;
    }

    return l2src_len;
}

static void _send_nbr_sol(kernel_pid_t iface, ng_ipv6_addr_t *tgt,
                          ng_ipv6_addr_t *dst)
{
    ng_pktsnip_t *hdr, *pkt = NULL;
    ng_ipv6_addr_t *src = NULL;
    size_t src_len = 0;
    uint8_t l2src[8];
    uint16_t l2src_len;

    /* check if there is a fitting source address to target */
    if ((src = ng_ipv6_netif_find_best_src_addr(iface, tgt)) != NULL) {
        src_len = sizeof(ng_ipv6_addr_t);
        l2src_len = _get_l2src(l2src, sizeof(l2src), iface);

        if (l2src_len > 0) {
            /* add source address link-layer address option */
            pkt = ng_ndp_opt_sl2a_build(l2src, l2src_len, NULL);

            if (pkt == NULL) {
                DEBUG("ndp: error allocating Source Link-layer address option.\n");
                ng_pktbuf_release(pkt);
                return;
            }
        }
    }

    hdr = ng_ndp_nbr_sol_build(tgt, pkt);

    if (hdr == NULL) {
        DEBUG("ndp: error allocating Neighbor solicitation.\n");
        ng_pktbuf_release(pkt);
        return;
    }

    pkt = hdr;
    hdr = ng_ipv6_hdr_build(pkt, (uint8_t *)src, src_len, (uint8_t *)dst,
                            sizeof(ng_ipv6_addr_t));

    if (hdr == NULL) {
        DEBUG("ndp: error allocating IPv6 header.\n");
        ng_pktbuf_release(pkt);
        return;
    }

    ((ng_ipv6_hdr_t *)hdr->data)->hl = 255;

    pkt = hdr;
    /* add netif header for send interface specification */
    hdr = ng_netif_hdr_build(NULL, 0, NULL, 0);

    if (hdr == NULL) {
        DEBUG("ndp: error allocating netif header.\n");
        return;
    }

    LL_PREPEND(pkt, hdr);

    ((ng_netif_hdr_t *)hdr->data)->if_pid = iface;

    ng_netapi_send(ng_ipv6_pid, pkt);
}

static void _send_nbr_adv(kernel_pid_t iface, ng_ipv6_addr_t *tgt,
                          ng_ipv6_addr_t *dst, bool supply_tl2a)
{
    ng_pktsnip_t *hdr, *pkt = NULL;
    uint8_t l2src[8];
    uint16_t l2src_len;
    uint8_t adv_flags = 0;

    if (ng_ipv6_netif_get(iface)->flags & NG_IPV6_NETIF_FLAGS_ROUTER) {
        adv_flags |= NG_NDP_NBR_ADV_FLAGS_R;
    }

    if (ng_ipv6_addr_is_unspecified(dst)) {
        ng_ipv6_addr_set_all_nodes_multicast(dst,
                                             NG_IPV6_ADDR_MCAST_SCP_LINK_LOCAL);
    }
    else {
        adv_flags |= NG_NDP_NBR_ADV_FLAGS_S;
    }

    if (supply_tl2a) {
        /* we previously checked if we are the target, so we can take our L2src */
        l2src_len = _get_l2src(l2src, sizeof(l2src), iface);

        if (l2src_len > 0) {
            /* add target address link-layer address option */
            pkt = ng_ndp_opt_tl2a_build(l2src, l2src_len, NULL);

            if (pkt == NULL) {
                DEBUG("ndp: error allocating Target Link-layer address option.\n");
                ng_pktbuf_release(pkt);
                return;
            }
        }
    }

    /* TODO: also check if the node provides proxy servies for tgt */
    if ((pkt != NULL) && !ng_ipv6_netif_addr_is_non_unicast(tgt)) {
        /* TL2A is not supplied and tgt is not anycast */
        adv_flags |= NG_NDP_NBR_ADV_FLAGS_O;
    }

    hdr = ng_ndp_nbr_adv_build(adv_flags, tgt, pkt);

    if (hdr == NULL) {
        DEBUG("ndp: error allocating Neighbor advertisement.\n");
        ng_pktbuf_release(pkt);
        return;
    }

    pkt = hdr;
    hdr = ng_ipv6_hdr_build(pkt, NULL, 0, (uint8_t *)dst,
                            sizeof(ng_ipv6_addr_t));

    if (hdr == NULL) {
        DEBUG("ndp: error allocating IPv6 header.\n");
        ng_pktbuf_release(pkt);
        return;
    }

    ((ng_ipv6_hdr_t *)hdr->data)->hl = 255;

    pkt = hdr;
    /* add netif header for send interface specification */
    hdr = ng_netif_hdr_build(NULL, 0, NULL, 0);

    if (hdr == NULL) {
        DEBUG("ndp: error allocating netif header.\n");
        return;
    }

    LL_PREPEND(pkt, hdr);

    ((ng_netif_hdr_t *)hdr->data)->if_pid = iface;

    if (ng_ipv6_netif_addr_is_non_unicast(tgt)) {
        /* avoid collision for anycast addresses */
        timex_t delay = { _rand(0, NG_NDP_MAX_AC_TGT_DELAY), 0 };
        ng_ipv6_nc_t *nc_entry = ng_ipv6_nc_get(iface, tgt);

        /* nc_entry must be set so no need to check it */
        _send_delayed(&nc_entry->nbr_adv_timer, delay, pkt);
    }
    else {
        ng_netapi_send(ng_ipv6_pid, pkt);
    }
}

static inline ng_pktsnip_t *_opt_l2a_build(uint8_t type, const uint8_t *l2addr,
                                           uint8_t l2addr_len, ng_pktsnip_t *next)
{
    ng_pktsnip_t *pkt = ng_ndp_opt_build(type, sizeof(ng_ndp_opt_t) + l2addr_len,
                                         next);

    if (pkt != NULL) {
        ng_ndp_opt_t *l2a_opt = pkt->data;

        memset(l2a_opt + 1, 0, pkt->size - sizeof(ng_ndp_opt_t));
        memcpy(l2a_opt + 1, l2addr, l2addr_len);
    }

    return pkt;
}

ng_pktsnip_t *ng_ndp_opt_sl2a_build(const uint8_t *l2addr, uint8_t l2addr_len,
                                    ng_pktsnip_t *next)
{
    DEBUG("ndp: building source link-layer address option\n");

    return _opt_l2a_build(NG_NDP_OPT_SL2A, l2addr, l2addr_len, next);
}

ng_pktsnip_t *ng_ndp_opt_tl2a_build(const uint8_t *l2addr, uint8_t l2addr_len,
                                    ng_pktsnip_t *next)
{
    DEBUG("ndp: building target link-layer address option\n");

    return _opt_l2a_build(NG_NDP_OPT_TL2A, l2addr, l2addr_len, next);
}

/* internal functions */
/* packet queue node allocation */
static ng_pktqueue_node_t *_alloc_pkt_node(ng_pktsnip_t *pkt)
{
    for (size_t i = 0; i < sizeof(_pkt_nodes); i++) {
        if (_pkt_nodes[i].data == NULL) {
            ng_pktqueue_node_init(_pkt_nodes + i);
            _pkt_nodes[i].data = pkt;

            return &(_pkt_nodes[i]);
        }
    }

    return NULL;
}

static bool _handle_sl2a_opt(kernel_pid_t iface, ng_pktsnip_t *pkt,
                             ng_ipv6_hdr_t *ipv6, uint8_t icmpv6_type,
                             ng_ndp_opt_t *sl2a_opt)
{
    ng_ipv6_nc_t *nc_entry = NULL;
    uint8_t sl2a_len = 0;
    uint8_t *sl2a = (uint8_t *)(sl2a_opt + 1);

    if ((sl2a_opt->len == 0) || ng_ipv6_addr_is_unspecified(&ipv6->src)) {
        DEBUG("ndp: invalid source link-layer address option received\n");
        return false;
    }

    while (pkt) {
        if (pkt->type == NG_NETTYPE_NETIF) {
            ng_netif_hdr_t *hdr = pkt->data;
            sl2a_len = hdr->src_l2addr_len;
            break;
        }
        pkt = pkt->next;
    }

    if (sl2a_len == 0) {  /* in case there was no source address in l2 */
        sl2a_len = (sl2a_opt->len / 8) - sizeof(ng_ndp_opt_t);

        /* ignore all zeroes at the end for length */
        for (; sl2a[sl2a_len - 1] == 0x00; sl2a_len--);
    }

    switch (icmpv6_type) {
        case NG_ICMPV6_NBR_SOL:
            nc_entry = ng_ipv6_nc_get(iface, &ipv6->src);

            if (nc_entry != NULL) {
                if ((sl2a_len != nc_entry->l2_addr_len) ||
                    (memcmp(sl2a, nc_entry->l2_addr, sl2a_len) != 0)) {
                    /* if entry exists but l2 address differs: set */
                    nc_entry->l2_addr_len = sl2a_len;
                    memcpy(nc_entry->l2_addr, sl2a, sl2a_len);

                    _set_state(nc_entry, NG_IPV6_NC_STATE_STALE);
                }
            }
            else {
                ng_ipv6_nc_add(iface, &ipv6->src, sl2a, sl2a_len,
                               NG_IPV6_NC_STATE_STALE);
            }

            return true;

        default:    /* wrong encapsulating message: silently discard */
            DEBUG("ndp: silently discard sl2a_opt for ICMPv6 message type %"
                  PRIu8 "\n", icmpv6_type);
            return true;
    }
}

static bool _handle_tl2a_opt(kernel_pid_t iface, ng_pktsnip_t *pkt,
                             ng_ipv6_hdr_t *ipv6, uint8_t icmpv6_type,
                             ng_ndp_opt_t *tl2a_opt, ng_ipv6_addr_t *tgt,
                             uint8_t adv_flags)
{
    ng_ipv6_nc_t *nc_entry = NULL;
    uint8_t tl2a_len = 0;
    uint8_t *tl2a = (uint8_t *)(tl2a_opt + 1);

    if ((tl2a_opt->len == 0) || ng_ipv6_addr_is_unspecified(&ipv6->src)) {
        DEBUG("ndp: invalid target link-layer address option received\n");
        return false;
    }

    while (pkt) {
        if (pkt->type == NG_NETTYPE_NETIF) {
            ng_netif_hdr_t *hdr = pkt->data;
            tl2a_len = hdr->src_l2addr_len;
            break;
        }
        pkt = pkt->next;
    }

    if (tl2a_len == 0) {  /* in case there was no source address in l2 */
        tl2a_len = (tl2a_opt->len / 8) - sizeof(ng_ndp_opt_t);

        /* ignore all zeroes at the end for length */
        for (; tl2a[tl2a_len - 1] == 0x00; tl2a_len--);
    }

    switch (icmpv6_type) {
        case NG_ICMPV6_NBR_ADV:
            nc_entry = ng_ipv6_nc_get(iface, tgt);

            /* no need to create an entry in the negative case (see RFC 4861) */
            if (nc_entry != NULL) {
                nc_entry->l2_addr_len = tl2a_len;

                if (tl2a_len > 0) {
                    memcpy(nc_entry->l2_addr, tl2a, tl2a_len);
                }

                if (ng_ipv6_nc_get_state(nc_entry) == NG_IPV6_NC_STATE_INCOMPLETE) {
                    ng_pktqueue_node_t *queued_pkt;

                    if (adv_flags & NG_NDP_NBR_ADV_FLAGS_S) {
                        _set_state(nc_entry, NG_IPV6_NC_STATE_REACHABLE);
                    }
                    else {
                        _set_state(nc_entry, NG_IPV6_NC_STATE_STALE);
                    }

                    if (adv_flags & NG_NDP_NBR_ADV_FLAGS_R) {
                        nc_entry->flags |= NG_IPV6_NC_IS_ROUTER;
                    }
                    else {
                        nc_entry->flags &= ~NG_IPV6_NC_IS_ROUTER;
                    }

                    while ((queued_pkt = ng_pktqueue_remove_head(&nc_entry->pkts)) != NULL) {
                        ng_netapi_send(ng_ipv6_pid, queued_pkt->data);
                        queued_pkt->data = NULL;
                    }
                }
                else {
                    if (memcmp(tl2a, nc_entry->l2_addr, tl2a_len) != 0) {
                        if ((adv_flags & NG_NDP_NBR_ADV_FLAGS_O)) {
                            memcpy(nc_entry->l2_addr, tl2a, tl2a_len);
                        }
                        else if (ng_ipv6_nc_get_state(nc_entry) == NG_IPV6_NC_STATE_REACHABLE) {
                            _set_state(nc_entry, NG_IPV6_NC_STATE_STALE);
                        }
                    }

                    if ((adv_flags & NG_NDP_NBR_ADV_FLAGS_O)) {
                        if (adv_flags & NG_NDP_NBR_ADV_FLAGS_S) {
                            _set_state(nc_entry, NG_IPV6_NC_STATE_REACHABLE);
                        }
                        else {
                            _set_state(nc_entry, NG_IPV6_NC_STATE_STALE);
                        }

                        if (adv_flags & NG_NDP_NBR_ADV_FLAGS_R) {
                            nc_entry->flags |= NG_IPV6_NC_IS_ROUTER;
                        }
                        else {
                            nc_entry->flags &= ~NG_IPV6_NC_IS_ROUTER;
                        }
                    }
                }
            }

            return true;

        default:    /* wrong encapsulating message: silently discard */
            DEBUG("ndp: silently discard tl2a_opt for ICMPv6 message type %"
                  PRIu8 "\n", icmpv6_type);
            return true;
    }
}

static void _set_state(ng_ipv6_nc_t *nc_entry, uint8_t state)
{
    ng_ipv6_netif_t *ipv6_iface;
    timex_t t = { NG_NDP_FIRST_PROBE_DELAY, 0 };

    nc_entry->flags &= ~NG_IPV6_NC_STATE_MASK;
    nc_entry->flags |= state;

    switch (state) {
        case NG_IPV6_NC_STATE_REACHABLE:
            ipv6_iface = ng_ipv6_netif_get(nc_entry->iface);
            t = ipv6_iface->reach_time;
            vtimer_remove(&nc_entry->nbr_sol_timer);

        case NG_IPV6_NC_STATE_DELAY:
            vtimer_set_msg(&nc_entry->nbr_sol_timer, t, ng_ipv6_pid,
                           NG_NDP_MSG_NC_STATE_TIMEOUT, nc_entry);
            break;

        case NG_IPV6_NC_STATE_PROBE:
            ipv6_iface = ng_ipv6_netif_get(nc_entry->iface);

            nc_entry->probes_remaining = NG_NDP_MAX_UC_NBR_SOL_NUMOF;
            _send_nbr_sol(nc_entry->iface, &nc_entry->ipv6_addr,
                          &nc_entry->ipv6_addr);

            mutex_lock(&ipv6_iface->mutex);
            vtimer_set_msg(&nc_entry->nbr_sol_timer,
                           ipv6_iface->retrans_timer, ng_ipv6_pid,
                           NG_NDP_MSG_NBR_SOL_RETRANS, nc_entry);
            mutex_unlock(&ipv6_iface->mutex);
            break;

        default:
            break;
    }
}

/**
 * @}
 */
