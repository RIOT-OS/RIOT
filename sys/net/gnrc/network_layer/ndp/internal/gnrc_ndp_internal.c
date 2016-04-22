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

#include <stdlib.h>

#include "net/eui64.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/ndp.h"
#include "net/gnrc/sixlowpan/ctx.h"
#include "net/gnrc/sixlowpan/nd.h"
#include "random.h"
#include "xtimer.h"

#include "net/gnrc/ndp/internal.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

static gnrc_ipv6_nc_t *_last_router = NULL; /* last router chosen as default
                                             * router. Only used if reachability
                                             * is suspect (i. e. incomplete or
                                             * not at all) */
static gnrc_pktsnip_t *_build_headers(kernel_pid_t iface, gnrc_pktsnip_t *payload,
                                      ipv6_addr_t *dst, ipv6_addr_t *src);
static size_t _get_l2src(kernel_pid_t iface, uint8_t *l2src, size_t l2src_maxlen);

/**
 * @brief   Sends @ref GNRC_NETAPI_MSG_TYPE_SND delayed.
 *
 * @param[in] t         Timer for the delay.
 * @param[in] msg       Msg for the timer.
 * @param[in] interval  Delay interval.
 * @param[in] pkt       Packet to send delayed.
 */
static inline void _send_delayed(xtimer_t *t, msg_t *msg, uint32_t interval, gnrc_pktsnip_t *pkt)
{
    xtimer_remove(t);
    msg->type = GNRC_NETAPI_MSG_TYPE_SND;
    msg->content.ptr = (char *) pkt;
    xtimer_set_msg(t, interval, msg, gnrc_ipv6_pid);
}


ipv6_addr_t *gnrc_ndp_internal_default_router(void)
{
    gnrc_ipv6_nc_t *router = gnrc_ipv6_nc_get_next_router(NULL);

    /* first look if there is any reachable router */
    while (router != NULL) {
        if ((gnrc_ipv6_nc_get_state(router) != GNRC_IPV6_NC_STATE_INCOMPLETE) &&
            (gnrc_ipv6_nc_get_state(router) != GNRC_IPV6_NC_STATE_UNREACHABLE)) {
            _last_router = NULL;

            return &router->ipv6_addr;
        }

        router = gnrc_ipv6_nc_get_next_router(router);
    }

    /* else take the first one, but keep round-robin in further selections */
    router = gnrc_ipv6_nc_get_next_router(_last_router);

    if (router == NULL) {   /* end of router list or there is none => wrap around */
        router = gnrc_ipv6_nc_get_next_router(router);

        if (router == NULL) {   /* still nothing found => no router in list */
            return NULL;
        }
    }

    _last_router = router;

    return &router->ipv6_addr;
}

void gnrc_ndp_internal_set_state(gnrc_ipv6_nc_t *nc_entry, uint8_t state)
{
    gnrc_ipv6_netif_t *ipv6_iface;
    uint32_t t = GNRC_NDP_FIRST_PROBE_DELAY * SEC_IN_USEC;

    nc_entry->flags &= ~GNRC_IPV6_NC_STATE_MASK;
    nc_entry->flags |= state;

    DEBUG("ndp internal: set %s state to ",
          ipv6_addr_to_str(addr_str, &nc_entry->ipv6_addr, sizeof(addr_str)));

    switch (state) {
        case GNRC_IPV6_NC_STATE_REACHABLE:
            ipv6_iface = gnrc_ipv6_netif_get(nc_entry->iface);
            DEBUG("REACHABLE (reachable time = %" PRIu32 " us)\n", ipv6_iface->reach_time);
            t = ipv6_iface->reach_time;

            /* we intentionally fall through here to set the desired timeout t */
        case GNRC_IPV6_NC_STATE_DELAY:
#if ENABLE_DEBUG
            if (state == GNRC_IPV6_NC_STATE_DELAY) {
                DEBUG("DELAY (probe with unicast NS in %u seconds)\n",
                      GNRC_NDP_FIRST_PROBE_DELAY);
            }
#endif
            gnrc_ndp_internal_reset_nbr_sol_timer(nc_entry, t, GNRC_NDP_MSG_NC_STATE_TIMEOUT,
                                                  gnrc_ipv6_pid);
            break;

        case GNRC_IPV6_NC_STATE_PROBE:
            ipv6_iface = gnrc_ipv6_netif_get(nc_entry->iface);

            nc_entry->probes_remaining = GNRC_NDP_MAX_UC_NBR_SOL_NUMOF;
            DEBUG("PROBE (probe with %" PRIu8 " unicast NS every %" PRIu32 " us)\n",
                  nc_entry->probes_remaining, ipv6_iface->retrans_timer);

            gnrc_ndp_internal_send_nbr_sol(nc_entry->iface, NULL, &nc_entry->ipv6_addr,
                                           &nc_entry->ipv6_addr);

            mutex_lock(&ipv6_iface->mutex);
            gnrc_ndp_internal_reset_nbr_sol_timer(nc_entry, ipv6_iface->retrans_timer,
                                                  GNRC_NDP_MSG_NBR_SOL_RETRANS, gnrc_ipv6_pid);
            mutex_unlock(&ipv6_iface->mutex);
            break;

#ifdef ENABLE_DEBUG
        case GNRC_IPV6_NC_STATE_STALE:
            DEBUG("STALE (go into DELAY on next packet)\n");
            break;
#endif

        default:
            DEBUG("errorneous or unknown\n");
            break;
    }
}

