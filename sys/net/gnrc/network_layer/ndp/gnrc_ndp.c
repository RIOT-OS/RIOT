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
#include "net/ieee802154.h"
#include "net/ipv6/ext/rh.h"
#include "net/gnrc/icmpv6.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/sixlowpan/nd.h"
#include "net/gnrc.h"
#include "net/sixlowpan/nd.h"
#include "random.h"
#include "utlist.h"
#include "thread.h"
#include "xtimer.h"

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
#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
            /* tentative type see https://tools.ietf.org/html/rfc6775#section-6.3 */
            gnrc_ipv6_netif_t *ipv6_iface = gnrc_ipv6_netif_get(iface);
            if ((ipv6_iface->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN) &&
                (ipv6_iface->flags & GNRC_IPV6_NETIF_FLAGS_ROUTER)) {
                if ((nc_entry = gnrc_ipv6_nc_add(iface, ipaddr, l2addr,
                                                 (uint16_t)l2addr_len,
                                                 GNRC_IPV6_NC_STATE_STALE |
                                                 GNRC_IPV6_NC_TYPE_TENTATIVE)) != NULL) {
                    xtimer_set_msg(&nc_entry->type_timeout,
                                   (GNRC_SIXLOWPAN_ND_TENTATIVE_NCE_LIFETIME * SEC_IN_USEC),
                                   &nc_entry->type_timeout_msg,
                                   gnrc_ipv6_pid);
                }
                return;
            }
#endif
            gnrc_ipv6_nc_add(iface, ipaddr, l2addr, (uint16_t)l2addr_len,
                             GNRC_IPV6_NC_STATE_STALE);
        }
#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
        /* unreachable set in gnrc_ndp_retrans_nbr_sol() will just be staled */
        else if (gnrc_ipv6_nc_get_state(nc_entry) == GNRC_IPV6_NC_STATE_UNREACHABLE) {
            gnrc_ndp_internal_set_state(nc_entry, GNRC_IPV6_NC_STATE_STALE);
        }
#endif
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
    ipv6_addr_t *tgt, nbr_adv_dst;
    gnrc_pktsnip_t *nbr_adv_opts = NULL;
#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
    ndp_opt_t *sl2a_opt = NULL;
    sixlowpan_nd_opt_ar_t *ar_opt = NULL;
#endif
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
#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
                sl2a_opt = opt;
                break;
            case NDP_OPT_AR:
                /* actually handling at the end of the function (see below) */
                ar_opt = (sixlowpan_nd_opt_ar_t *)opt;
#endif
                break;
            default:
                /* silently discard all other options */
                break;
        }
        opt_offset += (opt->len * 8);
        sicmpv6_size -= (opt->len * 8);

#if ENABLE_DEBUG
        if (sicmpv6_size < 0) {
            DEBUG("ndp: Option parsing out of sync.\n");
        }
#endif
    }
#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
    gnrc_ipv6_netif_t *ipv6_iface = gnrc_ipv6_netif_get(iface);
    assert(ipv6_iface != NULL);
    if ((sl2a_opt != NULL) && (ar_opt != NULL) &&
        (ipv6_iface->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN) &&
        (ipv6_iface->flags & GNRC_IPV6_NETIF_FLAGS_ROUTER)) {
        uint8_t status = gnrc_sixlowpan_nd_opt_ar_handle(iface, ipv6,
                                                         nbr_sol->type,
                                                         &ipv6->src, ar_opt,
                                                         l2src, l2src_len);
        /* check for multihop DAD return */
        nbr_adv_opts = gnrc_sixlowpan_nd_opt_ar_build(status, GNRC_SIXLOWPAN_ND_AR_LTIME,
                                                      &ar_opt->eui64, NULL);
        if (status == 0) {
            memcpy(&nbr_adv_dst, &ipv6->src, sizeof(ipv6_addr_t));
        }
        else {
            /* see https://tools.ietf.org/html/rfc6775#section-6.5.2 */
            eui64_t iid;
            ieee802154_get_iid(&iid, ar_opt->eui64.uint8, sizeof(eui64_t));
            ipv6_addr_set_iid(&nbr_adv_dst, iid.uint64.u64);
            ipv6_addr_set_link_local_prefix(&nbr_adv_dst);
        }
    }
    else {  /* gnrc_sixlowpan_nd_opt_ar_handle updates neighbor cache */
        _stale_nc(iface, &ipv6->src, l2src, l2src_len);
        memcpy(&nbr_adv_dst, &ipv6->src, sizeof(ipv6_addr_t));
    }
