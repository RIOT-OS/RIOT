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

#include "net/eui64.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/ndp.h"
#include "net/gnrc/ndp/internal.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/sixlowpan.h"
#include "net/gnrc/sixlowpan/ctx.h"
#include "random.h"
#include "timex.h"

#include "net/gnrc/sixlowpan/nd.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static inline void _rtr_sol_reschedule(gnrc_ipv6_netif_t *iface, uint32_t sec_delay)
{
    vtimer_remove(&iface->rtr_sol_timer);
    vtimer_set_msg(&iface->rtr_sol_timer, timex_set(sec_delay, 0), gnrc_ipv6_pid,
                   GNRC_SIXLOWPAN_ND_MSG_MC_RTR_SOL, iface);
}

static inline uint32_t _binary_exp_backoff(uint32_t base_sec, unsigned int exp)
{
    return genrand_uint32_range(0, (1 << exp)) * base_sec;
}

static inline void _revert_iid(uint8_t *iid)
{
    iid[0] ^= 0x02;
}

void gnrc_sixlowpan_nd_init(gnrc_ipv6_netif_t *iface)
{
    assert(iface->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN);
    mutex_lock(&iface->mutex);
    iface->rtr_sol_count = 0;   /* first will be sent immediately */

    DEBUG("6lo nd: retransmit multicast rtr sol in 10 sec\n");
    _rtr_sol_reschedule(iface, GNRC_SIXLOWPAN_ND_RTR_SOL_INT);
    mutex_unlock(&iface->mutex);
    gnrc_ndp_internal_send_rtr_sol(iface->pid, NULL);
}

void gnrc_sixlowpan_nd_mc_rtr_sol(gnrc_ipv6_netif_t *iface)
{
    uint32_t interval;
    assert(iface->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN);
    mutex_lock(&iface->mutex);
    if (iface->rtr_sol_count < GNRC_NDP_MAX_RTR_SOL_NUMOF) {
        DEBUG("6lo nd: retransmit multicast rtr sol in 10 sec\n");
        iface->rtr_sol_count++;
        interval = GNRC_SIXLOWPAN_ND_RTR_SOL_INT;
    }
    else {
        unsigned int exp = (unsigned int)(iface->rtr_sol_count - GNRC_NDP_MAX_RTR_SOL_NUMOF);
        interval = _binary_exp_backoff(1, exp);
        if ((1U << exp) < GNRC_SIXLOWPAN_ND_MAX_RTR_SOL_INT) {
            /* XXX Not sure if this is the correct interpretation of the truncation described in
             * https://tools.ietf.org/html/rfc6775#section-5.3. In every source I've read the
             * truncating value was the exponent, not the target value, so I'm very confused
             * about this sentencing. Anyway, since 60 sec is a maximum value this should only
             * affect the energy consumption of the implementation by sending the next RS too fast
             * but not its interoperability. */
            iface->rtr_sol_count++;
        }
        /* RFC6775, section 5.3 (https://tools.ietf.org/html/rfc6775#section-5.3)
         * states that router solicitation should be sent slower after the
         * initial 3 retransmissions (i.e. >= 10 secondes) and truncate "the
         * increase of the retransmission timer at 60 seconds". */
        if (interval < GNRC_SIXLOWPAN_ND_RTR_SOL_INT) {
            interval = GNRC_SIXLOWPAN_ND_RTR_SOL_INT;
        }
        else if (interval > GNRC_SIXLOWPAN_ND_MAX_RTR_SOL_INT) {
            interval = GNRC_SIXLOWPAN_ND_MAX_RTR_SOL_INT;
        }

        DEBUG("6lo nd: retransmit multicast rtr sol in %" PRIu32 " sec\n", interval);
    }
    _rtr_sol_reschedule(iface, interval);
    mutex_unlock(&iface->mutex);
    gnrc_ndp_internal_send_rtr_sol(iface->pid, NULL);
}

void gnrc_sixlowpan_nd_uc_rtr_sol(gnrc_ipv6_nc_t *nce)
{
    assert(gnrc_ipv6_netif_get(nce->iface)->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN);
    /* neighbor is not a router anymore */
    if (!(nce->flags & GNRC_IPV6_NC_IS_ROUTER) || ipv6_addr_is_unspecified(&nce->ipv6_addr)) {
        /* and there are no routers anymore */
        if (gnrc_ipv6_nc_get_next_router(NULL) == NULL) {
            /* start search for routers */
            gnrc_sixlowpan_nd_init(gnrc_ipv6_netif_get(nce->iface));
        }
        /* otherwise ignore this call */
        return;
    }
    /* next RS is rescheduled by RA handle function */
    gnrc_ndp_internal_send_rtr_sol(nce->iface, &nce->ipv6_addr);
}

