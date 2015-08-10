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

#include "net/ng_ipv6.h"
#include "net/ng_ndp.h"
#include "random.h"
#include "timex.h"
#include "vtimer.h"

#include "net/ng_ndp/internal.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

static ng_ipv6_nc_t *_last_router = NULL;   /* last router chosen as default
                                             * router. Only used if reachability
                                             * is suspect (i. e. incomplete or
                                             * not at all) */

/**
 * @brief   Get L2 address from interface
 */
static uint16_t _get_l2src(uint8_t *l2src, size_t l2src_size, kernel_pid_t iface);

/**
 * @brief   Sends @ref NG_NETAPI_MSG_TYPE_SND delayed.
 *
 * @param[in] t         Timer for the delay.
 * @param[in] interval  Delay interval.
 * @param[in] pkt       Packet to send delayed.
 */
static inline void _send_delayed(vtimer_t *t, timex_t interval, ng_pktsnip_t *pkt)
{
    vtimer_remove(t);
    vtimer_set_msg(t, interval, ng_ipv6_pid, NG_NETAPI_MSG_TYPE_SND, pkt);
}


ipv6_addr_t *ng_ndp_internal_default_router(void)
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

void ng_ndp_internal_set_state(ng_ipv6_nc_t *nc_entry, uint8_t state)
{
    ng_ipv6_netif_t *ipv6_iface;
    timex_t t = { NG_NDP_FIRST_PROBE_DELAY, 0 };

    nc_entry->flags &= ~NG_IPV6_NC_STATE_MASK;
    nc_entry->flags |= state;

    DEBUG("ndp internal: set %s state to ",
          ipv6_addr_to_str(addr_str, &nc_entry->ipv6_addr, sizeof(addr_str)));

    switch (state) {
        case NG_IPV6_NC_STATE_REACHABLE:
            ipv6_iface = ng_ipv6_netif_get(nc_entry->iface);
            DEBUG("REACHABLE (reachable time = %" PRIu32 ".%06" PRIu32 ")\n",
                  ipv6_iface->reach_time.seconds,
                  ipv6_iface->reach_time.microseconds);
            t = ipv6_iface->reach_time;

            /* we intentionally fall through here to set the desired timeout t */
        case NG_IPV6_NC_STATE_DELAY:
#if ENABLE_DEBUG
            if (state == NG_IPV6_NC_STATE_DELAY) {
                DEBUG("DELAY (probe with unicast NS in %u seconds)\n",
                      NG_NDP_FIRST_PROBE_DELAY);
            }
#endif
            vtimer_remove(&nc_entry->nbr_sol_timer);
            vtimer_set_msg(&nc_entry->nbr_sol_timer, t, ng_ipv6_pid,
                           NG_NDP_MSG_NC_STATE_TIMEOUT, nc_entry);
            break;

        case NG_IPV6_NC_STATE_PROBE:
            ipv6_iface = ng_ipv6_netif_get(nc_entry->iface);

            nc_entry->probes_remaining = NG_NDP_MAX_UC_NBR_SOL_NUMOF;
            DEBUG("PROBE (probe with %" PRIu8 " unicast NS every %" PRIu32
                  ".%06" PRIu32 " seconds)\n", nc_entry->probes_remaining,
                  ipv6_iface->retrans_timer.seconds,
                  ipv6_iface->retrans_timer.microseconds);

            ng_ndp_internal_send_nbr_sol(nc_entry->iface, &nc_entry->ipv6_addr,
                                         &nc_entry->ipv6_addr);

            mutex_lock(&ipv6_iface->mutex);
            vtimer_remove(&nc_entry->nbr_sol_timer);
            vtimer_set_msg(&nc_entry->nbr_sol_timer,
                           ipv6_iface->retrans_timer, ng_ipv6_pid,
                           NG_NDP_MSG_NBR_SOL_RETRANS, nc_entry);
            mutex_unlock(&ipv6_iface->mutex);
            break;

#ifdef ENABLE_DEBUG
        case NG_IPV6_NC_STATE_STALE:
            DEBUG("STALE (go into DELAY on next packet)\n");
            break;
#endif

        default:
            DEBUG("errorneous or unknown\n");
            break;
    }
}

