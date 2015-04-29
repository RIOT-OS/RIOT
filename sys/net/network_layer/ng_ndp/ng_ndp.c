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
#include "net/ng_netbase.h"
#include "net/ng_netif/hdr.h"
#include "net/ng_pktbuf.h"
#include "random.h"
#include "utlist.h"
#include "vtimer.h"

#include "net/ng_ndp.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* Despite suggestion in RFC 4861 we do not store deterimined next hops
 * here, just the information learned through redirects */
typedef struct {
    kernel_pid_t iface;         /* The interface for the destination */
    ng_ipv6_addr_t dst;         /* A destination address */
    ng_ipv6_addr_t next_hop;    /* next hop for destination */
} _dst_cache_t;

static _dst_cache_t _dst_cache[NG_NDP_DST_CACHE_SIZE];
static uint8_t _dst_cache_idx = 0;
static ng_pktqueue_node_t _pkt_nodes[NG_IPV6_NC_SIZE * 2];
static ng_ipv6_nc_t *_last_router = NULL;   /* last router chosen as default
                                             * router. Only used if reachability
                                             * is suspect (i. e. incomplete or
                                             * not at all) */

/* destination_cache functions */
static void _dst_cache_add(kernel_pid_t iface, ng_ipv6_addr_t *dst,
                           ng_ipv6_addr_t *next_hop);
static kernel_pid_t _dst_cache_get_next_hop(ng_ipv6_addr_t **next_hop,
        const ng_ipv6_addr_t *dst);

/* packet queue node allocation */
static ng_pktqueue_node_t *_alloc_pkt_node(ng_pktsnip_t *pkt);
static inline void _free_pkt_node(ng_pktqueue_node_t *node);

/* random helper function */
static inline uint32_t _rand(uint32_t min, uint32_t max)
{
    return (genrand_uint32() % (max - min)) + min;
}

/* receive handlers for messages */
static void _handle_rtr_adv(kernel_pid_t iface, ng_pktsnip_t *pkt,
                            ng_ipv6_hdr_t *ipv6, ng_ndp_rtr_adv_t *rtr_adv,
                            size_t icmpv6_size);
static void _handle_nbr_sol(kernel_pid_t iface, ng_pktsnip_t *pkt,
                            ng_ipv6_hdr_t *ipv6, ng_ndp_nbr_sol_t *nbr_sol,
                            size_t icmpv6_size);
static void _handle_nbr_adv(kernel_pid_t iface, ng_pktsnip_t *pkt,
                            ng_ipv6_hdr_t *ipv6, ng_ndp_nbr_adv_t *nbr_adv,
                            size_t icmpv6_size);
static void _handle_redirect(kernel_pid_t iface, ng_pktsnip_t *pkt,
                             ng_ipv6_hdr_t *ipv6, ng_ndp_redirect_t *redirect,
                             size_t icmpv6_size);
/* router only functions */
#ifdef MODULE_NG_IPV6_ROUTER
static void _handle_rtr_sol(kernel_pid_t iface, ng_pktsnip_t *pkt,
                            ng_ipv6_hdr_t *ipv6, ng_ndp_rtr_sol_t *rtr_sol,
                            size_t icmpv6_size);
#else  /* MODULE_NG_IPV6_ROUTER */
/* non-routers must silently discard */
#define _handle_rtr_sol(iface, pkt, ipv6, rtr_sol, icmpv6_size)
#endif /* MODULE_NG_IPV6_ROUTER */

static bool _handle_sl2a_opt(kernel_pid_t iface, ng_pktsnip_t *pkt,
                             ng_ipv6_hdr_t *ipv6, uint8_t icmpv6_type,
                             ng_ndp_opt_t *sl2a_opt);
static ng_ipv6_nc_t *_handle_tl2a_opt(kernel_pid_t iface, ng_pktsnip_t *pkt,
                                      ng_ipv6_hdr_t *ipv6, uint8_t icmpv6_type,
                                      ng_ndp_opt_t *tl2a_opt);
static bool _handle_pi_opt(kernel_pid_t iface, ng_pktsnip_t *pkt,
                           ng_ipv6_hdr_t *ipv6, uint8_t icmpv6_type,
                           ng_ndp_opt_pi_t *pi_opt);
static bool _handle_rh_opt(kernel_pid_t iface, ng_pktsnip_t *pkt,
                           ng_ipv6_hdr_t *ipv6, uint8_t icmpv6_type,
                           ng_ndp_opt_rh_t *rh_opt);
static bool _handle_mtu_opt(kernel_pid_t iface, ng_pktsnip_t *pkt,
                            ng_ipv6_hdr_t *ipv6, uint8_t icmpv6_type,
                            ng_ndp_opt_mtu_t *mtu_opt);
static ng_pktsnip_t *_iface_rtr_sol_build(ng_ipv6_netif_t *iface);