kernel_pid_t gnrc_sixlowpan_nd_next_hop_l2addr(uint8_t *l2addr, uint8_t *l2addr_len,
                                               kernel_pid_t iface, ipv6_addr_t *dst)
{
    ipv6_addr_t *next_hop = NULL;
    gnrc_ipv6_nc_t *nc_entry = NULL;

#ifdef MODULE_GNRC_IPV6_EXT_RH
    ipv6_hdr_t *hdr;
    gnrc_pktsnip_t *ipv6;
    LL_SEARCH_SCALAR(pkt, ipv6, type, GNRC_NETTYPE_IPV6);
    assert(ipv6);
    hdr = ipv6->data;
    next_hop = ipv6_ext_rh_next_hop(hdr);
#endif
#ifdef MODULE_FIB
    ipv6_addr_t next_hop_actual;    /* FIB copies address into this variable */
    /* don't look-up link local addresses in FIB */
    if ((next_hop == NULL) && !ipv6_addr_is_link_local(dst)) {
        size_t next_hop_size = sizeof(ipv6_addr_t);
        uint32_t next_hop_flags = 0;
        if ((next_hop == NULL) &&
            (fib_get_next_hop(&gnrc_ipv6_fib_table, &iface, next_hop_actual.u8, &next_hop_size,
                              &next_hop_flags, (uint8_t *)dst,
                              sizeof(ipv6_addr_t), 0) >= 0) &&
            (next_hop_size == sizeof(ipv6_addr_t))) {
            next_hop = &next_hop_actual;
        }
    }
#endif
#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
    /* next hop determination: https://tools.ietf.org/html/rfc6775#section-6.5.4 */
    nc_entry = gnrc_ipv6_nc_get(iface, dst);
    if ((nc_entry != NULL) && (gnrc_ipv6_nc_get_type(nc_entry) == GNRC_IPV6_NC_TYPE_REGISTERED)) {
        next_hop = dst;
    }
#endif
    /* next hop determination according to: https://tools.ietf.org/html/rfc6775#section-5.6 */
    if ((next_hop == NULL) && ipv6_addr_is_link_local(dst)) {   /* prefix is "on-link" */
        /* multicast is not handled here anyway so we don't need to check that */
        next_hop = dst;
    }
    else if (next_hop == NULL) {                                /* prefix is off-link */
        next_hop = gnrc_ndp_internal_default_router();
    }

    /* address resolution of next_hop: https://tools.ietf.org/html/rfc6775#section-5.7 */
    if ((nc_entry == NULL) || (next_hop != dst)) {
        /* get if not gotten from previous check */
        nc_entry = gnrc_ipv6_nc_get(iface, next_hop);
    }
    if (ipv6_addr_is_link_local(next_hop)) {
        kernel_pid_t ifs[GNRC_NETIF_NUMOF];
        size_t ifnum = gnrc_netif_get(ifs);
        /* we don't need address resolution, the EUI-64 is in next_hop's IID */
        *l2addr_len = sizeof(eui64_t);
        memcpy(l2addr, &next_hop->u8[8], sizeof(eui64_t));
        _revert_iid(l2addr);
        if (iface == KERNEL_PID_UNDEF) {
            for (unsigned i = 0; i < ifnum; i++) {
                gnrc_ipv6_netif_t *ipv6_if = gnrc_ipv6_netif_get(ifs[i]);
                if ((ipv6_if != NULL) && (ipv6_if->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN)) {
                    /* always take the first 6LoWPAN interface we can find */
                    return ifs[i];
                }
            }
        }
        return iface;
    }
    else if ((nc_entry == NULL) || (!gnrc_ipv6_nc_is_reachable(nc_entry)) ||
        (gnrc_ipv6_nc_get_type(nc_entry) == GNRC_IPV6_NC_TYPE_TENTATIVE)) {
        return KERNEL_PID_UNDEF;
    }
    else {
        if (nc_entry->l2_addr_len > 0) {
            memcpy(l2addr, nc_entry->l2_addr, nc_entry->l2_addr_len);
        }
        *l2addr_len = nc_entry->l2_addr_len;
    }
    return nc_entry->iface;
}