void ng_ndp_internal_send_nbr_adv(kernel_pid_t iface, ipv6_addr_t *tgt,
                                  ipv6_addr_t *dst, bool supply_tl2a)
{
    ng_pktsnip_t *hdr, *pkt = NULL;
    uint8_t adv_flags = 0;

    DEBUG("ndp internal: send neighbor advertisement (iface: %" PRIkernel_pid ", tgt: %s, ",
          iface, ipv6_addr_to_str(addr_str, tgt, sizeof(addr_str)));
    DEBUG("dst: %s, supply_tl2a: %d)\n",
          ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)), supply_tl2a);

    if (ng_ipv6_netif_get(iface)->flags & NG_IPV6_NETIF_FLAGS_ROUTER) {
        adv_flags |= NG_NDP_NBR_ADV_FLAGS_R;
    }

    if (ipv6_addr_is_unspecified(dst)) {
        ipv6_addr_set_all_nodes_multicast(dst, IPV6_ADDR_MCAST_SCP_LINK_LOCAL);
    }
    else {
        adv_flags |= NG_NDP_NBR_ADV_FLAGS_S;
    }

    if (supply_tl2a) {
        uint8_t l2src[8];
        uint16_t l2src_len;
        /* we previously checked if we are the target, so we can take our L2src */
        l2src_len = _get_l2src(l2src, sizeof(l2src), iface);

        if (l2src_len > 0) {
            /* add target address link-layer address option */
            pkt = ng_ndp_opt_tl2a_build(l2src, l2src_len, NULL);

            if (pkt == NULL) {
                DEBUG("ndp internal: error allocating Target Link-layer address option.\n");
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
        DEBUG("ndp internal: error allocating Neighbor advertisement.\n");
        ng_pktbuf_release(pkt);
        return;
    }

    pkt = hdr;
    hdr = ng_ipv6_hdr_build(pkt, NULL, 0, (uint8_t *)dst,
                            sizeof(ipv6_addr_t));

    if (hdr == NULL) {
        DEBUG("ndp internal: error allocating IPv6 header.\n");
        ng_pktbuf_release(pkt);
        return;
    }

    ((ipv6_hdr_t *)hdr->data)->hl = 255;

    pkt = hdr;
    /* add netif header for send interface specification */
    hdr = ng_netif_hdr_build(NULL, 0, NULL, 0);

    if (hdr == NULL) {
        DEBUG("ndp internal: error allocating netif header.\n");
        return;
    }

    ((ng_netif_hdr_t *)hdr->data)->if_pid = iface;

    LL_PREPEND(pkt, hdr);

    if (ng_ipv6_netif_addr_is_non_unicast(tgt)) {
        /* avoid collision for anycast addresses
         * (see https://tools.ietf.org/html/rfc4861#section-7.2.7) */
        timex_t delay = { 0, genrand_uint32_range(0, NG_NDP_MAX_AC_TGT_DELAY * SEC_IN_USEC) };
        timex_normalize(&delay);
        ng_ipv6_nc_t *nc_entry = ng_ipv6_nc_get(iface, tgt);
        DEBUG("ndp internal: delay neighbor advertisement for %" PRIu32 " sec.",
              delay.seconds);

        /* nc_entry must be set so no need to check it */
        _send_delayed(&nc_entry->nbr_adv_timer, delay, pkt);
    }
    else {
        ng_netapi_send(ng_ipv6_pid, pkt);
    }
}

void ng_ndp_internal_send_nbr_sol(kernel_pid_t iface, ipv6_addr_t *tgt,
                                  ipv6_addr_t *dst)
{
    ng_pktsnip_t *hdr, *pkt = NULL;
    ipv6_addr_t *src = NULL;
    size_t src_len = 0;

    DEBUG("ndp internal: send neighbor solicitation (iface: %" PRIkernel_pid ", tgt: %s, ",
          iface, ipv6_addr_to_str(addr_str, tgt, sizeof(addr_str)));
    DEBUG("dst: %s)\n", ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)));

    /* check if there is a fitting source address to target */
    if ((src = ng_ipv6_netif_find_best_src_addr(iface, tgt)) != NULL) {
        uint8_t l2src[8];
        uint16_t l2src_len;
        src_len = sizeof(ipv6_addr_t);
        l2src_len = _get_l2src(l2src, sizeof(l2src), iface);

        if (l2src_len > 0) {
            /* add source address link-layer address option */
            pkt = ng_ndp_opt_sl2a_build(l2src, l2src_len, NULL);

            if (pkt == NULL) {
                DEBUG("ndp internal: error allocating Source Link-layer address option.\n");
                ng_pktbuf_release(pkt);
                return;
            }
        }
    }

    hdr = ng_ndp_nbr_sol_build(tgt, pkt);

    if (hdr == NULL) {
        DEBUG("ndp internal: error allocating Neighbor solicitation.\n");
        ng_pktbuf_release(pkt);
        return;
    }

    pkt = hdr;
    hdr = ng_ipv6_hdr_build(pkt, (uint8_t *)src, src_len, (uint8_t *)dst,
                            sizeof(ipv6_addr_t));

    if (hdr == NULL) {
        DEBUG("ndp internal: error allocating IPv6 header.\n");
        ng_pktbuf_release(pkt);
        return;
    }

    ((ipv6_hdr_t *)hdr->data)->hl = 255;

    pkt = hdr;
    /* add netif header for send interface specification */
    hdr = ng_netif_hdr_build(NULL, 0, NULL, 0);

    if (hdr == NULL) {
        DEBUG("ndp internal: error allocating netif header.\n");
        return;
    }

    ((ng_netif_hdr_t *)hdr->data)->if_pid = iface;

    LL_PREPEND(pkt, hdr);

    ng_netapi_send(ng_ipv6_pid, pkt);
}

