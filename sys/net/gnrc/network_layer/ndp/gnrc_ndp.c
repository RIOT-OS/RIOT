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

/* random helper function */
void gnrc_ndp_nbr_sol_handle(kernel_pid_t iface, gnrc_pktsnip_t *pkt,
                             ipv6_hdr_t *ipv6, ndp_nbr_sol_t *nbr_sol,
                             size_t icmpv6_size)
{
    uint16_t opt_offset = 0;
    uint8_t *buf = ((uint8_t *)nbr_sol) + sizeof(ndp_nbr_sol_t);
    ipv6_addr_t *tgt;
    int sicmpv6_size = (int)icmpv6_size;

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
                if (!gnrc_ndp_internal_sl2a_opt_handle(iface, pkt, ipv6, nbr_sol->type, opt)) {
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

    gnrc_ndp_internal_send_nbr_adv(iface, tgt, &ipv6->src, ipv6_addr_is_multicast(&ipv6->dst),
                                   NULL);

    return;
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

    if (gnrc_ipv6_nc_get_state(nc_entry) == GNRC_IPV6_NC_STATE_INCOMPLETE) {
        gnrc_pktqueue_t *queued_pkt;

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
            /* TODO: update FIB */
        }

        while ((queued_pkt = gnrc_pktqueue_remove_head(&nc_entry->pkts)) != NULL) {
            gnrc_netapi_send(gnrc_ipv6_pid, queued_pkt->pkt);
            queued_pkt->pkt = NULL;
        }
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
            else if (l2tgt_changed && (l2tgt_len != 0)) {
                gnrc_ndp_internal_set_state(nc_entry, GNRC_IPV6_NC_STATE_STALE);
            }

            if (nbr_adv->flags & NDP_NBR_ADV_FLAGS_R) {
                nc_entry->flags |= GNRC_IPV6_NC_IS_ROUTER;
            }
            else {
                nc_entry->flags &= ~GNRC_IPV6_NC_IS_ROUTER;
                /* TODO: update FIB */
            }
        }
        else if (l2tgt_changed) {
            if (gnrc_ipv6_nc_get_state(nc_entry) == GNRC_IPV6_NC_STATE_REACHABLE) {
                gnrc_ndp_internal_set_state(nc_entry, GNRC_IPV6_NC_STATE_STALE);

            }
        }
    }

    return;
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
    uint32_t reach_time = genrand_uint32_range(GNRC_NDP_MIN_RAND, GNRC_NDP_MAX_RAND);

    /* set default values */
    mutex_lock(&iface->mutex);
    iface->reach_time_base = GNRC_NDP_REACH_TIME;
    reach_time = (reach_time * iface->reach_time_base) / 10;
    iface->reach_time = timex_set(0, reach_time);
    timex_normalize(&iface->reach_time);
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