void gnrc_sixlowpan_nd_rtr_sol_reschedule(gnrc_ipv6_nc_t *nce, uint32_t sec_delay)
{
    assert(nce != NULL);
    assert(sec_delay != 0U);
    vtimer_remove(&nce->rtr_sol_timer);
    vtimer_set_msg(&nce->rtr_sol_timer, timex_set(sec_delay, 0), gnrc_ipv6_pid,
                   GNRC_SIXLOWPAN_ND_MSG_MC_RTR_SOL, nce);
}

gnrc_pktsnip_t *gnrc_sixlowpan_nd_opt_ar_build(uint8_t status, uint16_t ltime, eui64_t *eui64,
                                               gnrc_pktsnip_t *next)
{
    gnrc_pktsnip_t *pkt = gnrc_ndp_opt_build(NDP_OPT_AR, sizeof(sixlowpan_nd_opt_ar_t), next);

    if (pkt != NULL) {
        sixlowpan_nd_opt_ar_t *ar_opt = pkt->data;
        ar_opt->status = status;
        ar_opt->resv[0] = ar_opt->resv[1] = ar_opt->resv[2] = 0;
        ar_opt->ltime = byteorder_htons(ltime);
        memcpy(&ar_opt->eui64, eui64, sizeof(eui64_t));
    }

    return pkt;
}

uint8_t gnrc_sixlowpan_nd_opt_ar_handle(kernel_pid_t iface, ipv6_hdr_t *ipv6, uint8_t icmpv6_type,
                                        sixlowpan_nd_opt_ar_t *ar_opt, uint8_t *sl2a,
                                        size_t sl2a_len)
{
    eui64_t eui64;
    gnrc_ipv6_netif_t *ipv6_iface;
    gnrc_ipv6_nc_t *nc_entry;
    uint8_t status = 0;
    (void)sl2a;
    (void)sl2a_len;
    if (ar_opt->len != SIXLOWPAN_ND_OPT_AR_LEN) {
        /* discard silently: see https://tools.ietf.org/html/rfc6775#section-5.5.2 */
        return 0;
    }
    if (gnrc_netapi_get(iface, NETOPT_ADDRESS_LONG, 0, &eui64,
                        sizeof(eui64)) < 0) {
        /* discard silently: see https://tools.ietf.org/html/rfc6775#section-5.5.2 */
        return 0;
    }
    ipv6_iface = gnrc_ipv6_netif_get(iface);
    nc_entry = gnrc_ipv6_nc_get(iface, &ipv6->src);
    switch (icmpv6_type) {
        case ICMPV6_NBR_ADV:
            if (!(ipv6_iface->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN)) {
                DEBUG("6lo nd: interface not a 6LoWPAN interface\n");
                return 0;
            }
            if (eui64.uint64.u64 != ar_opt->eui64.uint64.u64) {
                /* discard silently: see https://tools.ietf.org/html/rfc6775#section-5.5.2 */
                return 0;
            }
            switch (ar_opt->status) {
                case SIXLOWPAN_ND_STATUS_SUCCESS:
                    DEBUG("6lo nd: address registration successful\n");
                    mutex_lock(&ipv6_iface->mutex);
                    /* reschedule 1 minute before lifetime expires */
                    timex_t t = { (uint32_t)(byteorder_ntohs(ar_opt->ltime) - 1) * 60, 0 };
                    vtimer_remove(&nc_entry->nbr_sol_timer);
                    vtimer_set_msg(&nc_entry->nbr_sol_timer, t, gnrc_ipv6_pid,
                                   GNRC_NDP_MSG_NBR_SOL_RETRANS, nc_entry);
                    mutex_unlock(&ipv6_iface->mutex);
                    break;
                case SIXLOWPAN_ND_STATUS_DUP:
                    DEBUG("6lo nd: address registration determined duplicated\n");
                    /* TODO: handle DAD failed case */
                    gnrc_ipv6_netif_remove_addr(iface, &ipv6->dst);
                    /* address should not be used anymore */
                    break;
                case SIXLOWPAN_ND_STATUS_NC_FULL:
                    DEBUG("6lo nd: neighbor cache on router is full\n");
                    gnrc_ipv6_nc_remove(iface, &ipv6->src);
                    /* try to find another router */
                    gnrc_sixlowpan_nd_init(ipv6_iface);
                    break;
                default:
                    DEBUG("6lo nd: unknown status for registration received\n");
                    break;
            }
#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
        case ICMPV6_NBR_SOL:
            if (!(ipv6_iface->flags & GNRC_IPV6_NETIF_FLAGS_SIXLOWPAN) &&
                !(ipv6_iface->flags & GNRC_IPV6_NETIF_FLAGS_ROUTER)) {
                DEBUG("6lo nd: interface not a 6LoWPAN or forwarding interface\n");
                return 0;
            }
            if ((ar_opt->status != 0) ||
                ipv6_addr_is_unspecified(&ipv6->src)) {
                /* discard silently */
                return 0;
            }
            /* TODO multihop DAD */
            if ((nc_entry != NULL) &&
                ((gnrc_ipv6_nc_get_type(nc_entry) == GNRC_IPV6_NC_TYPE_REGISTERED) ||
                 (gnrc_ipv6_nc_get_type(nc_entry) == GNRC_IPV6_NC_TYPE_TENTATIVE)) &&
                (ar_opt->eui64.uint64.u64 != nc_entry->eui64.uint64.u64)) {
                /* there is already another node with this address */
                DEBUG("6lo nd: duplicate address detected\n");
                status = SIXLOWPAN_ND_STATUS_DUP;
            }
            else if ((nc_entry != NULL) && (ar_opt->ltime.u16 == 0)) {
                gnrc_ipv6_nc_remove(iface, &ipv6->src);
                /* TODO, notify routing protocol */
            }
            else if (ar_opt->ltime.u16 != 0) {
                /* TODO: multihop DAD behavior */
                uint16_t reg_ltime;
                if (nc_entry == NULL) {
                    if ((nc_entry = gnrc_ipv6_nc_add(iface, &ipv6->src, sl2a, sl2a_len,
                                                     GNRC_IPV6_NC_STATE_STALE)) == NULL) {
                        DEBUG("6lo nd: neighbor cache is full\n");
                        return SIXLOWPAN_ND_STATUS_NC_FULL;
                    }
                    nc_entry->eui64 = ar_opt->eui64;
                }
                nc_entry->flags &= ~GNRC_IPV6_NC_TYPE_MASK;
                nc_entry->flags |= GNRC_IPV6_NC_TYPE_REGISTERED;
                reg_ltime = byteorder_ntohs(ar_opt->ltime);
                /* TODO: notify routing protocol */
                vtimer_remove(&nc_entry->type_timeout);
                vtimer_set_msg(&nc_entry->type_timeout, timex_set(reg_ltime * 60, 0),
                               gnrc_ipv6_pid, GNRC_SIXLOWPAN_ND_MSG_AR_TIMEOUT, nc_entry);
            }
#endif
        default:
            break;
    }

    return status;
}