void gnrc_ndp_internal_send_nbr_adv(kernel_pid_t iface, ipv6_addr_t *tgt, ipv6_addr_t *dst,
                                    bool supply_tl2a, gnrc_pktsnip_t *ext_opts)
{
    gnrc_pktsnip_t *hdr, *pkt = ext_opts;
    uint8_t adv_flags = 0;

    DEBUG("ndp internal: send neighbor advertisement (iface: %" PRIkernel_pid ", tgt: %s, ",
          iface, ipv6_addr_to_str(addr_str, tgt, sizeof(addr_str)));
    DEBUG("dst: %s, supply_tl2a: %d)\n",
          ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)), supply_tl2a);

    if ((gnrc_ipv6_netif_get(iface)->flags & GNRC_IPV6_NETIF_FLAGS_ROUTER) &&
        (gnrc_ipv6_netif_get(iface)->flags & GNRC_IPV6_NETIF_FLAGS_RTR_ADV)) {
        adv_flags |= NDP_NBR_ADV_FLAGS_R;
    }

    if (ipv6_addr_is_unspecified(dst)) {
        ipv6_addr_set_all_nodes_multicast(dst, IPV6_ADDR_MCAST_SCP_LINK_LOCAL);
    }
    else {
        adv_flags |= NDP_NBR_ADV_FLAGS_S;
    }

    if (supply_tl2a) {
        uint8_t l2src[8];
        size_t l2src_len;
        /* we previously checked if we are the target, so we can take our L2src */
        l2src_len = _get_l2src(iface, l2src, sizeof(l2src));

        if (l2src_len > 0) {
            /* add target address link-layer address option */
            pkt = gnrc_ndp_opt_tl2a_build(l2src, l2src_len, pkt);

            if (pkt == NULL) {
                DEBUG("ndp internal: error allocating Target Link-layer address option.\n");
                gnrc_pktbuf_release(ext_opts);
                return;
            }
        }
    }

    /* TODO: also check if the node provides proxy servies for tgt */
    if ((pkt != NULL) && !gnrc_ipv6_netif_addr_is_non_unicast(tgt)) {
        /* TL2A is not supplied and tgt is not anycast */
        adv_flags |= NDP_NBR_ADV_FLAGS_O;
    }

    hdr = gnrc_ndp_nbr_adv_build(adv_flags, tgt, pkt);

    if (hdr == NULL) {
        DEBUG("ndp internal: error allocating Neighbor advertisement.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    pkt = hdr;
    hdr = _build_headers(iface, pkt, dst, NULL);
    if (hdr == NULL) {
        DEBUG("ndp internal: error adding lower-layer headers.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    if (gnrc_ipv6_netif_addr_is_non_unicast(tgt)) {
        /* avoid collision for anycast addresses
         * (see https://tools.ietf.org/html/rfc4861#section-7.2.7) */
        uint32_t delay = random_uint32_range(0, GNRC_NDP_MAX_AC_TGT_DELAY * SEC_IN_USEC);
        gnrc_ipv6_nc_t *nc_entry = gnrc_ipv6_nc_get(iface, dst);
        DEBUG("ndp internal: delay neighbor advertisement for %" PRIu32 " sec.",
              (delay / SEC_IN_USEC));

        /* nc_entry must be set so no need to check it */
        assert(nc_entry);

        _send_delayed(&nc_entry->nbr_adv_timer, &nc_entry->nbr_adv_msg, delay, hdr);
    }
    else if (gnrc_netapi_send(gnrc_ipv6_pid, hdr) < 1) {
        DEBUG("ndp internal: unable to send neighbor advertisement\n");
        gnrc_pktbuf_release(hdr);
    }
}

void gnrc_ndp_internal_send_nbr_sol(kernel_pid_t iface, ipv6_addr_t *src, ipv6_addr_t *tgt,
                                    ipv6_addr_t *dst)
{
#ifdef MODULE_GNRC_SIXLOWPAN_ND
    gnrc_ipv6_netif_t *ipv6_iface = gnrc_ipv6_netif_get(iface);
    assert(ipv6_iface != NULL);
#endif
    gnrc_pktsnip_t *hdr, *pkt = NULL;
    /* both suppressions, since they are needed in the MODULE_GNRC_SIXLOWPAN_ND branch */
    /* cppcheck-suppress variableScope */
    uint8_t l2src[8];
    /* cppcheck-suppress variableScope */
    size_t l2src_len = 0;

    DEBUG("ndp internal: send neighbor solicitation (iface: %" PRIkernel_pid ", src: %s, ",
          iface, ipv6_addr_to_str(addr_str, src, sizeof(addr_str)));
    DEBUG(" tgt: %s, ", ipv6_addr_to_str(addr_str, tgt, sizeof(addr_str)));
    DEBUG("dst: %s)\n", ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)));

    /* check if there is a fitting source address to target */
    if (src == NULL) {
        src = gnrc_ipv6_netif_find_best_src_addr(iface, tgt, false);
    }
    if (src != NULL) {
        l2src_len = _get_l2src(iface, l2src, sizeof(l2src));

        if (l2src_len > 0) {
            /* add source address link-layer address option */
            pkt = gnrc_ndp_opt_sl2a_build(l2src, l2src_len, NULL);

            if (pkt == NULL) {
                DEBUG("ndp internal: error allocating Source Link-layer address option.\n");
                gnrc_pktbuf_release(pkt);
                return;
            }
        }
    }

#ifdef MODULE_GNRC_SIXLOWPAN_ND
    if (ipv6_iface->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN) {
        if (l2src_len != sizeof(eui64_t)) {
            l2src_len = (uint16_t)gnrc_netapi_get(iface, NETOPT_ADDRESS_LONG, 0, l2src,
                                                  sizeof(l2src));
            if (l2src_len != sizeof(eui64_t)) {
                DEBUG("ndp internal: can't get EUI-64 of the interface\n");
                gnrc_pktbuf_release(pkt);
                return;
            }
        }
        hdr = gnrc_sixlowpan_nd_opt_ar_build(0, GNRC_SIXLOWPAN_ND_AR_LTIME, (eui64_t *)l2src, pkt);
        if (hdr == NULL) {
            DEBUG("ndp internal: error allocatin Address Registration option.\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
        pkt = hdr;
    }
#endif

    hdr = gnrc_ndp_nbr_sol_build(tgt, pkt);

    if (hdr == NULL) {
        DEBUG("ndp internal: error allocating Neighbor solicitation.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    pkt = hdr;
    hdr = _build_headers(iface, pkt, dst, src);
    if (hdr == NULL) {
        DEBUG("ndp internal: error adding lower-layer headers.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    else if (gnrc_netapi_send(gnrc_ipv6_pid, hdr) < 1) {
        DEBUG("ndp internal: unable to send neighbor solicitation\n");
        gnrc_pktbuf_release(hdr);
    }
}

void gnrc_ndp_internal_send_rtr_sol(kernel_pid_t iface, ipv6_addr_t *dst)
{
    gnrc_pktsnip_t *hdr, *pkt = NULL;
    ipv6_addr_t *src = NULL;
    DEBUG("ndp internal: send router solicitation (iface: %" PRIkernel_pid ", dst: ff02::2)\n",
          iface);
    if (dst == NULL) {
        /* isn't changed afterwards so discarding const should be alright */
        dst = (ipv6_addr_t *)&ipv6_addr_all_routers_link_local;
    }
    /* check if there is a fitting source address to target */
    if ((src = gnrc_ipv6_netif_find_best_src_addr(iface, dst, false)) != NULL) {
        uint8_t l2src[8];
        size_t l2src_len;
        l2src_len = _get_l2src(iface, l2src, sizeof(l2src));
        if (l2src_len > 0) {
            /* add source address link-layer address option */
            pkt = gnrc_ndp_opt_sl2a_build(l2src, l2src_len, NULL);

            if (pkt == NULL) {
                DEBUG("ndp internal: error allocating Source Link-layer address option.\n");
                gnrc_pktbuf_release(pkt);
                return;
            }
        }
    }
    hdr = gnrc_ndp_rtr_sol_build(pkt);
    if (hdr == NULL) {
        DEBUG("ndp internal: error allocating router solicitation.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    pkt = hdr;
    hdr = _build_headers(iface, pkt, dst, src);
    if (hdr == NULL) {
        DEBUG("ndp internal: error adding lower-layer headers.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    else if (gnrc_netapi_send(gnrc_ipv6_pid, hdr) < 1) {
        DEBUG("ndp internal: unable to send router solicitation\n");
        gnrc_pktbuf_release(hdr);
    }
}

#if (defined(MODULE_GNRC_NDP_ROUTER) || defined(MODULE_GNRC_SIXLOWPAN_ND_ROUTER))
static bool _pio_from_iface_addr(gnrc_pktsnip_t **res, gnrc_ipv6_netif_t *iface,
                                 gnrc_ipv6_netif_addr_t *addr, gnrc_pktsnip_t *next)
{
    assert(((uint8_t) addr->prefix_len) <= 128U);

    if (!ipv6_addr_is_unspecified(&addr->addr) &&
        !ipv6_addr_is_link_local(&addr->addr) &&
        !gnrc_ipv6_netif_addr_is_non_unicast(&addr->addr)) {
        uint8_t flags = 0;
        DEBUG(" - PIO for %s/%" PRIu8 "\n", ipv6_addr_to_str(addr_str, &addr->addr,
                                                             sizeof(addr_str)),
              addr->prefix_len);

#ifdef MODULE_GNRC_SIXLOWPAN_ND
        if (!(iface->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN)) {
            flags = GNRC_IPV6_NETIF_ADDR_FLAGS_NDP_ON_LINK;
        }
#else
        (void) iface;
#endif

        *res = gnrc_ndp_opt_pi_build(addr->prefix_len, addr->flags | flags, addr->valid,
                                     addr->preferred, &addr->addr, next);
        return true;
    }
    return false;
}

static inline bool _check_prefixes(gnrc_ipv6_netif_addr_t *a, gnrc_ipv6_netif_addr_t *b)
{
    if ((a->prefix_len == b->prefix_len) &&
        (ipv6_addr_match_prefix(&a->addr, &b->addr) >= a->prefix_len)) {
        return true;
    }

    return false;
}

static bool _add_pios(gnrc_pktsnip_t **res, gnrc_ipv6_netif_t *ipv6_iface, gnrc_pktsnip_t *pkt)
{
    for (int i = 0; i < GNRC_IPV6_NETIF_ADDR_NUMOF; i++) {
        /* skip if prefix has been processed already */
        bool processed_before = false;

        for (int j = 0; j < i; j++) {
            if ((processed_before =
                     _check_prefixes(&ipv6_iface->addrs[i], &ipv6_iface->addrs[j]))) {
                break;
            }
        }

        if (processed_before) {
            continue;
        }

        if (_pio_from_iface_addr(res, ipv6_iface, &ipv6_iface->addrs[i], pkt)) {
            if (*res != NULL) {
                pkt = *res;
            }
            else {
                DEBUG("ndp rtr: error allocating PIO\n");
                gnrc_pktbuf_release(pkt);
                return false;
            }
        }
    }
    return true;
}

void gnrc_ndp_internal_send_rtr_adv(kernel_pid_t iface, ipv6_addr_t *src, ipv6_addr_t *dst,
                                    bool fin)
{
    gnrc_pktsnip_t *hdr = NULL, *pkt = NULL;
    gnrc_ipv6_netif_t *ipv6_iface = gnrc_ipv6_netif_get(iface);
    uint32_t reach_time = 0, retrans_timer = 0;
    uint16_t adv_ltime = 0;
    uint8_t cur_hl = 0;

    if (dst == NULL) {
        /* isn't changed afterwards so discarding const should be fine */
        dst = (ipv6_addr_t *)&ipv6_addr_all_nodes_link_local;
    }
    DEBUG("ndp internal: send router advertisement (iface: %" PRIkernel_pid ", dst: %s%s\n",
          iface, ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)), fin ? ", final" : "");
    mutex_lock(&ipv6_iface->mutex);
#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
    if (!(ipv6_iface->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN)) {
#endif
    if (!_add_pios(&hdr, ipv6_iface, pkt)) {
        /* pkt already released in _add_pios */
        mutex_unlock(&ipv6_iface->mutex);
        return;
    }
    pkt = hdr;
#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
    }
    else {
        gnrc_sixlowpan_nd_router_abr_t *abr = gnrc_sixlowpan_nd_router_abr_get();
        if (abr != NULL) {
            gnrc_sixlowpan_nd_router_prf_t *prf = abr->prfs;
            /* add prefixes from border router */
            while (prf) {
                bool processed_before = false;
                /* skip if prefix does not belong to iface */
                if (prf->iface != ipv6_iface) {
                    prf = prf->next;
                    continue;
                }
                /* skip if prefix has been processed already */
                for (gnrc_sixlowpan_nd_router_prf_t *tmp = abr->prfs; tmp != prf; tmp = tmp->next) {
                    if ((processed_before =
                             _check_prefixes(prf->prefix, tmp->prefix))) {
                        break;
                    }
                }

                if (processed_before) {
                    prf = prf->next;
                    continue;
                }

                if (_pio_from_iface_addr(&hdr, ipv6_iface, prf->prefix, pkt)) {
                    if (hdr != NULL) {
                        pkt = hdr;
                    }
                    else {
                        DEBUG("ndp rtr: error allocating PIO\n");
                        gnrc_pktbuf_release(pkt);
                        mutex_unlock(&ipv6_iface->mutex);
                        return;
                    }
                }
                prf = prf->next;
            }
            for (unsigned int i = 0; i < GNRC_SIXLOWPAN_CTX_SIZE; i++) {
                gnrc_sixlowpan_ctx_t *ctx;
                if (!bf_isset(abr->ctxs, i)) {
                    continue;
                }
                ctx = gnrc_sixlowpan_ctx_lookup_id(i);
                hdr = gnrc_sixlowpan_nd_opt_6ctx_build(ctx->prefix_len, ctx->flags_id, ctx->ltime,
                                                       &ctx->prefix, pkt);
                if (hdr == NULL) {
                    DEBUG("ndp rtr: error allocating 6CO\n");
                    mutex_unlock(&ipv6_iface->mutex);
                    gnrc_pktbuf_release(pkt);
                    return;
                }
                pkt = hdr;
            }
            hdr = gnrc_sixlowpan_nd_opt_abr_build(abr->version, abr->ltime, &abr->addr, pkt);
            if (hdr == NULL) {
                DEBUG("ndp internal: error allocating ABRO.\n");
                mutex_unlock(&ipv6_iface->mutex);
                gnrc_pktbuf_release(pkt);
                return;
            }
            pkt = hdr;
        }
    }
#endif /* MODULE_GNRC_SIXLOWPAN_ND_ROUTER */
    if (ipv6_iface->flags & GNRC_IPV6_NETIF_FLAGS_ADV_MTU) {
        if ((hdr = gnrc_ndp_opt_mtu_build(ipv6_iface->mtu, pkt)) == NULL) {
            DEBUG("ndp rtr: no space left in packet buffer\n");
            mutex_unlock(&ipv6_iface->mutex);
            gnrc_pktbuf_release(pkt);
            return;
        }
        pkt = hdr;
    }
    if (src == NULL) {
        mutex_unlock(&ipv6_iface->mutex);
        /* get address from source selection algorithm.
         * Only link local addresses may be used (RFC 4861 section 4.1) */
        src = gnrc_ipv6_netif_find_best_src_addr(iface, dst, true);
        mutex_lock(&ipv6_iface->mutex);
    }
    /* add SL2A for source address */
    if (src != NULL) {
        DEBUG(" - SL2A\n");
        uint8_t l2src[8];
        size_t l2src_len;
        /* optimization note: MAY also be omitted to facilitate in-bound load balancing over
         * replicated interfaces.
         * source: https://tools.ietf.org/html/rfc4861#section-6.2.3 */
        l2src_len = _get_l2src(iface, l2src, sizeof(l2src));
        if (l2src_len > 0) {
            /* add source address link-layer address option */
            hdr = gnrc_ndp_opt_sl2a_build(l2src, l2src_len, pkt);

            if (hdr == NULL) {
                DEBUG("ndp internal: error allocating Source Link-layer address option.\n");
                mutex_unlock(&ipv6_iface->mutex);
                gnrc_pktbuf_release(pkt);
                return;
            }
            pkt = hdr;
        }
    }
    if (ipv6_iface->flags & GNRC_IPV6_NETIF_FLAGS_ADV_CUR_HL) {
        cur_hl = ipv6_iface->cur_hl;
    }
    if (ipv6_iface->flags & GNRC_IPV6_NETIF_FLAGS_ADV_REACH_TIME) {

        if (ipv6_iface->reach_time > (3600 * SEC_IN_USEC)) { /* reach_time > 1 hour */
            reach_time = (3600 * SEC_IN_MS);
        }
        else {
            reach_time = ipv6_iface->reach_time / MS_IN_USEC;
        }
    }
    if (ipv6_iface->flags & GNRC_IPV6_NETIF_FLAGS_ADV_RETRANS_TIMER) {
        retrans_timer = ipv6_iface->retrans_timer / MS_IN_USEC;
    }
    if (!fin) {
        adv_ltime = ipv6_iface->adv_ltime;
    }
    mutex_unlock(&ipv6_iface->mutex);
    hdr = gnrc_ndp_rtr_adv_build(cur_hl,
                                 (ipv6_iface->flags & (GNRC_IPV6_NETIF_FLAGS_OTHER_CONF |
                                                       GNRC_IPV6_NETIF_FLAGS_MANAGED)) >> 8,
                                 adv_ltime, reach_time, retrans_timer, pkt);
    if (hdr == NULL) {
        DEBUG("ndp internal: error allocating router advertisement.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    pkt = hdr;
    hdr = _build_headers(iface, pkt, dst, src);
    if (hdr == NULL) {
        DEBUG("ndp internal: error adding lower-layer headers.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    else if (gnrc_netapi_send(gnrc_ipv6_pid, hdr) < 1) {
        DEBUG("ndp internal: unable to send router advertisement\n");
        gnrc_pktbuf_release(hdr);
    }
}
#endif

int gnrc_ndp_internal_sl2a_opt_handle(gnrc_pktsnip_t *pkt, ipv6_hdr_t *ipv6, uint8_t icmpv6_type,
                                      ndp_opt_t *sl2a_opt, uint8_t *l2src)
{
    int sl2a_len = 0;
    uint8_t *sl2a = (uint8_t *)(sl2a_opt + 1);

    if ((sl2a_opt->len == 0) || ipv6_addr_is_unspecified(&ipv6->src)) {
        DEBUG("ndp: invalid source link-layer address option received\n");
        return -EINVAL;
    }

    while (pkt) {
        if (pkt->type == GNRC_NETTYPE_NETIF) {
            gnrc_netif_hdr_t *hdr = pkt->data;
            sl2a_len = hdr->src_l2addr_len;
            break;
        }
        pkt = pkt->next;
    }

#ifdef MODULE_GNRC_SIXLOWPAN_ND
    if ((sl2a_len == 2) || (sl2a_len == 8)) {
        /* The link-layer seems to be IEEE 802.15.4.
         * Determining address length from the option length:
         * https://tools.ietf.org/html/rfc4944#section-8 */
        if (sl2a_opt->len == 1) {
            sl2a_len = 2;
        }
        else if (sl2a_opt->len == 2) {
            sl2a_len = 8;
        }
        else {
            DEBUG("ndp: invalid source link-layer address option received\n");
            return -EINVAL;
        }
    }
#endif

    DEBUG("ndp: received SL2A (link-layer address: %s)\n",
          gnrc_netif_addr_to_str(addr_str, sizeof(addr_str), sl2a, sl2a_len));

    switch (icmpv6_type) {
        case ICMPV6_RTR_SOL:
        case ICMPV6_RTR_ADV:
        case ICMPV6_NBR_SOL:
            if (sl2a_len == 0) {  /* in case there was no source address in l2 */
                sl2a_len = (sl2a_opt->len * 8) - sizeof(ndp_opt_t);

                /* ignore all zeroes at the end for length */
                for (; sl2a[sl2a_len - 1] == 0x00; sl2a_len--);
            }

            memcpy(l2src, sl2a, sl2a_len);
            return sl2a_len;

        default:    /* wrong encapsulating message: silently discard */
            DEBUG("ndp: silently discard sl2a_opt for ICMPv6 message type %"
                  PRIu8 "\n", icmpv6_type);
            return -ENOTSUP;
    }
}

int gnrc_ndp_internal_tl2a_opt_handle(gnrc_pktsnip_t *pkt, ipv6_hdr_t *ipv6,
                                      uint8_t icmpv6_type, ndp_opt_t *tl2a_opt,
                                      uint8_t *l2addr)
{
    uint8_t tl2a_len = 0;
    uint8_t *tl2a = (uint8_t *)(tl2a_opt + 1);

    if ((tl2a_opt->len == 0) || ipv6_addr_is_unspecified(&ipv6->src)) {
        DEBUG("ndp: invalid target link-layer address option received\n");
        return -EINVAL;
    }

    switch (icmpv6_type) {
        case ICMPV6_NBR_ADV:
            while (pkt) {
                if (pkt->type == GNRC_NETTYPE_NETIF) {
                    gnrc_netif_hdr_t *hdr = pkt->data;
                    tl2a_len = hdr->src_l2addr_len;
                    break;
                }
                pkt = pkt->next;
            }
#ifdef MODULE_GNRC_SIXLOWPAN_ND
            if ((tl2a_len == 2) || (tl2a_len == 8)) {
                /* The link-layer seems to be IEEE 802.15.4.
                 * Determining address length from the option length:
                 * https://tools.ietf.org/html/rfc4944#section-8 */
                if (tl2a_opt->len == 1) {
                    tl2a_len = 2;
                }
                else if (tl2a_opt->len == 2) {
                    tl2a_len = 8;
                }
                else {
                    DEBUG("ndp: invalid target link-layer address option received\n");
                    return -EINVAL;
                }
            }
#endif

            if (tl2a_len == 0) {  /* in case there was no source address in l2 */
                tl2a_len = (tl2a_opt->len / 8) - sizeof(ndp_opt_t);

                /* ignore all zeroes at the end for length */
                for (; tl2a[tl2a_len - 1] == 0x00; tl2a_len--);
            }

            DEBUG("ndp: received TL2A (link-layer address: %s)\n",
                  gnrc_netif_addr_to_str(addr_str, sizeof(addr_str), tl2a, tl2a_len));

            memcpy(l2addr, tl2a, tl2a_len);

            return (int)tl2a_len;

        default:    /* wrong encapsulating message: silently discard */
            DEBUG("ndp: silently discard tl2a_opt for ICMPv6 message type %"
                  PRIu8 "\n", icmpv6_type);
            return 0;
    }
}

bool gnrc_ndp_internal_mtu_opt_handle(kernel_pid_t iface, uint8_t icmpv6_type,
                                      ndp_opt_mtu_t *mtu_opt)
{
    gnrc_ipv6_netif_t *if_entry = gnrc_ipv6_netif_get(iface);

    if ((mtu_opt->len != NDP_OPT_MTU_LEN)) {
        DEBUG("ndp: invalid MTU option received\n");
        return false;
    }
    if (icmpv6_type != ICMPV6_RTR_ADV) {
        /* else discard silently */
        return true;
    }
    mutex_lock(&if_entry->mutex);
    if_entry->mtu = byteorder_ntohl(mtu_opt->mtu);
    mutex_unlock(&if_entry->mutex);
    return true;
}

bool gnrc_ndp_internal_pi_opt_handle(kernel_pid_t iface, uint8_t icmpv6_type,
                                     ndp_opt_pi_t *pi_opt)
{
    ipv6_addr_t *prefix;
    gnrc_ipv6_netif_addr_t *netif_addr;

    if ((pi_opt->len != NDP_OPT_PI_LEN)) {
        DEBUG("ndp: invalid PI option received\n");
        return false;
    }
    if (icmpv6_type != ICMPV6_RTR_ADV || ipv6_addr_is_link_local(&pi_opt->prefix)) {
        /* else discard silently */
        return true;
    }
#ifdef MODULE_GNRC_SIXLOWPAN_ND
    if ((gnrc_ipv6_netif_get(iface)->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN) &&
        (pi_opt->flags & NDP_OPT_PI_FLAGS_L)) {
        /* ignore: see https://tools.ietf.org/html/rfc6775#section-5.4 */
        return true;
    }
#endif
    prefix = gnrc_ipv6_netif_find_addr(iface, &pi_opt->prefix);
    if (((prefix == NULL) ||
         (gnrc_ipv6_netif_addr_get(prefix)->prefix_len != pi_opt->prefix_len)) &&
        (pi_opt->valid_ltime.u32 != 0)) {
        ipv6_addr_t pref_addr = IPV6_ADDR_UNSPECIFIED;

        if (pi_opt->flags & NDP_OPT_PI_FLAGS_A) {
            if ((gnrc_netapi_get(iface, NETOPT_IPV6_IID, 0, &pref_addr.u64[1],
                                 sizeof(eui64_t)) < 0)) {
                DEBUG("ndp: could not get IID from interface %d\n", iface);
                return false;
            }
        }

        ipv6_addr_init_prefix(&pref_addr, &pi_opt->prefix, pi_opt->prefix_len);
        prefix = gnrc_ipv6_netif_add_addr(iface, &pref_addr,
                                          pi_opt->prefix_len,
                                          pi_opt->flags & NDP_OPT_PI_FLAGS_MASK);
        if (prefix == NULL) {
            DEBUG("ndp: could not add prefix to interface %d\n", iface);
            return false;
        }
#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
        gnrc_sixlowpan_nd_router_set_rtr_adv(gnrc_ipv6_netif_get(iface), true);
#endif
    }
    netif_addr = gnrc_ipv6_netif_addr_get(prefix);
    if (pi_opt->valid_ltime.u32 == 0) {
        if (prefix != NULL) {
            gnrc_ipv6_netif_remove_addr(iface, &netif_addr->addr);
        }

        return true;
    }
    netif_addr->valid = byteorder_ntohl(pi_opt->valid_ltime);
    netif_addr->preferred = byteorder_ntohl(pi_opt->pref_ltime);
    if (netif_addr->valid != UINT32_MAX) {
        xtimer_set_msg(&netif_addr->valid_timeout,
                       (byteorder_ntohl(pi_opt->valid_ltime) * SEC_IN_USEC),
                       &netif_addr->valid_timeout_msg, thread_getpid());
    }
    /* TODO: preferred lifetime for address auto configuration */
    /* on-link flag MUST stay set if it was */
    netif_addr->flags &= NDP_OPT_PI_FLAGS_L;
    netif_addr->flags |= (pi_opt->flags & NDP_OPT_PI_FLAGS_MASK);
    return true;
}

static size_t _get_l2src(kernel_pid_t iface, uint8_t *l2src, size_t l2src_maxlen)
{
    bool try_long = false;
    int res;
    uint16_t l2src_len;
    /* maximum address length that fits into a minimum length (8) S/TL2A option */
    const uint16_t max_short_len = 6;

    /* try getting source address */
    if ((gnrc_netapi_get(iface, NETOPT_SRC_LEN, 0, &l2src_len,
                         sizeof(l2src_len)) >= 0) &&
        (l2src_len > max_short_len)) {
        try_long = true;
    }

    if (try_long && ((res = gnrc_netapi_get(iface, NETOPT_ADDRESS_LONG, 0,
                                            l2src, l2src_maxlen)) > max_short_len)) {
        l2src_len = (uint16_t)res;
    }
    else if ((res = gnrc_netapi_get(iface, NETOPT_ADDRESS, 0, l2src,
                                    l2src_maxlen)) >= 0) {
        l2src_len = (uint16_t)res;
    }
    else {
        DEBUG("ndp internal: no link-layer address found.\n");
        l2src_len = 0;
    }

    return l2src_len;
}

static gnrc_pktsnip_t *_build_headers(kernel_pid_t iface, gnrc_pktsnip_t *payload,
                                      ipv6_addr_t *dst, ipv6_addr_t *src)
{
    gnrc_pktsnip_t *l2hdr;
    gnrc_pktsnip_t *iphdr = gnrc_ipv6_hdr_build(payload, src, dst);
    if (iphdr == NULL) {
        DEBUG("ndp internal: error allocating IPv6 header.\n");
        return NULL;
    }
    ((ipv6_hdr_t *)iphdr->data)->hl = 255;
    /* add netif header for send interface specification */
    l2hdr = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    if (l2hdr == NULL) {
        DEBUG("ndp internal: error allocating netif header.\n");
        gnrc_pktbuf_remove_snip(iphdr, iphdr);
        return NULL;
    }
    ((gnrc_netif_hdr_t *)l2hdr->data)->if_pid = iface;
    LL_PREPEND(iphdr, l2hdr);
    return l2hdr;
}

/** @} */