#else
    _stale_nc(iface, &ipv6->src, l2src, l2src_len);
    memcpy(&nbr_adv_dst, &ipv6->src, sizeof(ipv6_addr_t));
#endif
    gnrc_ndp_internal_send_nbr_adv(iface, tgt, &nbr_adv_dst, ipv6_addr_is_multicast(&ipv6->dst),
                                   nbr_adv_opts);
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
#ifdef MODULE_GNRC_SIXLOWPAN_ND
            case NDP_OPT_AR:
                /* address registration option is always ignored when invalid */
                gnrc_sixlowpan_nd_opt_ar_handle(iface, ipv6, nbr_adv->type,
                                                &nbr_adv->tgt,
                                                (sixlowpan_nd_opt_ar_t *)opt,
                                                NULL, 0);
                break;
#endif
            default:
                /* silently discard all other options */
                break;
        }

        opt_offset += (opt->len * 8);
        sicmpv6_size -= (opt->len * 8);

#if ENABLE_DEBUG
        if (sicmpv6_size < 0) {
            DEBUG("ndp: Option parsing out of sync.\n");
        }
#endif
    }

    LL_SEARCH_SCALAR(pkt, netif, type, GNRC_NETTYPE_NETIF);

    if (netif != NULL) {
        netif_hdr = netif->data;
    }

    if (l2tgt_len != -ENOTSUP) {
#ifdef MODULE_GNRC_SIXLOWPAN_ND
        /* check if entry wasn't removed by ARO, ideally there should not be any TL2A in here */
        nc_entry = gnrc_ipv6_nc_get(iface, &nbr_adv->tgt);
        if (nc_entry == NULL) {
            return;
        }
#endif
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
                if (gnrc_netapi_send(gnrc_ipv6_pid, queued_pkt->pkt) < 1) {
                    DEBUG("ndp: unable to send queued packet\n");
                    gnrc_pktbuf_release(queued_pkt->pkt);
                }
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

#if (defined(MODULE_GNRC_NDP_ROUTER) || defined(MODULE_GNRC_SIXLOWPAN_ND_ROUTER))
void gnrc_ndp_rtr_sol_handle(kernel_pid_t iface, gnrc_pktsnip_t *pkt,
                             ipv6_hdr_t *ipv6, ndp_rtr_sol_t *rtr_sol,
                             size_t icmpv6_size)
{
    gnrc_ipv6_netif_t *if_entry = gnrc_ipv6_netif_get(iface);

    if (if_entry->flags & GNRC_IPV6_NETIF_FLAGS_ROUTER) {
        int sicmpv6_size = (int)icmpv6_size, l2src_len = 0;
        uint8_t l2src[GNRC_IPV6_NC_L2_ADDR_MAX];
        uint16_t opt_offset = 0;
        uint8_t *buf = (uint8_t *)(rtr_sol + 1);
        /* check validity */
        if ((ipv6->hl != 255) || (rtr_sol->code != 0) ||
            (icmpv6_size < sizeof(ndp_rtr_sol_t))) {
            DEBUG("ndp: router solicitation was invalid\n");
            return;
        }
        sicmpv6_size -= sizeof(ndp_rtr_sol_t);
        while (sicmpv6_size > 0) {
            ndp_opt_t *opt = (ndp_opt_t *)(buf + opt_offset);

            switch (opt->type) {
                case NDP_OPT_SL2A:
                    if ((l2src_len = gnrc_ndp_internal_sl2a_opt_handle(pkt, ipv6, rtr_sol->type, opt,
                                                                       l2src)) < 0) {
                        /* -ENOTSUP can not happen */
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

#if ENABLE_DEBUG
            if (sicmpv6_size < 0) {
                DEBUG("ndp: Option parsing out of sync.\n");
            }
#endif
        }
        _stale_nc(iface, &ipv6->src, l2src, l2src_len);
        /* send delayed */
        if (if_entry->flags & GNRC_IPV6_NETIF_FLAGS_RTR_ADV) {
            uint32_t delay;
            uint32_t ms = GNRC_NDP_MAX_RTR_ADV_DELAY;
#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
            if (if_entry->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN) {
                ms = GNRC_SIXLOWPAN_ND_MAX_RTR_ADV_DELAY;
            }
#endif
            delay = genrand_uint32_range(0, ms);
            xtimer_remove(&if_entry->rtr_adv_timer);
#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
            /* in case of a 6LBR we have to check if the interface is actually
             * the 6lo interface */
            if (if_entry->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN) {
                gnrc_ipv6_nc_t *nc_entry = gnrc_ipv6_nc_get(iface, &ipv6->src);
                if (nc_entry != NULL) {
                    if_entry->rtr_adv_msg.type = GNRC_NDP_MSG_RTR_ADV_SIXLOWPAN_DELAY;
                    if_entry->rtr_adv_msg.content.ptr = (char *) nc_entry;
                    xtimer_set_msg(&if_entry->rtr_adv_timer, delay, &if_entry->rtr_adv_msg,
                                   gnrc_ipv6_pid);
                }
            }
#elif defined(MODULE_GNRC_NDP_ROUTER) || defined(MODULE_GNRC_SIXLOWPAN_ND_BORDER_ROUTER)
            if (ipv6_addr_is_unspecified(&ipv6->src)) {
                /* either multicast, if source unspecified */
                if_entry->rtr_adv_msg.type = GNRC_NDP_MSG_RTR_ADV_RETRANS;
                if_entry->rtr_adv_msg.content.ptr = (char *) if_entry;
                xtimer_set_msg(&if_entry->rtr_adv_timer, delay, &if_entry->rtr_adv_msg,
                               gnrc_ipv6_pid);
            }
            else {
                /* or unicast, if source is known */
                /* XXX: can't just use GNRC_NETAPI_MSG_TYPE_SND, since the next retransmission
                 * must also be set. */
                gnrc_ipv6_nc_t *nc_entry = gnrc_ipv6_nc_get(iface, &ipv6->src);
                xtimer_set_msg(&nc_entry->rtr_adv_timer, delay, &nc_entry->rtr_adv_msg,
                               gnrc_ipv6_pid);
            }
#endif
        }
    }
    /* otherwise ignore silently */
}
#endif

static inline void _set_reach_time(gnrc_ipv6_netif_t *if_entry, uint32_t mean)
{
    uint32_t reach_time = genrand_uint32_range(GNRC_NDP_MIN_RAND, GNRC_NDP_MAX_RAND);

    if_entry->reach_time_base = mean;
    /* to avoid floating point number computation and have higher value entropy, the
     * boundaries for the random value are multiplied by 10 and we need to account for that */
    reach_time = (reach_time * if_entry->reach_time_base) / 10;
    if_entry->reach_time = reach_time;
}

void gnrc_ndp_rtr_adv_handle(kernel_pid_t iface, gnrc_pktsnip_t *pkt, ipv6_hdr_t *ipv6,
                             ndp_rtr_adv_t *rtr_adv, size_t icmpv6_size)
{
    uint8_t *buf = (uint8_t *)(rtr_adv + 1);
    gnrc_ipv6_nc_t *nc_entry = NULL;
    gnrc_ipv6_netif_t *if_entry = gnrc_ipv6_netif_get(iface);
    uint8_t l2src[GNRC_IPV6_NC_L2_ADDR_MAX];
#ifdef MODULE_GNRC_SIXLOWPAN_ND
    uint32_t next_rtr_sol = 0;
#endif
    int sicmpv6_size = (int)icmpv6_size, l2src_len = 0;
    uint16_t opt_offset = 0;

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
        uint16_t ltime = byteorder_ntohs(rtr_adv->ltime);
#ifdef MODULE_GNRC_SIXLOWPAN_ND
        next_rtr_sol = ltime;
#endif
        xtimer_set_msg(&nc_entry->rtr_timeout, (ltime * SEC_IN_USEC),
                       &nc_entry->rtr_timeout_msg, thread_getpid());
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
#ifdef MODULE_GNRC_SIXLOWPAN_ND
                uint32_t valid_ltime = byteorder_ntohl(((ndp_opt_pi_t *)opt)->valid_ltime);
                if ((valid_ltime != 0) && (valid_ltime < next_rtr_sol)) {
                    next_rtr_sol = valid_ltime;
                }
#endif
                break;
#ifdef MODULE_GNRC_SIXLOWPAN_ND
            case NDP_OPT_6CTX:
                if (!gnrc_sixlowpan_nd_opt_6ctx_handle(rtr_adv->type,
                                                       (sixlowpan_nd_opt_6ctx_t *)opt)) {
                    /* invalid 6LoWPAN context option */
                    return;
                }
                uint16_t ltime = byteorder_ntohs(((sixlowpan_nd_opt_6ctx_t *)opt)->ltime);
                if ((ltime != 0) && (ltime < (next_rtr_sol / 60))) {
                    next_rtr_sol = ltime * 60;
                }

                break;
#endif
#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
            case NDP_OPT_ABR:
                gnrc_sixlowpan_nd_opt_abr_handle(iface, rtr_adv, icmpv6_size,
                                                 (sixlowpan_nd_opt_abr_t *)opt);
                break;
#endif
        }

        opt_offset += (opt->len * 8);
        sicmpv6_size -= (opt->len * 8);

#if ENABLE_DEBUG
        if (sicmpv6_size < 0) {
            DEBUG("ndp: Option parsing out of sync.\n");
        }
#endif
    }
#if ENABLE_DEBUG && defined(MODULE_GNRC_SIXLOWPAN_ND)
    if ((if_entry->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN) && (l2src_len <= 0)) {
        DEBUG("ndp: Router advertisement did not contain any source address information\n");
    }
#endif
    _stale_nc(iface, &ipv6->src, l2src, l2src_len);
    /* stop multicast router solicitation retransmission timer */
    xtimer_remove(&if_entry->rtr_sol_timer);
#ifdef MODULE_GNRC_SIXLOWPAN_ND
    if (if_entry->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN) {
        /* 3/4 of the time should be "well before" enough the respective timeout
         * not to run out; see https://tools.ietf.org/html/rfc6775#section-5.4.3 */
        next_rtr_sol *= 3;
        next_rtr_sol = (next_rtr_sol > 4) ? (next_rtr_sol >> 2) : 1;
        /* according to https://tools.ietf.org/html/rfc6775#section-5.3:
         * "In all cases, the RS retransmissions are terminated when an RA is
         *  received."
         *  Hence, reset router solicitation counter and reset timer. */
        if_entry->rtr_sol_count = 0;
        gnrc_sixlowpan_nd_rtr_sol_reschedule(nc_entry, next_rtr_sol);
        gnrc_ndp_internal_send_nbr_sol(nc_entry->iface, NULL, &nc_entry->ipv6_addr,
                                       &nc_entry->ipv6_addr);
        if (if_entry->flags & GNRC_IPV6_NETIF_FLAGS_ROUTER) {
            gnrc_ipv6_netif_set_rtr_adv(if_entry, true);
        }
    }
#endif
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
                kernel_pid_t ifs[GNRC_NETIF_NUMOF];
                size_t ifnum = gnrc_netif_get(ifs);

                for (size_t i = 0; i < ifnum; i++) {
                    gnrc_ndp_internal_send_nbr_sol(ifs[i], NULL, &nc_entry->ipv6_addr, &dst);
                }

                gnrc_ndp_internal_reset_nbr_sol_timer(nc_entry, GNRC_NDP_RETRANS_TIMER,
                                                      GNRC_NDP_MSG_NBR_SOL_RETRANS, gnrc_ipv6_pid);
            }
            else {
                gnrc_ipv6_netif_t *ipv6_iface = gnrc_ipv6_netif_get(nc_entry->iface);

                gnrc_ndp_internal_send_nbr_sol(nc_entry->iface, NULL, &nc_entry->ipv6_addr, &dst);

                mutex_lock(&ipv6_iface->mutex);
                gnrc_ndp_internal_reset_nbr_sol_timer(nc_entry, (uint32_t) timex_uint64(
                                                       ipv6_iface->retrans_timer
                                                      ),
                                                      GNRC_NDP_MSG_NBR_SOL_RETRANS, gnrc_ipv6_pid);
                mutex_unlock(&ipv6_iface->mutex);
            }
        }
        else if (nc_entry->probes_remaining <= 1) {

            /* For a 6LoWPAN router entries may be set to UNREACHABLE instead
             * of removing them, since RFC6775, section 6
             * (https://tools.ietf.org/html/rfc6775#section-6) says: "if NUD on
             * the router determines that the host is UNREACHABLE (based on the
             * logic in [RFC4861]), the NCE SHOULD NOT be deleted but rather
             * retained until the Registration Lifetime expires." However, this
             * "SHOULD NOT" is not implemented to circumvent NCEs going into
             * UNREACHABLE forever and in order to save some memory in the
             * neighbor cache. */

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

#if (defined(MODULE_GNRC_NDP_ROUTER) || defined(MODULE_GNRC_SIXLOWPAN_ND_ROUTER))
gnrc_pktsnip_t *gnrc_ndp_rtr_adv_build(uint8_t cur_hl, uint8_t flags,
                                       uint16_t ltime, uint32_t reach_time,
                                       uint32_t retrans_timer, gnrc_pktsnip_t *options)
{
    gnrc_pktsnip_t *pkt;
    DEBUG("ndp rtr: building router advertisement message\n");
    pkt = gnrc_icmpv6_build(options, ICMPV6_RTR_ADV, 0, sizeof(ndp_rtr_adv_t));
    if (pkt != NULL) {
        ndp_rtr_adv_t *rtr_adv = pkt->data;
        rtr_adv->cur_hl = cur_hl;
        rtr_adv->flags = (flags & NDP_RTR_ADV_FLAGS_MASK);
        rtr_adv->ltime = byteorder_htons(ltime);
        rtr_adv->reach_time = byteorder_htonl(reach_time);
        rtr_adv->retrans_timer = byteorder_htonl(retrans_timer);
    }
    return pkt;
}

gnrc_pktsnip_t *gnrc_ndp_opt_pi_build(uint8_t prefix_len, uint8_t flags,
                                      uint32_t valid_ltime, uint32_t pref_ltime,
                                      ipv6_addr_t *prefix, gnrc_pktsnip_t *next)
{
    gnrc_pktsnip_t *pkt = gnrc_ndp_opt_build(NDP_OPT_PI, sizeof(ndp_opt_pi_t),
                                             next);
    if (pkt != NULL) {
        ndp_opt_pi_t *pi_opt = pkt->data;
        pi_opt->prefix_len = prefix_len;
        pi_opt->flags = (flags & NDP_OPT_PI_FLAGS_MASK);
        pi_opt->valid_ltime = byteorder_htonl(valid_ltime);
        pi_opt->pref_ltime = byteorder_htonl(pref_ltime);
        pi_opt->resv.u32 = 0;
        /* Bits beyond prefix_len MUST be 0 */
        ipv6_addr_set_unspecified(&pi_opt->prefix);
        ipv6_addr_init_prefix(&pi_opt->prefix, prefix, prefix_len);
    }
    return pkt;
}

gnrc_pktsnip_t *gnrc_ndp_opt_mtu_build(uint32_t mtu, gnrc_pktsnip_t *next)
{
    gnrc_pktsnip_t *pkt = gnrc_ndp_opt_build(NDP_OPT_MTU, sizeof(ndp_opt_mtu_t),
                                             next);
    if (pkt != NULL) {
        ndp_opt_mtu_t *mtu_opt = pkt->data;
        mtu_opt->resv.u16 = 0;
        mtu_opt->mtu = byteorder_htonl(mtu);
    }
    return pkt;
}
#endif

/**
 * @}
 */