bool gnrc_sixlowpan_nd_opt_6ctx_handle(uint8_t icmpv6_type, sixlowpan_nd_opt_6ctx_t *ctx_opt)
{
    if (((ctx_opt->ctx_len < 64) && (ctx_opt->len != 2)) ||
        ((ctx_opt->ctx_len >= 64) && (ctx_opt->len != 3))) {
        DEBUG("6lo nd: invalid 6LoWPAN context option received\n");
        return false;
    }
    if (icmpv6_type != ICMPV6_RTR_ADV) {
        /* discard silently */
        return true;
    }
    /* don't care for result */
    gnrc_sixlowpan_ctx_update(sixlowpan_nd_opt_6ctx_get_cid(ctx_opt), (ipv6_addr_t *)(ctx_opt + 1),
                              ctx_opt->ctx_len, byteorder_ntohs(ctx_opt->ltime),
                              sixlowpan_nd_opt_6ctx_is_comp(ctx_opt));
    return true;
}

void gnrc_sixlowpan_nd_wakeup(void)
{
    gnrc_ipv6_nc_t *router = gnrc_ipv6_nc_get_next_router(NULL);
    while (router) {
        timex_t t = { 0, GNRC_NDP_RETRANS_TIMER };
        vtimer_remove(&router->rtr_sol_timer);
        gnrc_sixlowpan_nd_uc_rtr_sol(router);
        gnrc_ndp_internal_send_nbr_sol(router->iface, &router->ipv6_addr, &router->ipv6_addr);
        vtimer_remove(&router->nbr_sol_timer);
        vtimer_set_msg(&router->nbr_sol_timer, t, gnrc_ipv6_pid, GNRC_NDP_MSG_NBR_SOL_RETRANS,
                       router);
    }
}

/* gnrc_sixlowpan_nd_opt_abr_handle etc. implemented in gnrc_sixlowpan_nd_router */

/** @} */
