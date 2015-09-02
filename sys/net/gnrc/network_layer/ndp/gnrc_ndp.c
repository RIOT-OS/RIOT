/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup net_gnrc_ndp
 * @{
 *
 * @file
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>
#include <string.h>

#include "byteorder.h"
#include "net/fib.h"
#include "net/ipv6/ext/rh.h"
#include "net/gnrc/icmpv6.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc.h"
#include "random.h"
#include "utlist.h"
#include "thread.h"
#include "vtimer.h"

#include "net/gnrc/ndp/internal.h"

#include "net/gnrc/ndp.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu8 etc. */
#include <inttypes.h>

static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

/* sets an entry to stale if its l2addr differs from the given one or creates it stale if it
 * does not exist */
static void _stale_nc(kernel_pid_t iface, ipv6_addr_t *ipaddr, uint8_t *l2addr,
                      int l2addr_len)
{
    if (l2addr_len != -ENOTSUP) {
        gnrc_ipv6_nc_t *nc_entry = gnrc_ipv6_nc_get(iface, ipaddr);
        if (nc_entry == NULL) {
            gnrc_ipv6_nc_add(iface, ipaddr, l2addr, (uint16_t)l2addr_len,
                             GNRC_IPV6_NC_STATE_STALE);
        }
        else if (((uint16_t)l2addr_len != nc_entry->l2_addr_len) ||
                 (memcmp(l2addr, nc_entry->l2_addr, l2addr_len) != 0)) {
            /* if entry exists but l2 address differs: set */
            nc_entry->l2_addr_len = (uint16_t)l2addr_len;
            memcpy(nc_entry->l2_addr, l2addr, l2addr_len);
            gnrc_ndp_internal_set_state(nc_entry, GNRC_IPV6_NC_STATE_STALE);
        }
    }
}

void gnrc_ndp_nbr_sol_handle(kernel_pid_t iface, gnrc_pktsnip_t *pkt,
                             ipv6_hdr_t *ipv6, ndp_nbr_sol_t *nbr_sol,
                             size_t icmpv6_size)
{
    uint16_t opt_offset = 0;
    uint8_t l2src[GNRC_IPV6_NC_L2_ADDR_MAX];
    uint8_t *buf = ((uint8_t *)nbr_sol) + sizeof(ndp_nbr_sol_t);
    ipv6_addr_t *tgt;
    int sicmpv6_size = (int)icmpv6_size, l2src_len = 0;
    DEBUG("ndp: received neighbor solicitation (src: %s, ",
          ipv6_addr_to_str(addr_str, &ipv6->src, sizeof(addr_str)));
    DEBUG("dst: %s, ",
          ipv6_addr_to_str(addr_str, &ipv6->dst, sizeof(addr_str)));
    DEBUG("tgt: %s)\n",
          ipv6_addr_to_str(addr_str, &nbr_sol->tgt, sizeof(addr_str)));
    /* check validity */
    if ((ipv6->hl != 255) || (nbr_sol->code != 0) ||
        (icmpv6_size < sizeof(ndp_nbr_sol_t)) ||
        ipv6_addr_is_multicast(&nbr_sol->tgt) ||
        (ipv6_addr_is_unspecified(&ipv6->src) &&
         ipv6_addr_is_solicited_node(&ipv6->dst))) {
        DEBUG("ndp: neighbor solicitation was invalid.\n");
        /* ipv6 releases */
        return;
    }
    if ((tgt = gnrc_ipv6_netif_find_addr(iface, &nbr_sol->tgt)) == NULL) {
        DEBUG("ndp: Target address is not to interface %" PRIkernel_pid "\n",
              iface);
        /* ipv6 releases */
        return;
    }
    sicmpv6_size -= sizeof(ndp_nbr_sol_t);
    while (sicmpv6_size > 0) {
        ndp_opt_t *opt = (ndp_opt_t *)(buf + opt_offset);
        switch (opt->type) {
            case NDP_OPT_SL2A:
                if ((l2src_len = gnrc_ndp_internal_sl2a_opt_handle(pkt, ipv6, nbr_sol->type, opt,
                                                                   l2src)) < 0) {
                    /* -ENOTSUP can not happen, since the function only returns this for invalid
                     * message types containing the SL2A. Neighbor solicitations are not an
                     * invalid message type for SL2A. According to that, we don't need to watch
                     * out for that here, but regardless, the source link-layer address option
                     * is invalid. */
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
    _stale_nc(iface, &ipv6->src, l2src, l2src_len);
    gnrc_ndp_internal_send_nbr_adv(iface, tgt, &ipv6->src, ipv6_addr_is_multicast(&ipv6->dst),
                                   NULL);
}

static inline bool _pkt_has_l2addr(gnrc_netif_hdr_t *netif_hdr)
{
    return (netif_hdr != NULL) && (netif_hdr->src_l2addr_len != 0) &&
           (netif_hdr->dst_l2addr_len != 0);
}

void gnrc_ndp_nbr_adv_handle(kernel_pid_t iface, gnrc_pktsnip_t *pkt,
                             ipv6_hdr_t *ipv6, ndp_nbr_adv_t *nbr_adv,
                             size_t icmpv6_size)
{
    uint16_t opt_offset = 0;
    uint8_t *buf = ((uint8_t *)nbr_adv) + sizeof(ndp_nbr_adv_t);
    int l2tgt_len = 0;
    uint8_t l2tgt[GNRC_IPV6_NC_L2_ADDR_MAX];
    int sicmpv6_size = (int)icmpv6_size;
    gnrc_ipv6_nc_t *nc_entry = gnrc_ipv6_nc_get(iface, &nbr_adv->tgt);
    gnrc_pktsnip_t *netif;
    gnrc_netif_hdr_t *netif_hdr = NULL;

    DEBUG("ndp: received neighbor advertisement (src: %s, ",
          ipv6_addr_to_str(addr_str, &ipv6->src, sizeof(addr_str)));
    DEBUG("dst: %s, ",
          ipv6_addr_to_str(addr_str, &ipv6->dst, sizeof(addr_str)));
    DEBUG("tgt: %s)\n",
          ipv6_addr_to_str(addr_str, &nbr_adv->tgt, sizeof(addr_str)));

    /* check validity */
    if ((ipv6->hl != 255) || (nbr_adv->code != 0) ||
        (icmpv6_size < sizeof(ndp_nbr_adv_t)) ||
        ipv6_addr_is_multicast(&nbr_adv->tgt)) {
        DEBUG("ndp: neighbor advertisement was invalid.\n");
        /* ipv6 releases */
        return;
    }

    if (nc_entry == NULL) {
        /* see https://tools.ietf.org/html/rfc4861#section-7.2.5 */
        DEBUG("ndp: no neighbor cache entry found for advertisement's target\n");
        /* ipv6 releases */
        return;
    }

    sicmpv6_size -= sizeof(ndp_nbr_adv_t);

    while (sicmpv6_size > 0) {
        ndp_opt_t *opt = (ndp_opt_t *)(buf + opt_offset);

        switch (opt->type) {
            case NDP_OPT_TL2A:
                if ((l2tgt_len = gnrc_ndp_internal_tl2a_opt_handle(pkt, ipv6, nbr_adv->type, opt, l2tgt)) < 0) {
                    /* invalid target link-layer address option */
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

    LL_SEARCH_SCALAR(pkt, netif, type, GNRC_NETTYPE_NETIF);

    if (netif != NULL) {
        netif_hdr = netif->data;
    }

    if (l2tgt_len != -ENOTSUP) {
        if (gnrc_ipv6_nc_get_state(nc_entry) == GNRC_IPV6_NC_STATE_INCOMPLETE) {
            if (_pkt_has_l2addr(netif_hdr) && (l2tgt_len == 0)) {
                /* link-layer has addresses, but no TLLAO supplied: discard silently
                 * (see https://tools.ietf.org/html/rfc4861#section-7.2.5) */
                return;
            }

            nc_entry->iface = iface;
            nc_entry->l2_addr_len = l2tgt_len;
            memcpy(nc_entry->l2_addr, l2tgt, l2tgt_len);

            if (nbr_adv->flags & NDP_NBR_ADV_FLAGS_S) {
                gnrc_ndp_internal_set_state(nc_entry, GNRC_IPV6_NC_STATE_REACHABLE);
            }
            else {
                gnrc_ndp_internal_set_state(nc_entry, GNRC_IPV6_NC_STATE_STALE);
            }

            if (nbr_adv->flags & NDP_NBR_ADV_FLAGS_R) {
                nc_entry->flags |= GNRC_IPV6_NC_IS_ROUTER;
            }
            else {
                nc_entry->flags &= ~GNRC_IPV6_NC_IS_ROUTER;
                /* TODO: update state of neighbor as router in FIB? */
            }
#ifdef MODULE_GNRC_NDP_NODE
            gnrc_pktqueue_t *queued_pkt;
            while ((queued_pkt = gnrc_pktqueue_remove_head(&nc_entry->pkts)) != NULL) {
                gnrc_netapi_send(gnrc_ipv6_pid, queued_pkt->pkt);
                queued_pkt->pkt = NULL;
            }
#endif
        }
        else {
            /* first or-term: no link-layer, but nc_entry has l2addr,
             * second or-term: different l2addr cached */
            bool l2tgt_changed = false;

            if ((!_pkt_has_l2addr(netif_hdr)) && (l2tgt_len == 0)) {
                /* there was previously a L2 address registered */
                l2tgt_changed = (nc_entry->l2_addr_len != 0);
            }
            /* link-layer has addresses and TLLAO with different address */
            else if (_pkt_has_l2addr(netif_hdr) && (l2tgt_len != 0)) {
                l2tgt_changed = (!(l2tgt_len == nc_entry->l2_addr_len)) &&
                                (memcmp(nc_entry->l2_addr, l2tgt, l2tgt_len) == 0);
            }

            if ((nbr_adv->flags & NDP_NBR_ADV_FLAGS_O) || !l2tgt_changed ||
                (l2tgt_len == 0)) {
                if (l2tgt_len != 0) {
                    nc_entry->iface = iface;
                    nc_entry->l2_addr_len = l2tgt_len;
                    memcpy(nc_entry->l2_addr, l2tgt, l2tgt_len);
                }

                if (nbr_adv->flags & NDP_NBR_ADV_FLAGS_S) {
                    gnrc_ndp_internal_set_state(nc_entry, GNRC_IPV6_NC_STATE_REACHABLE);
                }
                else if (l2tgt_changed) {
                    gnrc_ndp_internal_set_state(nc_entry, GNRC_IPV6_NC_STATE_STALE);
                }

                if (nbr_adv->flags & NDP_NBR_ADV_FLAGS_R) {
                    nc_entry->flags |= GNRC_IPV6_NC_IS_ROUTER;
                }
                else {
                    nc_entry->flags &= ~GNRC_IPV6_NC_IS_ROUTER;
                    /* TODO: update state of neighbor as router in FIB? */
                }
            }
            else if (l2tgt_changed &&
                     gnrc_ipv6_nc_get_state(nc_entry) == GNRC_IPV6_NC_STATE_REACHABLE) {
                gnrc_ndp_internal_set_state(nc_entry, GNRC_IPV6_NC_STATE_STALE);
            }
        }
    }

    return;
}

static inline void _set_reach_time(gnrc_ipv6_netif_t *if_entry, uint32_t mean)
{
    uint32_t reach_time = genrand_uint32_range(GNRC_NDP_MIN_RAND, GNRC_NDP_MAX_RAND);

    if_entry->reach_time_base = mean;
    /* to avoid floating point number computation and have higher value entropy, the
     * boundaries for the random value are multiplied by 10 and we need to account for that */
    reach_time = (reach_time * if_entry->reach_time_base) / 10;
    if_entry->reach_time = timex_set(0, reach_time);
    timex_normalize(&if_entry->reach_time);
}

void gnrc_ndp_rtr_adv_handle(kernel_pid_t iface, gnrc_pktsnip_t *pkt, ipv6_hdr_t *ipv6,
                             ndp_rtr_adv_t *rtr_adv, size_t icmpv6_size)
{
    uint8_t *buf = (uint8_t *)(rtr_adv + 1);
    gnrc_ipv6_nc_t *nc_entry = NULL;
    gnrc_ipv6_netif_t *if_entry = gnrc_ipv6_netif_get(iface);
    uint8_t l2src[GNRC_IPV6_NC_L2_ADDR_MAX];
    int sicmpv6_size = (int)icmpv6_size, l2src_len = 0;
    uint16_t opt_offset = 0;

    assert(if_entry != NULL);
    if (!ipv6_addr_is_link_local(&ipv6->src) ||
        ipv6_addr_is_multicast(&ipv6->src) ||
        (ipv6->hl != 255) || (rtr_adv->code != 0) ||
        (icmpv6_size < sizeof(ndp_rtr_adv_t))) {
        DEBUG("ndp: router advertisement was invalid\n");
        /* ipv6 releases */
        return;
    }
    /* get source from default router list */
    nc_entry = gnrc_ipv6_nc_get(iface, &ipv6->src);
    if (nc_entry == NULL) { /* not in default router list */
        /* create default router list entry */
        nc_entry = gnrc_ipv6_nc_add(iface, &ipv6->src, NULL, 0,
                                    GNRC_IPV6_NC_IS_ROUTER);
        if (nc_entry == NULL) {
            DEBUG("ndp: error on default router list entry creation\n");
            return;
        }
    }
    else if ((nc_entry->flags & GNRC_IPV6_NC_IS_ROUTER) && (byteorder_ntohs(rtr_adv->ltime) == 0)) {
        nc_entry->flags &= ~GNRC_IPV6_NC_IS_ROUTER;
    }
    else {
        nc_entry->flags |= GNRC_IPV6_NC_IS_ROUTER;
    }
    /* set router life timer */
    if (rtr_adv->ltime.u16 != 0) {
        vtimer_remove(&nc_entry->rtr_timeout);
        vtimer_set_msg(&nc_entry->rtr_timeout,
                       timex_set(byteorder_ntohs(rtr_adv->ltime), 0),
                       thread_getpid(), GNRC_NDP_MSG_RTR_TIMEOUT, nc_entry);
    }
    /* set current hop limit from message if available */
    if (rtr_adv->cur_hl != 0) {
        if_entry->cur_hl = rtr_adv->cur_hl;
    }
    /* set flags from message */
    if_entry->flags &= ~GNRC_IPV6_NETIF_FLAGS_RTR_ADV_MASK;
    if_entry->flags |= (rtr_adv->flags << GNRC_IPV6_NETIF_FLAGS_RTR_ADV_POS) &
                       GNRC_IPV6_NETIF_FLAGS_RTR_ADV_MASK;
    /* set reachable time from message if it is not the same as the random base
     * value */
    if ((rtr_adv->reach_time.u32 != 0) &&
        (if_entry->reach_time_base != byteorder_ntohl(rtr_adv->reach_time))) {
        _set_reach_time(if_entry, byteorder_ntohl(rtr_adv->reach_time));
    }
    /* set retransmission timer from message */
    if (rtr_adv->retrans_timer.u32 != 0) {
        if_entry->retrans_timer = timex_set(0, byteorder_ntohl(rtr_adv->retrans_timer));
        timex_normalize(&if_entry->retrans_timer);
    }
    mutex_unlock(&if_entry->mutex);
    sicmpv6_size -= sizeof(ndp_rtr_adv_t);
    /* parse options */
    while (sicmpv6_size > 0) {
        ndp_opt_t *opt = (ndp_opt_t *)(buf + opt_offset);
        switch (opt->type) {
            case NDP_OPT_SL2A:
                if ((l2src_len = gnrc_ndp_internal_sl2a_opt_handle(pkt, ipv6, rtr_adv->type, opt,
                                                                   l2src)) < 0) {
                    /* -ENOTSUP can not happen */
                    /* invalid source link-layer address option */
                    return;
                }
                break;
            case NDP_OPT_MTU:
                if (!gnrc_ndp_internal_mtu_opt_handle(iface, rtr_adv->type, (ndp_opt_mtu_t *)opt)) {
                    /* invalid MTU option */
                    return;
                }
                break;
            case NDP_OPT_PI:
                if (!gnrc_ndp_internal_pi_opt_handle(iface, rtr_adv->type, (ndp_opt_pi_t *)opt)) {
                    /* invalid prefix information option */
                    return;
                }
                break;
        }
    }
    _stale_nc(iface, &ipv6->src, l2src, l2src_len);
}

void gnrc_ndp_retrans_nbr_sol(gnrc_ipv6_nc_t *nc_entry)
{
    if ((gnrc_ipv6_nc_get_state(nc_entry) == GNRC_IPV6_NC_STATE_INCOMPLETE) ||
        (gnrc_ipv6_nc_get_state(nc_entry) == GNRC_IPV6_NC_STATE_PROBE)) {
        if (nc_entry->probes_remaining > 1) {
            ipv6_addr_t dst;

            DEBUG("ndp: Retransmit neighbor solicitation for %s\n",
                  ipv6_addr_to_str(addr_str, &nc_entry->ipv6_addr, sizeof(addr_str)));

            /* retransmit neighbor solicatation */
            if (gnrc_ipv6_nc_get_state(nc_entry) == GNRC_IPV6_NC_STATE_INCOMPLETE) {
                ipv6_addr_set_solicited_nodes(&dst, &nc_entry->ipv6_addr);
            }
            else {
                dst.u64[0] = nc_entry->ipv6_addr.u64[0];
                dst.u64[1] = nc_entry->ipv6_addr.u64[1];
            }

            nc_entry->probes_remaining--;

            if (nc_entry->iface == KERNEL_PID_UNDEF) {
                timex_t t = { 0, GNRC_NDP_RETRANS_TIMER };
                kernel_pid_t ifs[GNRC_NETIF_NUMOF];
                size_t ifnum = gnrc_netif_get(ifs);

                for (size_t i = 0; i < ifnum; i++) {
                    gnrc_ndp_internal_send_nbr_sol(ifs[i], &nc_entry->ipv6_addr, &dst);
                }

                vtimer_remove(&nc_entry->nbr_sol_timer);
                vtimer_set_msg(&nc_entry->nbr_sol_timer, t, gnrc_ipv6_pid,
                               GNRC_NDP_MSG_NBR_SOL_RETRANS, nc_entry);
            }
            else {
                gnrc_ipv6_netif_t *ipv6_iface = gnrc_ipv6_netif_get(nc_entry->iface);

                gnrc_ndp_internal_send_nbr_sol(nc_entry->iface, &nc_entry->ipv6_addr, &dst);

                mutex_lock(&ipv6_iface->mutex);
                vtimer_remove(&nc_entry->nbr_sol_timer);
                vtimer_set_msg(&nc_entry->nbr_sol_timer,
                               ipv6_iface->retrans_timer, gnrc_ipv6_pid,
                               GNRC_NDP_MSG_NBR_SOL_RETRANS, nc_entry);
                mutex_unlock(&ipv6_iface->mutex);
            }
        }
        else if (nc_entry->probes_remaining <= 1) {
            DEBUG("ndp: Remove nc entry %s for interface %" PRIkernel_pid "\n",
                  ipv6_addr_to_str(addr_str, &nc_entry->ipv6_addr, sizeof(addr_str)),
                  nc_entry->iface);

            gnrc_ipv6_nc_remove(nc_entry->iface, &nc_entry->ipv6_addr);
        }
    }
}

void gnrc_ndp_state_timeout(gnrc_ipv6_nc_t *nc_entry)
{
    switch (gnrc_ipv6_nc_get_state(nc_entry)) {
        case GNRC_IPV6_NC_STATE_REACHABLE:
            gnrc_ndp_internal_set_state(nc_entry, GNRC_IPV6_NC_STATE_STALE);
            break;

        case GNRC_IPV6_NC_STATE_DELAY:
            gnrc_ndp_internal_set_state(nc_entry, GNRC_IPV6_NC_STATE_PROBE);
            break;

        default:
            break;
    }
}

void gnrc_ndp_netif_add(gnrc_ipv6_netif_t *iface)
{
    /* set default values */
    mutex_lock(&iface->mutex);
    _set_reach_time(iface, GNRC_NDP_REACH_TIME);
    iface->retrans_timer = timex_set(0, GNRC_NDP_RETRANS_TIMER);
    timex_normalize(&iface->retrans_timer);
    mutex_unlock(&iface->mutex);
}

void gnrc_ndp_netif_remove(gnrc_ipv6_netif_t *iface)
{
    /* TODO */
    (void) iface;
}

gnrc_pktsnip_t *gnrc_ndp_nbr_sol_build(ipv6_addr_t *tgt, gnrc_pktsnip_t *options)
{
    gnrc_pktsnip_t *pkt;

    DEBUG("ndp: building neighbor solicitation message\n");

    if (ipv6_addr_is_multicast(tgt)) {
        DEBUG("ndp: tgt must not be multicast\n");
        return NULL;
    }

    pkt = gnrc_icmpv6_build(options, ICMPV6_NBR_SOL, 0, sizeof(ndp_nbr_sol_t));

    if (pkt != NULL) {
        ndp_nbr_sol_t *nbr_sol = pkt->data;
        nbr_sol->resv.u32 = 0;
        nbr_sol->tgt.u64[0].u64 = tgt->u64[0].u64;
        nbr_sol->tgt.u64[1].u64 = tgt->u64[1].u64;
    }

    return pkt;
}

gnrc_pktsnip_t *gnrc_ndp_nbr_adv_build(uint8_t flags, ipv6_addr_t *tgt,
                                       gnrc_pktsnip_t *options)
{
    gnrc_pktsnip_t *pkt;

    DEBUG("ndp: building neighbor advertisement message\n");

    if (ipv6_addr_is_multicast(tgt)) {
        DEBUG("ndp: tgt must not be multicast\n");
        return NULL;
    }

    pkt = gnrc_icmpv6_build(options, ICMPV6_NBR_ADV, 0, sizeof(ndp_nbr_adv_t));

    if (pkt != NULL) {
        ndp_nbr_adv_t *nbr_adv = pkt->data;
        nbr_adv->flags = (flags & NDP_NBR_ADV_FLAGS_MASK);
        nbr_adv->resv[0] = nbr_adv->resv[1] = nbr_adv->resv[2] = 0;
        nbr_adv->tgt.u64[0].u64 = tgt->u64[0].u64;
        nbr_adv->tgt.u64[1].u64 = tgt->u64[1].u64;
    }

    return pkt;
}

gnrc_pktsnip_t *gnrc_ndp_rtr_sol_build(gnrc_pktsnip_t *options)
{
    gnrc_pktsnip_t *pkt;
    DEBUG("ndp: building router solicitation message\n");
    pkt = gnrc_icmpv6_build(options, ICMPV6_RTR_SOL, 0, sizeof(ndp_rtr_sol_t));
    if (pkt != NULL) {
        ndp_rtr_sol_t *rtr_sol = pkt->data;
        rtr_sol->resv.u32 = 0;
    }
    return pkt;
}

static inline size_t _ceil8(uint8_t length)
{
    /* NDP options use units of 8 byte for there length field, so round up */
    return (length + 7U) & 0xf8U;
}

gnrc_pktsnip_t *gnrc_ndp_opt_build(uint8_t type, size_t size, gnrc_pktsnip_t *next)
{
    ndp_opt_t *opt;
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(next, NULL, _ceil8(size), GNRC_NETTYPE_UNDEF);

    if (pkt == NULL) {
        DEBUG("ndp: no space left in packet buffer\n");
        return NULL;
    }

    opt = pkt->data;

    opt->type = type;
    opt->len = (uint8_t)(pkt->size / 8);

    return pkt;
}

static inline gnrc_pktsnip_t *_opt_l2a_build(uint8_t type, const uint8_t *l2addr,
                                             uint8_t l2addr_len, gnrc_pktsnip_t *next)
{
    gnrc_pktsnip_t *pkt = gnrc_ndp_opt_build(type, sizeof(ndp_opt_t) + l2addr_len, next);

    if (pkt != NULL) {
        ndp_opt_t *l2a_opt = pkt->data;

        memset(l2a_opt + 1, 0, pkt->size - sizeof(ndp_opt_t));
        memcpy(l2a_opt + 1, l2addr, l2addr_len);
    }

    return pkt;
}

gnrc_pktsnip_t *gnrc_ndp_opt_sl2a_build(const uint8_t *l2addr, uint8_t l2addr_len,
                                        gnrc_pktsnip_t *next)
{
    DEBUG("ndp: building source link-layer address option (l2addr: %s)\n",
          gnrc_netif_addr_to_str(addr_str, sizeof(addr_str), l2addr, l2addr_len));

    return _opt_l2a_build(NDP_OPT_SL2A, l2addr, l2addr_len, next);
}

gnrc_pktsnip_t *gnrc_ndp_opt_tl2a_build(const uint8_t *l2addr, uint8_t l2addr_len,
                                        gnrc_pktsnip_t *next)
{
    DEBUG("ndp: building target link-layer address option (l2addr: %s)\n",
          gnrc_netif_addr_to_str(addr_str, sizeof(addr_str), l2addr, l2addr_len));

    return _opt_l2a_build(NDP_OPT_TL2A, l2addr, l2addr_len, next);
}

/**
 * @}
 */