void ng_ndp_demux(kernel_pid_t iface, ng_pktsnip_t *pkt, ng_ipv6_hdr_t *ipv6,
                  ng_icmpv6_hdr_t *icmpv6, size_t icmpv6_size)
{
    switch (icmpv6->type) {
        case NG_ICMPV6_RTR_SOL:
            _handle_rtr_sol(iface, pkt, ipv6, (ng_ndp_rtr_sol_t *)icmpv6,
                            icmpv6_size);
            return;

        case NG_ICMPV6_RTR_ADV:
            _handle_rtr_adv(iface, pkt, ipv6, (ng_ndp_rtr_adv_t *)icmpv6,
                            icmpv6_size);
            return;

        case NG_ICMPV6_NBR_SOL:
            _handle_nbr_sol(iface, pkt, ipv6, (ng_ndp_nbr_sol_t *)icmpv6,
                            icmpv6_size);
            return;

        case NG_ICMPV6_NBR_ADV:
            _handle_nbr_adv(iface, pkt, ipv6, (ng_ndp_nbr_adv_t *)icmpv6,
                            icmpv6_size);
            return;

        case NG_ICMPV6_REDIRECT:
            _handle_redirect(iface, pkt, ipv6, (ng_ndp_redirect_t *)icmpv6,
                             icmpv6_size);
            return;

        default:
            DEBUG("ndp: Message of type %" PRIu8 " is unknown to NDP\n",
                  icmpv6->type);
            return;
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

#ifdef MODULE_NG_IPV6_ROUTER
    ng_ndp_netif_add_router(iface);
#else
    ng_pktsnip_t *hdr, *pkt;
    ng_ipv6_addr_t *src, dst = NG_IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL;

    pkt = _iface_rtr_sol_build(iface);

    if (pkt != NULL) {
        return;
    }

    if ((src = ng_ipv6_netif_find_best_src_addr(iface->pid, &dst)) == NULL) {
        hdr = ng_ipv6_hdr_build(pkt, NULL, 0, (uint8_t *)&dst,
                                (uint8_t)sizeof(ng_ipv6_addr_t));
    }
    else {
        hdr = ng_ipv6_hdr_build(pkt, (uint8_t *)src,
                                (uint8_t)sizeof(ng_ipv6_addr_t),
                                (uint8_t *)&dst,
                                (uint8_t)sizeof(ng_ipv6_addr_t));
    }

    if (hdr == NULL) {
        DEBUG("ndp: no space left in packet buffer\n");
        ng_pktbuf_release(pkt);
        return;
    }

    hdr = ng_netif_hdr_build(NULL, 0, NULL, 0);

    if (hdr == NULL) {
        DEBUG("ndp: no space left in packet buffer\n");
        ng_pktbuf_release(pkt);
        return;
    }

    ((ng_netif_hdr_t *)hdr->data)->if_pid = iface->pid;

    LL_PREPEND(pkt, hdr);

    ng_netapi_send(thread_getpid(), pkt);
#endif
}

void ng_ndp_netif_remove(ng_ipv6_netif_t *iface)
{
    ng_ndp_netif_rtr_adv(iface, false);
#ifdef MODULE_NG_IPV6_ROUTER
    ng_ndp_netif_remove_router(iface);
#else
    /* TODO ? */
#endif
}

static ng_ipv6_addr_t *_default_router(void)
{
    ng_ipv6_nc_t *router = ng_ipv6_nc_get_next_router(NULL);

    /* first look if there is any reachable router */
    while (router != NULL) {
        if (!(router->flags & (NG_IPV6_NC_STATE_INCOMPLETE | NG_IPV6_NC_STATE_UNREACHABLE))) {
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
                                    const ng_ipv6_addr_t *dst,
                                    ng_pktsnip_t *pkt)
{
    ng_ipv6_addr_t *next_hop_ip = NULL;
    kernel_pid_t iface = _dst_cache_get_next_hop(&next_hop_ip, dst);

    if (iface == KERNEL_PID_UNDEF) {
        iface = ng_ipv6_netif_find_by_prefix(&next_hop_ip, dst);

        if ((iface != KERNEL_PID_UNDEF) &&
            (ng_ipv6_netif_addr_get(next_hop_ip)->flags &
             NG_IPV6_NETIF_ADDR_FLAGS_NDP_ON_LINK)) {
            next_hop_ip = (ng_ipv6_addr_t *)dst;
        }

#ifdef MODULE_NG_IPV6_ROUTER
        else {
            /* TODO search in FIB */
        }
#endif

    }

    if (next_hop_ip == NULL) {
        next_hop_ip = _default_router();
    }

    if (next_hop_ip != NULL) {
        ng_ipv6_nc_t *nc_entry = ng_ipv6_nc_get(iface, next_hop_ip);

        if (nc_entry != NULL && ng_ipv6_nc_is_reachable(nc_entry)) {
            DEBUG("ndp: found reachable neigbor\n");

            memcpy(l2addr, nc_entry->l2_addr, nc_entry->l2_addr_len);
            *l2addr_len = nc_entry->l2_addr_len;
            /* TODO: unreachability check */
            return nc_entry->iface;
        }
        else if (nc_entry == NULL) {
            ng_pktqueue_node_t *pkt_node;

            nc_entry = ng_ipv6_nc_add(iface, next_hop_ip, NULL, 0,
                                      NG_IPV6_NC_STATE_INCOMPLETE << NG_IPV6_NC_STATE_POS);

            if (nc_entry == NULL) {
                DEBUG("ndp: could not create neighbor cache entry\n");
                return KERNEL_PID_UNDEF;
            }

            pkt_node = _alloc_pkt_node(pkt);

            if (pkt_node == NULL) {
                DEBUG("ndp: could not add packet to packet queue")
            }
            else {
                ng_pktqueue_add(&nc_entry->pkts, pkt_node);
            }

            /* TODO: address resolution */
        }
    }

    return KERNEL_PID_UNDEF;
}

ng_pktsnip_t *ng_ndp_rtr_sol_build(ng_pktsnip_t *options)
{
    ng_pktsnip_t *pkt;
    ng_ndp_rtr_sol_t *rtr_sol;

    DEBUG("ndp: building router solicitation message\n");

    pkt = ng_pktbuf_add(options, NULL, sizeof(ng_ndp_rtr_sol_t),
                        NG_NETTYPE_ICMPV6);

    if (pkt == NULL) {
        DEBUG("ndp: no space left in packet buffer\n");
        return NULL;
    }

    rtr_sol = pkt->data;
    rtr_sol->type = NG_ICMPV6_RTR_SOL;
    rtr_sol->code = 0;
    rtr_sol->resv = 0;

    return pkt;
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

    pkt = ng_pktbuf_add(options, NULL, sizeof(ng_ndp_nbr_sol_t),
                        NG_NETTYPE_ICMPV6);

    if (pkt == NULL) {
        DEBUG("ndp: no space left in packet buffer\n");
        return NULL;
    }

    nbr_sol = pkt->data;
    nbr_sol->type = NG_ICMPV6_NBR_SOL;
    nbr_sol->code = 0;
    nbr_sol->resv = 0;
    memcpy(&nbr_sol->tgt, tgt, sizeof(ng_ipv6_addr_t));

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

    pkt = ng_pktbuf_add(options, NULL, sizeof(ng_ndp_nbr_adv_t),
                        NG_NETTYPE_ICMPV6);

    if (pkt == NULL) {
        DEBUG("ndp: no space left in packet buffer\n");
        return NULL;
    }

    nbr_adv = pkt->data;
    nbr_adv->type = NG_ICMPV6_NBR_ADV;
    nbr_adv->code = 0;
    nbr_adv->flags = (flags & NG_NDP_NBR_ADV_FLAGS_MASK);
    nbr_adv->resv[0] = nbr_adv->resv[1] = nbr_adv->resv[2] = 0;
    memcpy(&nbr_adv->tgt, tgt, sizeof(ng_ipv6_addr_t));

    return pkt;
}

ng_pktsnip_t *ng_ndp_redirect_build(ng_ipv6_addr_t *tgt, ng_ipv6_addr_t *dst,
                                    ng_pktsnip_t *options)
{
    ng_pktsnip_t *pkt;
    ng_ndp_redirect_t *redirect;

    DEBUG("ndp: building redirect message\n");

    pkt = ng_pktbuf_add(options, NULL, sizeof(ng_ndp_redirect_t),
                        NG_NETTYPE_ICMPV6);

    if (pkt == NULL) {
        DEBUG("ndp: no space left in packet buffer\n");
        return NULL;
    }

    redirect = pkt->data;
    redirect->type = NG_ICMPV6_REDIRECT;
    redirect->code = 0;
    redirect->resv = 0;
    memcpy(&redirect->tgt, tgt, sizeof(ng_ipv6_addr_t));
    memcpy(&redirect->dst, dst, sizeof(ng_ipv6_addr_t));

    return pkt;
}

static inline size_t _ceil8(uint8_t length)
{
    /* NDP options use units of 8 byte for there length field, so round up */
    return (length + 7U) & 0xf8U;
}

static ng_pktsnip_t *_opt_l2a_build(uint8_t type, const uint8_t *l2addr,
                                    uint8_t l2addr_len, ng_pktsnip_t *more)
{
    ng_ndp_opt_t *l2a_opt;
    ng_pktsnip_t *pkt = ng_pktbuf_add(more, NULL,
                                      _ceil8(sizeof(ng_ndp_opt_t) + l2addr_len),
                                      NG_NETTYPE_UNDEF);

    if (pkt == NULL) {
        DEBUG("ndp: no space left in packet buffer\n");
        return NULL;
    }

    l2a_opt = pkt->data;
    l2a_opt->type = type;
    l2a_opt->len = (uint8_t)(pkt->size / 8);
    memset(l2a_opt + 1, 0, pkt->size - sizeof(ng_ndp_opt_t));
    memcpy(l2a_opt + 1, l2addr, l2addr_len);

    return pkt;
}

ng_pktsnip_t *ng_ndp_opt_sl2a_build(const uint8_t *l2addr, uint8_t l2addr_len,
                                    ng_pktsnip_t *more)
{
    DEBUG("ndp: building source link-layer address option\n");

    return _opt_l2a_build(NG_NDP_OPT_SL2A, l2addr, l2addr_len, more);
}

ng_pktsnip_t *ng_ndp_opt_tl2a_build(const uint8_t *l2addr, uint8_t l2addr_len,
                                    ng_pktsnip_t *more)
{
    DEBUG("ndp: building target link-layer address option\n");

    return _opt_l2a_build(NG_NDP_OPT_TL2A, l2addr, l2addr_len, more);
}

ng_pktsnip_t *ng_ndp_opt_rh_build(ng_pktsnip_t *data, ng_pktsnip_t *more)
{
    ng_ndp_opt_rh_t *rh_opt;
    ng_pktsnip_t *pkt;
    ng_ipv6_netif_t *if_ent;
    size_t size;

    DEBUG("ndp: building redirected header option\n");

    if ((data == NULL) || (data->type != NG_NETTYPE_IPV6) ||
        (data->next == NULL) || (data->next->type != NG_NETTYPE_NETIF)) {
        DEBUG("ndp: data was no IPv6 packet.\n");
        return NULL;
    }

    size = sizeof(ng_ndp_opt_rh_t) + data->size;
    if_ent = ng_ipv6_netif_get(((ng_netif_hdr_t *)(data->next->data))->if_pid);

    /* cut of bytes that would exceed minimum MTU */
    size = ((sizeof(ng_ipv6_hdr_t) + size) > if_ent->mtu) ? (if_ent->mtu % 8)
           : size;

    pkt = ng_pktbuf_add(more, NULL, _ceil8(size),
                        NG_NETTYPE_UNDEF);

    if (pkt == NULL) {
        DEBUG("ndp: no space left in packet buffer\n");
        return NULL;
    }

    rh_opt = pkt->data;
    rh_opt->type = NG_NDP_OPT_RH;
    rh_opt->len = pkt->size / 8;
    memset(rh_opt->resv, 0, sizeof(rh_opt->resv));
    memcpy(rh_opt + 1, data->data, pkt->size - sizeof(ng_ndp_opt_rh_t));

    return pkt;
}


#ifdef MODULE_NG_IPV6_ROUTER    /* router only function definitions */
static inline uint8_t _rtr_adv_flags(ng_ipv6_netif_t *iface)
{
    return (iface->flags & (NG_NDP_NETIF_FLAGS_MANAGED |
                            NG_NDP_NETIF_FLAGS_OTHER_CONF));
}

static inline uint8_t _pi_flags(ng_ipv6_netif_addr_t *addr)
{
    return (addr->flags & (NG_IPV6_NETIF_ADDR_FLAGS_NDP_AUTO |
                           NG_IPV6_NETIF_ADDR_FLAGS_NDP_ON_LINK));
}

static ng_pktsnip_t *_iface_rtr_adv_build(ng_ipv6_netif_t *iface, bool final)
{
    ng_pktsnip_t *pkt, *opt;
    uint8_t cur_hl = 0, try_long = 0, l2addr[NG_IPV6_NC_L2_ADDR_MAX];
    uint16_t adv_ltime = 0, uint16_t l2addr_len;
    uint32_t reach_time = 0, retrans_timer = 0;

    mutex_lock(&iface->mutex);

    if (iface->flags | NG_NDP_NETIF_FLAGS_ADV_CUR_HL) {
        cur_hl = iface->cur_hl;
    }

    if (iface->flags | NG_NDP_NETIF_FLAGS_ADV_REACH_TIME) {
        uint64_t tmp = timex_uint64(iface->reach_time) / MS_IN_USEC;

        if (tmp > (3600 * SEC_IN_MS)) { /* tmp > 1 hour */
            tmp = (3600 * SEC_IN_MS);
        }

        reach_time = (uint32_t)tmp;
    }

    if (iface->flags | NG_NDP_NETIF_FLAGS_ADV_RETRANS_TIMER) {
        uint64_t tmp = timex_uint64(iface->retrans_timer) / MS_IN_USEC;

        if (tmp > UINT32_MAX) {
            tmp = UINT32_MAX;
        }


        retrans_timer = (uint32_t)tmp;
    }

    if (!final) {
        adv_ltime = iface->adv_ltime;
    }

    if ((ng_netapi_get(iface->pid, NETCONF_OPT_ADDR_LEN, 0, &l2addr_len,
                       sizeof(l2addr_len)) == 0) && l2addr_len >= 8) {
        try_long = 1;
    }

    if (!try_long || (ng_netapi_get(iface->pid, NETCONF_OPT_ADDRESS_LONG, 0,
                                    &l2addr, sizeof(l2addr)) < 0)) {
        if (ng_netapi_get(iface->pid, NETCONF_OPT_ADDRESS, 0, &l2addr,
                          sizeof(l2addr)) < 0) {
            l2addr_len = 0;
        }
    }

    if (l2addr_len > 0) {
        /* note: may also be omitted to facilitate in-bound load balancing over
         * replicated interfaces.
         * source: https://tools.ietf.org/html/rfc4861#section-6.2.3 */
        if ((opt = ng_ndp_opt_sl2a_build(l2addr, l2addr_len, NULL)) == NULL) {
            DEBUG("ndp: no space left in packet buffer\n");
            mutex_unlock(&iface->mutex);
            return NULL;
        }
    }

    if (iface->flags | NG_NDP_NETIF_FLAGS_ADV_MTU) {
        pkt = opt;

        if ((opt = ng_ndp_opt_mtu_build(iface->mtu, pkt)) == NULL) {
            DEBUG("ndp: no space left in packet buffer\n");
            mutex_unlock(&iface->mutex);
            ng_pktbuf_release(pkt);
            return NULL;
        }
    }

    for (int i = 0; i < NG_IPV6_NETIF_ADDR_NUMOF; i++) {
        ng_ipv6_netif_addr_t *addr = &iface->addrs[i];

        pkt = opt;

        if (!ng_ipv6_addr_is_link_local(&addr->addr) &&
            !ng_ipv6_netif_addr_is_non_unicast(&addr->addr)) {
            if ((opt = ng_ndp_opt_pi_build(addr->prefix_len,
                                           _pi_flags(addr),
                                           addr->valid,
                                           addr->preferreds,
                                           &addr->addr, pkt)) == NULL) {
                DEBUG("ndp: no space left in packet buffer\n");
                mutex_unlock(&iface->mutex);
                ng_pktbuf_release(pkt);
                return NULL;
            }
        }
    }

    pkt = ng_ndp_rtr_adv_build(cur_hl, _rtr_adv_flags(iface), adv_ltime,
                               reach_time, retrans_timer, opt);

    if (pkt == NULL) {
        DEBUG("ndp: no space left in packet buffer\n");
        mutex_unlock(&iface->mutex);
        ng_pktbuf_release(opt);
        return NULL;
    }

    mutex_unlock(&iface->mutex);

    return pkt;
}

static void _advertise_router(ng_ipv6_netif_t *iface, bool final)
{
    ng_pktsnip_t *pkt, *hdr;
    ng_ipv6_addr_t dst = NG_IPV6_ADDR_ALL_NODES_LINK_LOCAL;

    mutex_lock(&iface->mutex);

    if (!((iface->flags | NG_NDP_NETIF_FLAGS_ROUTER) &&
          (iface->flags | NG_NDP_NETIF_FLAGS_RTR_ADV))) {
        DEBUG("ndp: interface %" PRIkernel_pid " is not an advertising interface\n",
              iface->pid);
        return;
    }

    /* set timer for next router advertisement */
    if (!final) {
        vtimer_remove(&iface->rtr_adv_timer);
        vtimer_set_msg(&iface->rtr_adv_timer,
                       timex_set(_rand(iface->min_adv_int,
                                       iface->max_adv_int), 0),
                       thread_getpid(), NG_NDP_MSG_RTR_ADV, iface);
    }

    mutex_unlock(&iface->mutex);

    for (int i = 0; i < NG_IPV6_NETIF_ADDR_NUMOF; i++) {
        ng_ipv6_addr_t *src = &iface->addrs[i].addr;

        if (ng_ipv6_addr_is_link_local(src) &&
            !ng_ipv6_netif_addr_is_non_unicast(src)) {
            /* send one for every link local address (ideally there is only one) */
            pkt = _iface_rtr_adv_build(iface, final);

            if (pkt == NULL) {
                DEBUG("ndp: no space left in packet buffer\n");
                ng_pktbuf_release(pkt);
                return;
            }

            hdr = ng_ipv6_hdr_build(pkt, (uint8_t *)src,
                                    (uint8_t)sizeof(ng_ipv6_addr_t),
                                    (uint8_t *)&dst,
                                    (uint8_t)sizeof(ng_ipv6_addr_t));

            if (hdr == NULL) {
                DEBUG("ndp: no space left in packet buffer\n");
                ng_pktbuf_release(pkt);
                return;
            }

            hdr = ng_netif_hdr_build(NULL, 0, NULL, 0);

            if (hdr == NULL) {
                DEBUG("ndp: no space left in packet buffer\n");
                ng_pktbuf_release(pkt);
                return;
            }

            mutex_lock(&iface->mutex);
            ((ng_netif_hdr_t *)hdr->data)->if_pid = iface->pid;

            mutex_unlock(&iface->mutex);

            LL_PREPEND(pkt, hdr);

            /* send router advertisement to IPv6 control thread */
            ng_netapi_send(thread_getpid(), pkt);
        }
    }
}

void ng_ndp_netif_advertise_router(ng_ipv6_netif_t *iface)
{
    _advertise_router(iface, false);
}

void ng_ndp_netif_add_router(ng_ipv6_netif_t *iface)
{
    ng_ipv6_addr_t all_rtr = NG_IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL;

    /* set default values for routers */
    mutex_lock(&iface->mutex);
    iface->flags |= NG_NDP_NETIF_FLAGS_ROUTER;
    iface->flags |= NG_NDP_NETIF_FLAGS_RTR_ADV;
    iface->max_adv_int = NG_NDP_NETIF_MAX_ADV_INT_DEFAULT;
    iface->min_adv_int = NG_NDP_NETIF_MIN_ADV_INT_DEFAULT;
    iface->adv_ltime = NG_NDP_NETIF_ADV_LTIME;
    mutex_unlock(&iface->mutex);

    /* add all-routers multicast address to interface */
    ng_ipv6_netif_add_addr(iface->pid, &all_rtr, 128, 0);

    /* start periodic advertisements */
    _advertise_router(iface, false);
}

void ng_ndp_netif_remove_router(ng_ipv6_netif_t *iface)
{
    ng_pktsnip_t *hdr, *pkt;
    ng_ipv6_addr_t *src, dst = NG_IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL;

    _advertise_router(iface, true);

    mutex_lock(&iface->mutex);
    iface->flags &= ~NG_NDP_NETIF_FLAGS_ROUTER;
    mutex_unlock(&iface->mutex);

    pkt = _iface_rtr_sol_build(iface);

    if (pkt != NULL) {
        return;
    }

    if ((src = ng_ipv6_netif_find_best_src_addr(iface->pid, &dst)) == NULL) {
        hdr = ng_ipv6_hdr_build(pkt, NULL, 0, (uint8_t *)&dst,
                                (uint8_t)sizeof(ng_ipv6_addr_t));
    }
    else {
        hdr = ng_ipv6_hdr_build(pkt, (uint8_t *)src,
                                (uint8_t)sizeof(ng_ipv6_addr_t),
                                (uint8_t *)&dst,
                                (uint8_t)sizeof(ng_ipv6_addr_t));
    }

    if (hdr == NULL) {
        DEBUG("ndp: no space left in packet buffer\n");
        ng_pktbuf_release(pkt);
        return;
    }

    hdr = ng_netif_hdr_build(NULL, 0, NULL, 0);

    if (hdr == NULL) {
        DEBUG("ndp: no space left in packet buffer\n");
        ng_pktbuf_release(pkt);
        return;
    }

    ((ng_netif_hdr_t *)hdr->data)->if_pid = iface->pid;

    LL_PREPEND(pkt, hdr);

    ng_netapi_send(thread_getpid(), pkt);
}

void ng_ndp_netif_rtr_adv(ng_ipv6_netif_t *iface, bool active)
{
    if (active) {

        if (!(iface->flags & NG_NDP_NETIF_FLAGS_RTR_ADV)) {
            mutex_lock(&iface->mutex);
            iface->flags |= NG_NDP_NETIF_FLAGS_RTR_ADV;
            mutex_unlock(&iface->mutex);

            _advertise_router(iface, false);
        }
    }
    else {
        ng_pktsnip_t *hdr, *pkt;
        ng_ipv6_addr_t *src, dst = NG_IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL;

        _advertise_router(iface, true);

        mutex_lock(&iface->mutex);
        iface->flags &= ~NG_NDP_NETIF_FLAGS_RTR_ADV;
        mutex_unlock(&iface->mutex);

        pkt = _iface_rtr_sol_build(iface);

        if (pkt != NULL) {
            return;
        }

        if ((src = ng_ipv6_netif_find_best_src_addr(iface->pid, &dst)) == NULL) {
            hdr = ng_ipv6_hdr_build(pkt, NULL, 0, (uint8_t *)&dst,
                                    (uint8_t)sizeof(ng_ipv6_addr_t));
        }
        else {
            hdr = ng_ipv6_hdr_build(pkt, (uint8_t *)src,
                                    (uint8_t)sizeof(ng_ipv6_addr_t),
                                    (uint8_t *)&dst,
                                    (uint8_t)sizeof(ng_ipv6_addr_t));
        }

        if (hdr == NULL) {
            DEBUG("ndp: no space left in packet buffer\n");
            ng_pktbuf_release(pkt);
            return;
        }

        hdr = ng_netif_hdr_build(NULL, 0, NULL, 0);

        if (hdr == NULL) {
            DEBUG("ndp: no space left in packet buffer\n");
            ng_pktbuf_release(pkt);
            return;
        }

        ((ng_netif_hdr_t *)hdr->data)->if_pid = iface->pid;

        LL_PREPEND(pkt, hdr);

        ng_netapi_send(thread_getpid(), pkt);
    }
}

ng_pktsnip_t *ng_ndp_rtr_adv_build(uint8_t cur_hl, uint8_t flags,
                                   uint16_t ltime, uint32_t reach_time,
                                   uint32_t retrans_timer, ng_pktsnip_t *options)
{
    ng_pktsnip_t *pkt;
    ng_ndp_rtr_adv_t *rtr_adv;

    DEBUG("ndp: building router advertisement message\n");

    pkt = ng_pktbuf_add(options, NULL, sizeof(ng_ndp_rtr_adv_t),
                        NG_NETTYPE_ICMPV6);

    if (pkt == NULL) {
        DEBUG("ndp: no space left in packet buffer\n");
        return NULL;
    }

    rtr_adv = pkt->data;
    rtr_adv->type = NG_ICMPV6_RTR_ADV;
    rtr_adv->code = 0;
    rtr_adv->cur_hl = cur_hl;
    rtr_adv->flags = (flags & NG_NDP_RTR_ADV_FLAGS_MASK);
    rtr_adv->ltime = byteorder_htons(ltime);
    rtr_adv->reach_time = byteorder_htonl(reach_time);
    rtr_adv->retrans_timer = byteorder_htonl(retrans_timer);

    return pkt;
}

ng_pktsnip_t *ng_ndp_opt_pi_build(uint8_t prefix_len, uint8_t flags,
                                  uint32_t valid_ltime, uint32_t pref_ltime,
                                  ng_ipv6_addr_t *prefix, ng_pktsnip_t *more)
{
    ng_ndp_opt_pi_t *pi_opt;
    ng_pktsnip_t *pkt = ng_pktbuf_add(more, NULL, sizeof(ng_ndp_opt_pi_t),
                                      NG_NETTYPE_UNDEF);

    DEBUG("ndp: building prefix information option\n");

    if (pkt == NULL) {
        DEBUG("ndp: no space left in packet buffer\n");
        return NULL;
    }

    pi_opt = pkt->data;
    pi_opt->type = NG_NDP_OPT_PI;
    pi_opt->len = NG_NDP_OPT_PI_LEN;
    pi_opt->prefix_len = prefix_len;
    pi_opt->flags = (flags & NG_NDP_OPT_PI_FLAGS_MASK);
    pi_opt->valid_ltime = byteorder_htonl(valid_ltime);
    pi_opt->pref_ltime = byteorder_htonl(pref_ltime);
    pi_opt->resv = 0;
    /* Bits beyond prefix_len MUST be 0 */
    ng_ipv6_addr_init_prefix(&pi_opt->prefix, prefix, prefix_len);

    return pkt;
}

ng_pktsnip_t *ng_ndp_opt_mtu_build(uint32_t mtu, ng_pktsnip_t *more)
{
    ng_ndp_opt_mtu_t *mtu_opt;
    ng_pktsnip_t *pkt = ng_pktbuf_add(more, NULL, sizeof(ng_ndp_opt_mtu_t),
                                      NG_NETTYPE_UNDEF);

    DEBUG("ndp: building MTU option\n");

    if (pkt == NULL) {
        DEBUG("ndp: no space left in packet buffer\n");
        return NULL;
    }

    mtu_opt = pkt->data;
    mtu_opt->type = NG_NDP_OPT_MTU;
    mtu_opt->len = NG_NDP_OPT_MTU_LEN;
    mtu_opt->resv = 0;
    mtu_opt->mtu = byteorder_htonl(mtu);

    return pkt;
}

static void _handle_rtr_sol(kernel_pid_t iface, ng_pktsnip_t *pkt,
                            ng_ipv6_hdr_t *ipv6, ng_ndp_rtr_sol_t *rtr_sol,
                            size_t icmpv6_size)
{
    ng_pktsnip_t *adv, *hdr;
    ng_ipv6_addr_t src = NG_IPV6_ADDR_ALL_NODES_LINK_LOCAL;
    uint16_t opt_offset = 0;
    uint8_t *buf = (uint8_t *)(rtr_sol + 1);
    ng_ipv6_netif_t *if_entry = ng_ipv6_netif_get(iface);

    /* check validity */
    if ((ipv6->hl != 255) || (rtr_sol->code != 0) ||
        (icmpv6_size < sizeof(ng_ndp_rtr_sol_t))) {
        DEBUG("ndp: router solicitation was invalid\n");
        return;
    }

    while (icmpv6_size > 0) {
        ng_ndp_opt_t *opt = (ng_ndp_opt_t *)(buf + opt_offset);

        switch (opt->type) {
            case NG_NDP_OPT_SL2A:
                if (!_handle_sl2a_opt(iface, pkt, ipv6, rtr_sol->type, opt)) {
                    /* invalid source link-layer address option */
                    return;
                }

                break;

            default:
                /* silently discard all other options */
                break;
        }

        opt_offset += (opt->len * 8);
        icmpv6_size -= (opt->len * 8);
    }

    if (!ng_ipv6_addr_is_unspecified(&ipv6->src)) {
        src.u64[0] = ipv6->src.u64[0];
        src.u64[1] = ipv6->src.u64[1];
    }

    adv = _iface_rtr_adv_build(if_entry, false);

    if (adv == NULL) {
        DEBUG("ndp: error on router advertisement allocation\n");
        return;
    }

    hdr = ng_ipv6_hdr_build(adv, NULL, 0, (uint8_t *)&src, sizeof(src));

    if (hdr == NULL) {
        DEBUG("ndp: error on router advertisement allocation\n");
        ng_pktbuf_release(adv);
        return;
    }

    adv = hdr;

    /* add set interface header for interface addressing */
    hdr = ng_netif_hdr_build(NULL, 0, NULL, 0);

    if (hdr == NULL) {
        DEBUG("ndp: error on router advertisement allocation\n");
        ng_pktbuf_release(adv);
        return;
    }

    ((ng_netif_hdr_t *)hdr)->if_pid = iface;
    /* prepend to packet */
    LL_PREPEND(adv, hdr);

    /* send router advertisement delayed */
    vtimer_remove(&if_entry->rtr_sol_timer);
    vtimer_set_msg(&if_entry->rtr_sol_timer,
                   timex_set(0, _rand(0, NG_NDP_MAX_RTR_ADV_DELAY)),
                   thread_getpid(), NG_NETAPI_MSG_TYPE_SND, adv);
}
#endif  /* MODULE_NG_IPV6_ROUTER */

/* internal functions */
/* destination_cache functions */
static void _dst_cache_add(kernel_pid_t iface, ng_ipv6_addr_t *dst,
                           ng_ipv6_addr_t *next_hop)
{
    for (int i = 0; i < NG_NDP_DST_CACHE_SIZE; i++) {
        if (_dst_cache[i].iface == iface &&
            ng_ipv6_addr_equal(&_dst_cache[i].dst, dst) &&
            ng_ipv6_addr_equal(&_dst_cache[i].next_hop, next_hop)) {
            return;
        }
    }

    _dst_cache[_dst_cache_idx].iface = iface;
    memcpy(&(_dst_cache[_dst_cache_idx].dst), dst, sizeof(ng_ipv6_addr_t));
    memcpy(&(_dst_cache[_dst_cache_idx].next_hop), next_hop, sizeof(ng_ipv6_addr_t));

    _dst_cache_idx = (_dst_cache_idx + 1) % NG_NDP_DST_CACHE_SIZE;
}

static kernel_pid_t _dst_cache_get_next_hop(ng_ipv6_addr_t **next_hop,
        const ng_ipv6_addr_t *dst)
{
    for (int i = 0; i < NG_NDP_DST_CACHE_SIZE; i++) {
        if (ng_ipv6_addr_equal(&_dst_cache[i].dst, dst)) {
            *next_hop = &_dst_cache[i].next_hop;
            return _dst_cache[i].iface;
        }
    }

    return KERNEL_PID_UNDEF;
}

/* packet queue node allocation */
static ng_pktqueue_node_t *_alloc_pkt_node(ng_pktsnip_t *pkt)
{
    for (int i = 0; i < sizeof(_pkt_nodes); i++) {
        if (_pkt_nodes[i].data == NULL) {
            ng_pktqueue_node_init(_pkt_nodes + i);
            _pkt_nodes[i].data = pkt;

            return _pkt_nodes + i;
        }
    }

    return NULL;
}

static inline void _free_pkt_node(ng_pktqueue_node_t *node)
{
    node->data = NULL;
}

static void _handle_rtr_adv(kernel_pid_t iface, ng_pktsnip_t *pkt, ng_ipv6_hdr_t *ipv6,
                            ng_ndp_rtr_adv_t *rtr_adv, size_t icmpv6_size)
{
    uint16_t opt_offset = 0;
    uint8_t *buf = (uint8_t *)(rtr_adv + 1);
    ng_ipv6_nc_t *nc_entry = NULL;
    ng_ipv6_netif_t *if_entry = ng_ipv6_netif_get(iface);

    /* check validity */
    if (!ng_ipv6_addr_is_link_local(&ipv6->src) ||
        ng_ipv6_addr_is_multicast(&ipv6->src) ||
        (ipv6->hl != 255) || (rtr_adv->code != 0) ||
        (icmpv6_size < sizeof(ng_ndp_rtr_adv_t))) {
        DEBUG("ndp: router advertisement was invalid\n");
        /* ipv6 releases */
        return;
    }

    nc_entry = ng_ipv6_nc_get(iface, &ipv6->src);

    if (nc_entry == NULL) { /* if still not found */
        /* create default router list entry */
        nc_entry = ng_ipv6_nc_add(iface, &ipv6->src, NULL, 0,
                                  NG_IPV6_NC_IS_ROUTER);

        if (nc_entry == NULL) {
            DEBUG("ndp: error on default router list entry creation\n");
            return;
        }
    }
    else {
        if (nc_entry->flags & NG_IPV6_NC_IS_ROUTER) {
            if (byteorder_ntohs(rtr_adv->ltime) == 0) {
                nc_entry->flags &= ~NG_IPV6_NC_IS_ROUTER;
            }
        }
        else {
            nc_entry->flags |= NG_IPV6_NC_IS_ROUTER;
        }
    }

    if (rtr_adv->ltime.u16 != 0) {
        vtimer_remove(&nc_entry->rtr_timeout);
        vtimer_set_msg(&nc_entry->rtr_timeout,
                       timex_set(byteorder_ntohs(rtr_adv->ltime), 0),
                       thread_getpid(), NG_NDP_MSG_RTR_TIMEOUT, nc_entry);
    }

    mutex_lock(&if_entry->mutex);

    if (rtr_adv->cur_hl != 0) {
        if_entry->cur_hl = rtr_adv->cur_hl;
    }

    if_entry->flags &= 0x3f;
    if_entry->flags |= (rtr_adv->flags & (0xc0));

    if (rtr_adv->reach_time.u32 != 0) {
        if (if_entry->reach_time_base != byteorder_ntohl(rtr_adv->reach_time)) {
            uint32_t reach_time = _rand(NG_NDP_MIN_RAND, NG_NDP_MAX_RAND);

            if_entry->reach_time_base = byteorder_ntohl(rtr_adv->reach_time);
            reach_time = (reach_time * if_entry->reach_time_base) / 10;
            if_entry->reach_time = timex_set(0, reach_time);
            timex_normalize(&if_entry->reach_time);
        }
    }

    if (rtr_adv->retrans_timer.u32 != 0) {
        if_entry->retrans_timer = timex_set(0,
                                            byteorder_ntohl(rtr_adv->retrans_timer));
        timex_normalize(&if_entry->retrans_timer);
    }

    mutex_unlock(&if_entry->mutex);

    while (icmpv6_size > 0) {
        ng_ndp_opt_t *opt = (ng_ndp_opt_t *)(buf + opt_offset);

        switch (opt->type) {
            case NG_NDP_OPT_SL2A:
                if (!_handle_sl2a_opt(iface, pkt, ipv6, rtr_adv->type, opt)) {
                    /* invalid source link-layer address option */
                    return;
                }

                break;

            case NG_NDP_OPT_MTU:
                if (!_handle_mtu_opt(iface, pkt, ipv6, rtr_adv->type,
                                     (ng_ndp_opt_mtu_t *)opt)) {
                    /* invalid MTU option */
                    return;
                }

                break;

            case NG_NDP_OPT_PI:
                if (!_handle_pi_opt(iface, pkt, ipv6, rtr_adv->type,
                                    (ng_ndp_opt_pi_t *)opt)) {
                    /* invalid prefix information option */
                    return;
                }

                break;

            default:
                /* silently discard all other options */
                break;
        }

        opt_offset += (opt->len * 8);
        icmpv6_size -= (opt->len * 8);
    }

    return;
}

static void _handle_nbr_sol(kernel_pid_t iface, ng_pktsnip_t *pkt, ng_ipv6_hdr_t *ipv6,
                            ng_ndp_nbr_sol_t *nbr_sol, size_t icmpv6_size)
{
    return;
}

static void _handle_nbr_adv(kernel_pid_t iface, ng_pktsnip_t *pkt, ng_ipv6_hdr_t *ipv6,
                            ng_ndp_nbr_adv_t *nbr_adv, size_t icmpv6_size)
{
    return;
}

static void _handle_redirect(kernel_pid_t iface, ng_pktsnip_t *pkt, ng_ipv6_hdr_t *ipv6,
                             ng_ndp_redirect_t *redirect, size_t icmpv6_size)
{
    return;
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

    if (sl2a_len == 0) {  /* in case there was no source address
                                 * (e.g. slip) */
        sl2a_len = (sl2a_opt->len / 8) - sizeof(ng_ndp_opt_t);

        /* ignore all zeroes at the end for length */
        for (; sl2a[sl2a_len - 1] != 0x00; sl2a_len--);
    }

    switch (icmpv6_type) {
#ifdef MODULE_NG_IPV6_ROUTER
        case NG_ICMPV6_RTR_SOL:
            nc_entry = ng_ipv6_nc_get(iface, &ipv6->src);

            if (nc_entry != NULL) {
                if ((sl2a_len != nc_entry->l2_addr_len) ||
                    (memcmp(sl2a, nc_entry->l2_addr, sl2a_len) != 0)) {
                    /* if entry exists but l2 address differs: set */
                    nc_entry->l2_addr_len = sl2a_len;
                    memcpy(nc_entry->l2_addr, sl2a, sl2a_len);

                    /* set router to false. XXX: this will be different for
                     * 6LoWPAN interfaces. */
                    nc_entry->flags &= ~NG_IPV6_NC_IS_ROUTER;
                    nc_entry->flags &= ~NG_IPV6_NC_STATE_MASK;
                    nc_entry->flags |= NG_IPV6_NC_STATE_STALE;
                }
            }
            else {
                ng_ipv6_nc_add(iface, &ipv6->src, sl2a, sl2a_len,
                               NG_IPV6_NC_STATE_STALE);

            }

            return true;
#endif
        case NG_ICMPV6_RTR_ADV:
            /* entry was created as part of default router list so no
             * check necessary */
            nc_entry = ng_ipv6_nc_get(iface, &ipv6->src);

            if ((sl2a_len == nc_entry->l2_addr_len) &&
                (memcmp(sl2a, nc_entry->l2_addr, sl2a_len) == 0)) {
                /* no update needed */
                return true;
            }

            nc_entry->l2_addr_len = sl2a_len;
            memcpy(nc_entry->l2_addr, sl2a, sl2a_len);

            /* NG_IPV6_NC_IS_ROUTER flag already set (or unset if adv::ltime was 0) */
            nc_entry->flags &= ~NG_IPV6_NC_STATE_MASK;
            nc_entry->flags |= NG_IPV6_NC_STATE_STALE;

            return true;

        default:    /* wrong encapsulating message: silently discard */
            DEBUG("ndp: silently discard sl2a_opt for ICMPv6 message type %"
                  PRIu8 "\n", icmpv6_type);
            return true;
    }
}

static bool _handle_pi_opt(kernel_pid_t iface, ng_pktsnip_t *pkt,
                           ng_ipv6_hdr_t *ipv6, uint8_t icmpv6_type,
                           ng_ndp_opt_pi_t *pi_opt)
{
    ng_ipv6_addr_t *prefix;
    ng_ipv6_netif_addr_t *a;

    if (pi_opt->len != NG_NDP_OPT_PI_LEN) {
        DEBUG("ndp: invalid prefix information option received\n");
        return false;
    }

    if ((icmpv6_type != NG_ICMPV6_RTR_ADV) ||
        ng_ipv6_addr_is_link_local(&pi_opt->prefix)) {
        /* discard silently */
        return true;
    }

    prefix = ng_ipv6_netif_find_addr(iface, &pi_opt->prefix);

    if (((prefix == NULL) ||
         (ng_ipv6_netif_addr_get(prefix)->prefix_len != pi_opt->prefix_len)) &&
        (pi_opt->valid_ltime.u32 != 0)) {
        prefix = ng_ipv6_netif_add_addr(iface, &pi_opt->prefix,
                                        pi_opt->prefix_len,
                                        pi_opt->flags & NG_NDP_OPT_PI_FLAGS_MASK);

        if (prefix == NULL) {
            DEBUG("ndp: could not add prefix to interface %d\n", iface);
            return false;
        }
    }

    a = ng_ipv6_netif_addr_get(prefix);

    if (pi_opt->valid_ltime.u32 == 0) {
        if (prefix != NULL) {
            ng_ipv6_netif_remove_addr(iface, &a->addr);
        }

        return true;
    }

    a->valid = byteorder_ntohl(pi_opt->valid_ltime);
    a->preferred = byteorder_ntohl(pi_opt->pref_ltime);

    vtimer_remove(&a->valid_timeout);

    if (a->valid != UINT32_MAX) {
        vtimer_set_msg(&a->valid_timeout,
                       timex_set(byteorder_ntohl(pi_opt->valid_ltime), 0),
                       thread_getpid(), NG_NDP_MSG_ADDR_TIMEOUT, &a->addr);
    }

    /* TODO: preferred lifetime for address auto configuration */

    /* on-link flag MUST stay set */
    a->flags &= ~NG_NDP_OPT_PI_FLAGS_A;
    a->flags |= (pi_opt->flags & NG_NDP_OPT_PI_FLAGS_MASK);

    return true;
}

static bool _handle_mtu_opt(kernel_pid_t iface, ng_pktsnip_t *pkt,
                            ng_ipv6_hdr_t *ipv6, uint8_t icmpv6_type,
                            ng_ndp_opt_mtu_t *mtu_opt)
{
    ng_ipv6_netif_t *if_entry = ng_ipv6_netif_get(iface);

    if ((mtu_opt->len != NG_NDP_OPT_MTU_LEN) ||
        (byteorder_ntohl(mtu_opt->mtu) < NG_IPV6_NETIF_DEFAULT_MTU)) {
        DEBUG("ndp: invalid MTU option received\n");
        return false;
    }

    if (icmpv6_type != NG_ICMPV6_RTR_ADV) {
        /* else discard silently */
        return true;
    }

    if_entry->mtu = byteorder_ntohl(mtu_opt->mtu);

    return true;
}

static ng_pktsnip_t *_iface_rtr_sol_build(ng_ipv6_netif_t *iface)
{
    ng_pktsnip_t *pkt, *opt = NULL;
    uint8_t try_long = 0, l2addr[NG_IPV6_NC_L2_ADDR_MAX];
    size_t l2addr_len;

    if ((ng_netapi_get(iface->pid, NETCONF_OPT_ADDR_LEN, 0, &l2addr_len,
                       sizeof(l2addr_len)) == 0) && l2addr_len >= 8) {
        try_long = 1;
    }

    if (!try_long || (ng_netapi_get(iface->pid, NETCONF_OPT_ADDRESS_LONG, 0,
                                    &l2addr, sizeof(l2addr)) < 0)) {
        if (ng_netapi_get(iface->pid, NETCONF_OPT_ADDRESS, 0, &l2addr,
                          sizeof(l2addr)) < 0) {
            l2addr_len = 0;
        }
    }

    if (l2addr_len > 0) {
        /* note: may also be omitted to facilitate in-bound load balancing over
         * replicated interfaces.
         * source: https://tools.ietf.org/html/rfc4861#section-6.2.3 */
        if ((opt = ng_ndp_opt_sl2a_build(l2addr, l2addr_len, NULL)) == NULL) {
            DEBUG("ndp: no space left in packet buffer\n");
            return NULL;
        }
    }

    pkt = ng_ndp_rtr_sol_build(opt);

    if (pkt == NULL) {
        DEBUG("ndp: no space left in packet buffer\n");
        ng_pktbuf_release(opt);
        return NULL;
    }

    return pkt;
}

/**
 * @}
 */