bool ng_ndp_internal_sl2a_opt_handle(kernel_pid_t iface, ng_pktsnip_t *pkt,
                                     ipv6_hdr_t *ipv6, uint8_t icmpv6_type,
                                     ng_ndp_opt_t *sl2a_opt)
{
    ng_ipv6_nc_t *nc_entry = NULL;
    uint8_t sl2a_len = 0;
    uint8_t *sl2a = (uint8_t *)(sl2a_opt + 1);

    if ((sl2a_opt->len == 0) || ipv6_addr_is_unspecified(&ipv6->src)) {
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

    DEBUG("ndp: received SL2A (link-layer address: %s)\n",
          ng_netif_addr_to_str(addr_str, sizeof(addr_str), sl2a, sl2a_len));

    switch (icmpv6_type) {
        case ICMPV6_NBR_SOL:
            nc_entry = ng_ipv6_nc_get(iface, &ipv6->src);

            if (nc_entry != NULL) {
                if ((sl2a_len != nc_entry->l2_addr_len) ||
                    (memcmp(sl2a, nc_entry->l2_addr, sl2a_len) != 0)) {
                    /* if entry exists but l2 address differs: set */
                    nc_entry->l2_addr_len = sl2a_len;
                    memcpy(nc_entry->l2_addr, sl2a, sl2a_len);

                    ng_ndp_internal_set_state(nc_entry, NG_IPV6_NC_STATE_STALE);
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

int ng_ndp_internal_tl2a_opt_handle(ng_pktsnip_t *pkt, ipv6_hdr_t *ipv6,
                                    uint8_t icmpv6_type, ng_ndp_opt_t *tl2a_opt,
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

            DEBUG("ndp: received TL2A (link-layer address: %s)\n",
                  ng_netif_addr_to_str(addr_str, sizeof(addr_str), tl2a, tl2a_len));

            memcpy(l2addr, tl2a, tl2a_len);

            return (int)tl2a_len;

        default:    /* wrong encapsulating message: silently discard */
            DEBUG("ndp: silently discard tl2a_opt for ICMPv6 message type %"
                  PRIu8 "\n", icmpv6_type);
            return 0;
    }
}
static uint16_t _get_l2src(uint8_t *l2src, size_t l2src_size, kernel_pid_t iface)
{
    bool try_long = false;
    int res;
    uint16_t l2src_len;
    /* maximum address length that fits into a minimum length (8) S/TL2A option */
    const uint16_t max_short_len = 6;

    /* try getting source address */
    if ((ng_netapi_get(iface, NETOPT_SRC_LEN, 0, &l2src_len,
                       sizeof(l2src_len)) >= 0) &&
        (l2src_len > max_short_len)) {
        try_long = true;
    }

    if (try_long && ((res = ng_netapi_get(iface, NETOPT_ADDRESS_LONG, 0,
                                          l2src, l2src_size)) > max_short_len)) {
        l2src_len = (uint16_t)res;
    }
    else if ((res = ng_netapi_get(iface, NETOPT_ADDRESS, 0, l2src,
                                  l2src_size)) >= 0) {
        l2src_len = (uint16_t)res;
    }
    else {
        DEBUG("ndp internal: no link-layer address found.\n");
        l2src_len = 0;
    }

    return l2src_len;
}

/** @} */
