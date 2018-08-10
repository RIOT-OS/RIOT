/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <errno.h>
#include <stdbool.h>

#include "log.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/netif/internal.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ndp.h"
#include "net/gnrc/pktqueue.h"
#include "net/gnrc/sixlowpan/nd.h"
#include "net/ndp.h"
#include "net/sixlowpan/nd.h"
#if GNRC_IPV6_NIB_CONF_DNS
#include "net/sock/dns.h"
#endif

#include "_nib-internal.h"
#include "_nib-arsm.h"
#include "_nib-router.h"
#include "_nib-6ln.h"
#include "_nib-6lr.h"
#include "_nib-slaac.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"
#if ENABLE_DEBUG
#include "xtimer.h"
#endif

static char addr_str[IPV6_ADDR_MAX_STR_LEN];

#if GNRC_IPV6_NIB_CONF_QUEUE_PKT
static gnrc_pktqueue_t _queue_pool[GNRC_IPV6_NIB_NUMOF];
#endif  /* GNRC_IPV6_NIB_CONF_QUEUE_PKT */

#if GNRC_IPV6_NIB_CONF_DNS
static evtimer_msg_event_t _rdnss_timeout;
#endif

/**
 * @internal
 * @{
 */
#if GNRC_IPV6_NIB_CONF_ROUTER
static void _handle_rtr_sol(gnrc_netif_t *netif, const ipv6_hdr_t *ipv6,
                            const ndp_rtr_sol_t *rtr_sol, size_t icmpv6_len);
#endif  /* GNRC_IPV6_NIB_CONF_ROUTER */
static void _handle_rtr_adv(gnrc_netif_t *netif, const ipv6_hdr_t *ipv6,
                            const ndp_rtr_adv_t *rtr_adv, size_t icmpv6_len);
static void _handle_nbr_sol(gnrc_netif_t *netif, const ipv6_hdr_t *ipv6,
                            const ndp_nbr_sol_t *nbr_sol, size_t icmpv6_len);
static void _handle_nbr_adv(gnrc_netif_t *netif, const ipv6_hdr_t *ipv6,
                            const ndp_nbr_adv_t *nbr_adv, size_t icmpv6_len);

static bool _resolve_addr(const ipv6_addr_t *dst, gnrc_netif_t *netif,
                          gnrc_pktsnip_t *pkt, gnrc_ipv6_nib_nc_t *nce,
                          _nib_onl_entry_t *entry);

static void _handle_pfx_timeout(_nib_offl_entry_t *pfx);
static void _handle_rtr_timeout(_nib_dr_entry_t *router);
static void _handle_snd_na(gnrc_pktsnip_t *pkt);
/* needs to be exported for 6LN's ARO handling */
void _handle_search_rtr(gnrc_netif_t *netif);
#if GNRC_IPV6_NIB_CONF_DNS
static void _handle_rdnss_timeout(sock_udp_ep_t *dns_server);
#endif
/** @} */

void gnrc_ipv6_nib_init(void)
{
    evtimer_event_t *tmp;

    mutex_lock(&_nib_mutex);
    for (evtimer_event_t *ptr = _nib_evtimer.events;
         (ptr != NULL) && (tmp = (ptr->next), 1);
         ptr = tmp) {
        evtimer_del((evtimer_t *)(&_nib_evtimer), ptr);
    }
    _nib_init();
    mutex_unlock(&_nib_mutex);
}

void gnrc_ipv6_nib_init_iface(gnrc_netif_t *netif)
{
    assert(netif != NULL);
    DEBUG("nib: Initialize interface %u\n", netif->pid);
    gnrc_netif_acquire(netif);

    _init_iface_arsm(netif);
    netif->ipv6.retrans_time = NDP_RETRANS_TIMER_MS;
#if GNRC_IPV6_NIB_CONF_SLAAC || GNRC_IPV6_NIB_CONF_6LN
    /* TODO: set differently dependent on GNRC_IPV6_NIB_CONF_SLAAC if
     * alternatives exist */
    netif->ipv6.aac_mode = GNRC_NETIF_AAC_AUTO;
#endif  /* GNRC_IPV6_NIB_CONF_SLAAC || GNRC_IPV6_NIB_CONF_6LN */
    _init_iface_router(netif);
#if GNRC_IPV6_NIB_CONF_6LN
    netif->ipv6.rs_sent = 0;
    if (netif->device_type == NETDEV_TYPE_IEEE802154) {
        /* see https://tools.ietf.org/html/rfc6775#section-5.2 */
        uint16_t src_len = IEEE802154_LONG_ADDRESS_LEN;
        gnrc_netapi_opt_t opt = { .opt = NETOPT_SRC_LEN,
                                  .data = &src_len,
                                  .data_len = sizeof(src_len) };

        /* XXX we are supposed to be in interface context here, so use driver
         * directly everything else would deadlock anyway */
        netif->ops->set(netif, &opt);
    }
#endif  /* GNRC_IPV6_NIB_CONF_6LN */
    netif->ipv6.na_sent = 0;
    if (gnrc_netif_ipv6_group_join_internal(netif,
                                            &ipv6_addr_all_nodes_link_local) < 0) {
        DEBUG("nib: Can't join link-local all-nodes on interface %u\n",
              netif->pid);
        gnrc_netif_release(netif);
        return;
    }
    _auto_configure_addr(netif, &ipv6_addr_link_local_prefix, 64U);
    if (!(gnrc_netif_is_rtr_adv(netif)) ||
        (gnrc_netif_is_6ln(netif) && !gnrc_netif_is_6lbr(netif))) {
        uint32_t next_rs_time = random_uint32_range(0, NDP_MAX_RS_MS_DELAY);

        _evtimer_add(netif, GNRC_IPV6_NIB_SEARCH_RTR, &netif->ipv6.search_rtr,
                     next_rs_time);
    }
#if GNRC_IPV6_NIB_CONF_ROUTER
    else {
        _handle_snd_mc_ra(netif);
    }
#endif  /* GNRC_IPV6_NIB_CONF_ROUTER */
    gnrc_netif_release(netif);
}

static bool _on_link(const ipv6_addr_t *dst, unsigned *iface)
{
    _nib_offl_entry_t *entry = NULL;

#if GNRC_IPV6_NIB_CONF_6LN
    if (*iface != 0) {
        if (gnrc_netif_is_6ln(gnrc_netif_get_by_pid(*iface))) {
            return ipv6_addr_is_link_local(dst);
        }
    }
#endif  /* GNRC_IPV6_NIB_CONF_6LN */
    while ((entry = _nib_offl_iter(entry))) {
        if ((entry->mode & _PL) && (entry->flags & _PFX_ON_LINK) &&
            (ipv6_addr_match_prefix(dst, &entry->pfx) >= entry->pfx_len)) {
            *iface = _nib_onl_get_if(entry->next_hop);
            return true;
        }
    }
    return ipv6_addr_is_link_local(dst);
}

int gnrc_ipv6_nib_get_next_hop_l2addr(const ipv6_addr_t *dst,
                                      gnrc_netif_t *netif, gnrc_pktsnip_t *pkt,
                                      gnrc_ipv6_nib_nc_t *nce)
{
    int res = 0;

    DEBUG("nib: get next hop link-layer address of %s%%%u\n",
          ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)),
          (netif != NULL) ? (unsigned)netif->pid : 0U);
    gnrc_netif_acquire(netif);
    mutex_lock(&_nib_mutex);
    do {    /* XXX: hidden goto ;-) */
        _nib_onl_entry_t *node = _nib_onl_get(dst,
                                              (netif == NULL) ? 0 : netif->pid);
        /* consider neighbor cache entries first */
        unsigned iface = (node == NULL) ? 0 : _nib_onl_get_if(node);

        if ((node != NULL) || _on_link(dst, &iface)) {
            DEBUG("nib: %s is on-link or in NC, start address resolution\n",
                  ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)));
            /* on-link prefixes return their interface */
            if (!ipv6_addr_is_link_local(dst) && (iface != 0)) {
                /* release preassumed interface */
                gnrc_netif_release(netif);
                netif = gnrc_netif_get_by_pid(iface);
                gnrc_netif_acquire(netif);
            }
            if ((netif == NULL) ||
                !_resolve_addr(dst, netif, pkt, nce, node)) {
                DEBUG("nib: host unreachable\n");
                /* _resolve_addr releases pkt only if not queued (in which case
                 * we also shouldn't release), but if netif is not defined we
                 * should release in any case. */
                if (netif == NULL) {
                    gnrc_pktbuf_release_error(pkt, EHOSTUNREACH);
                }
                res = -EHOSTUNREACH;
                break;
            }
        }
        else {
            gnrc_ipv6_nib_ft_t route;

            DEBUG("nib: %s is off-link, resolve route\n",
                  ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)));
            res = _nib_get_route(dst, pkt, &route);
            if ((res < 0) || ipv6_addr_is_unspecified(&route.next_hop)) {
                DEBUG("nib: no route to %s found or is prefix list entry, "
                      "search neighbor cache\n",
                      ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)));

                if (res == 0) {
                    DEBUG("nib: prefix list entry => taking dst as next hop\n");
                    memcpy(&route.next_hop, dst, sizeof(route.next_hop));
                }
                else {
                    res = -ENETUNREACH;
                    gnrc_pktbuf_release_error(pkt, ENETUNREACH);
                    break;
                }
            }
            if ((netif != NULL) && (netif->pid != (int)route.iface)) {
                /* drop pre-assumed netif */
                gnrc_netif_release(netif);
            }
            if ((netif == NULL) || (netif->pid != (int)route.iface)) {
                /* get actual netif */
                netif = gnrc_netif_get_by_pid(route.iface);
                gnrc_netif_acquire(netif);
            }
            node = _nib_onl_get(&route.next_hop,
                                (netif == NULL) ? netif->pid : 0);
            if (_resolve_addr(&route.next_hop, netif, pkt, nce, node)) {
                _call_route_info_cb(netif,
                                    GNRC_IPV6_NIB_ROUTE_INFO_TYPE_RN,
                                    &route.dst,
                                    (void *)((intptr_t)route.dst_len));
#if GNRC_IPV6_NIB_CONF_DC
                _nib_dc_add(&route.next_hop, netif->pid, dst);
#endif  /* GNRC_IPV6_NIB_CONF_DC */
            }
            else {
                /* _resolve_addr releases pkt if not queued (in which case
                 * we also shouldn't release */
                res = -EHOSTUNREACH;
            }
        }
    } while (0);
    mutex_unlock(&_nib_mutex);
    gnrc_netif_release(netif);
    return res;
}

void gnrc_ipv6_nib_handle_pkt(gnrc_netif_t *netif, const ipv6_hdr_t *ipv6,
                              const icmpv6_hdr_t *icmpv6, size_t icmpv6_len)
{
    DEBUG("nib: Handle packet (icmpv6->type = %u)\n", icmpv6->type);
    assert(netif != NULL);
    gnrc_netif_acquire(netif);
    mutex_lock(&_nib_mutex);
    switch (icmpv6->type) {
#if GNRC_IPV6_NIB_CONF_ROUTER
        case ICMPV6_RTR_SOL:
            _handle_rtr_sol(netif, ipv6, (ndp_rtr_sol_t *)icmpv6, icmpv6_len);
            break;
#endif  /* GNRC_IPV6_NIB_CONF_ROUTER */
        case ICMPV6_RTR_ADV:
            _handle_rtr_adv(netif, ipv6, (ndp_rtr_adv_t *)icmpv6, icmpv6_len);
            break;
        case ICMPV6_NBR_SOL:
            _handle_nbr_sol(netif, ipv6, (ndp_nbr_sol_t *)icmpv6, icmpv6_len);
            break;
        case ICMPV6_NBR_ADV:
            _handle_nbr_adv(netif, ipv6, (ndp_nbr_adv_t *)icmpv6, icmpv6_len);
            break;
#if GNRC_IPV6_NIB_CONF_REDIRECT
        case ICMPV6_REDIRECT:
            /* TODO */
            break;
#endif  /* GNRC_IPV6_NIB_CONF_REDIRECT */
#if GNRC_IPV6_NIB_CONF_MULTIHOP_DAD
        case ICMPV6_DAR:
            /* TODO */
            break;
        case ICMPV6_DAC:
            /* TODO */
            break;
#endif  /* GNRC_IPV6_NIB_CONF_MULTIHOP_DAD */
    }
    mutex_unlock(&_nib_mutex);
    gnrc_netif_release(netif);
}

void gnrc_ipv6_nib_handle_timer_event(void *ctx, uint16_t type)
{
    DEBUG("nib: Handle timer event (ctx = %p, type = 0x%04x, now = %ums)\n",
          ctx, type, (unsigned)xtimer_now_usec() / 1000);
    mutex_lock(&_nib_mutex);
    switch (type) {
#if GNRC_IPV6_NIB_CONF_ARSM
        case GNRC_IPV6_NIB_SND_UC_NS:
        case GNRC_IPV6_NIB_SND_MC_NS:
            _handle_snd_ns(ctx);
            break;
        case GNRC_IPV6_NIB_REACH_TIMEOUT:
        case GNRC_IPV6_NIB_DELAY_TIMEOUT:
            _handle_state_timeout(ctx);
            break;
        case GNRC_IPV6_NIB_RECALC_REACH_TIME:
            _recalc_reach_time(ctx);
            break;
#endif  /* GNRC_IPV6_NIB_CONF_ARSM */
        case GNRC_IPV6_NIB_SND_NA:
            _handle_snd_na(ctx);
            break;
        case GNRC_IPV6_NIB_SEARCH_RTR:
            _handle_search_rtr(ctx);
            break;
#if GNRC_IPV6_NIB_CONF_ROUTER
        case GNRC_IPV6_NIB_REPLY_RS:
            _handle_reply_rs(ctx);
            break;
        case GNRC_IPV6_NIB_SND_MC_RA:
            _handle_snd_mc_ra(ctx);
            break;
        case GNRC_IPV6_NIB_ROUTE_TIMEOUT:
            _nib_ft_remove(ctx);
            break;
#endif  /* GNRC_IPV6_NIB_CONF_ROUTER */
#if GNRC_IPV6_NIB_CONF_6LR
        case GNRC_IPV6_NIB_ADDR_REG_TIMEOUT:
            _nib_nc_remove(ctx);
            break;
#endif  /* GNRC_IPV6_NIB_CONF_6LR */
#if GNRC_IPV6_NIB_CONF_MULTIHOP_P6C
        case GNRC_IPV6_NIB_ABR_TIMEOUT:
            _nib_abr_remove(&((_nib_abr_entry_t *)ctx)->addr);
            break;
#endif  /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */
        case GNRC_IPV6_NIB_PFX_TIMEOUT:
            _handle_pfx_timeout(ctx);
            break;
        case GNRC_IPV6_NIB_RTR_TIMEOUT:
            _handle_rtr_timeout(ctx);
            break;
#if GNRC_IPV6_NIB_CONF_6LN
        case GNRC_IPV6_NIB_REREG_ADDRESS:
            _handle_rereg_address(ctx);
            break;
#endif  /* GNRC_IPV6_NIB_CONF_6LN */
        case GNRC_IPV6_NIB_DAD:
            _handle_dad(ctx);
            break;
        case GNRC_IPV6_NIB_VALID_ADDR:
            _handle_valid_addr(ctx);
            break;
#if GNRC_IPV6_NIB_CONF_DNS
        case GNRC_IPV6_NIB_RDNSS_TIMEOUT:
            _handle_rdnss_timeout(ctx);
#endif
        default:
            break;
    }
    mutex_unlock(&_nib_mutex);
}

#if GNRC_IPV6_NIB_CONF_ROUTER
void gnrc_ipv6_nib_change_rtr_adv_iface(gnrc_netif_t *netif, bool enable)
{
    gnrc_netif_acquire(netif);
    if (enable) {
        _set_rtr_adv(netif);
    }
    else {
        uint32_t next_rs_time = random_uint32_range(0, NDP_MAX_RS_MS_DELAY);

        netif->ipv6.ra_sent = (UINT8_MAX - NDP_MAX_FIN_RA_NUMOF) + 1;
        netif->flags &= ~GNRC_NETIF_FLAGS_IPV6_RTR_ADV;
        /* send final router advertisements */
        _handle_snd_mc_ra(netif);
        _evtimer_add(netif, GNRC_IPV6_NIB_SEARCH_RTR, &netif->ipv6.search_rtr,
                     next_rs_time);
    }
    gnrc_netif_release(netif);
}
#endif  /* GNRC_IPV6_NIB_CONF_ROUTER */

/*
 * @internal
 * @{
 */
static void _handle_mtuo(gnrc_netif_t *netif, const icmpv6_hdr_t *icmpv6,
                         const ndp_opt_mtu_t *mtuo);
#if GNRC_IPV6_NIB_CONF_DNS
static uint32_t _handle_rdnsso(gnrc_netif_t *netif, const icmpv6_hdr_t *icmpv6,
                               const ndp_opt_rdnss_t *rdnsso);
#endif
#if GNRC_IPV6_NIB_CONF_MULTIHOP_P6C
static uint32_t _handle_pio(gnrc_netif_t *netif, const icmpv6_hdr_t *icmpv6,
                            const ndp_opt_pi_t *pio,
                            _nib_abr_entry_t *abr);
#else   /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */
static uint32_t _handle_pio(gnrc_netif_t *netif, const icmpv6_hdr_t *icmpv6,
                            const ndp_opt_pi_t *pio);
#endif  /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */
/** @} */

/* Iterator for NDP options in a packet */
#define FOREACH_OPT(ndp_pkt, opt, icmpv6_len) \
    for (opt = (ndp_opt_t *)(ndp_pkt + 1); \
         icmpv6_len > 0; \
         icmpv6_len -= (opt->len << 3), \
         opt = (ndp_opt_t *)(((uint8_t *)opt) + (opt->len << 3)))

#if GNRC_IPV6_NIB_CONF_ROUTER
static void _handle_rtr_sol(gnrc_netif_t *netif, const ipv6_hdr_t *ipv6,
                            const ndp_rtr_sol_t *rtr_sol, size_t icmpv6_len)
{
    size_t tmp_len = icmpv6_len - sizeof(ndp_rtr_sol_t);
    _nib_onl_entry_t *nce = NULL;
    ndp_opt_t *opt;
    uint32_t next_ra_delay = random_uint32_range(0, NDP_MAX_RA_DELAY);

    assert(netif != NULL);
    /* check validity, see: https://tools.ietf.org/html/rfc4861#section-6.1.1 */
    /* checksum is checked by GNRC's ICMPv6 module */
    if (!(gnrc_netif_is_rtr(netif)) || (ipv6->hl != 255U) ||
        (rtr_sol->code != 0U) || (icmpv6_len < sizeof(ndp_rtr_sol_t))) {
        DEBUG("nib: Received router solicitation is invalid (or interface %i "
              "is not a forwarding interface). Discarding silently\n",
              netif->pid);
        DEBUG("     - IP Hop Limit: %u (should be 255)\n", ipv6->hl);
        DEBUG("     - ICMP code: %u (should be 0)\n", rtr_sol->code);
        DEBUG("     - ICMP length: %u (should > %u)\n", (unsigned)icmpv6_len,
              (unsigned)sizeof(ndp_rtr_sol_t));
        return;
    }
    /* pre-check option length */
    FOREACH_OPT(rtr_sol, opt, tmp_len) {
        if (tmp_len > icmpv6_len) {
            DEBUG("nib: Payload length (%u) of RS doesn't align with options\n",
                  (unsigned)icmpv6_len);
            return;
        }
        if (opt->len == 0U) {
            DEBUG("nib: Option of length 0 detected. "
                  "Discarding router solicitation silently\n");
            return;
        }
        if ((opt->type == NDP_OPT_SL2A) &&
            ipv6_addr_is_unspecified(&ipv6->src)) {
            DEBUG("nib: RS contains SLLAO, but source was unspecfied. "
                  "Discarding router solicitation silently\n");
            return;
        }
    }
    DEBUG("nib: Received valid router solicitation:\n");
    DEBUG("     - Source address: %s\n",
          ipv6_addr_to_str(addr_str, &ipv6->src, sizeof(addr_str)));
    DEBUG("     - Destination address: %s\n",
          ipv6_addr_to_str(addr_str, &ipv6->dst, sizeof(addr_str)));
    if (!ipv6_addr_is_unspecified(&ipv6->src)) {
        tmp_len = icmpv6_len - sizeof(ndp_rtr_sol_t);
        FOREACH_OPT(rtr_sol, opt, tmp_len) {
            switch (opt->type) {
                case NDP_OPT_SL2A:
                    if (!gnrc_netif_is_6ln(netif)) {
                        _handle_sl2ao(netif, ipv6, (const icmpv6_hdr_t *)rtr_sol,
                                      opt);
                    }

                    break;
                default:
                    break;
            }
        }
        nce = _nib_onl_get(&ipv6->src, netif->pid);
    }
    if (!gnrc_netif_is_6ln(netif)) {
        uint32_t next_ra_scheduled = _evtimer_lookup(netif,
                                                     GNRC_IPV6_NIB_SND_MC_RA);
        if (next_ra_scheduled < next_ra_delay) {
            DEBUG("nib: There is a MC RA scheduled within the next %" PRIu32 "ms. "
                  "Using that to advertise router\n", next_ra_scheduled);
            return;
        }
        else if (nce != NULL) {
            /* we send unicast RAs so we do not need to rate-limit as
             * https://tools.ietf.org/html/rfc4861#section-6.2.6 asks for */
            _evtimer_add(nce, GNRC_IPV6_NIB_REPLY_RS, &nce->reply_rs,
                         next_ra_delay);
        }
        else {
            uint32_t now = (xtimer_now_usec64() / MS_PER_SEC) & UINT32_MAX;

            /* check for integer overflows and initial value of last_ra */
            if (((netif->ipv6.last_ra > (UINT32_MAX - NDP_MIN_MS_DELAY_BETWEEN_RAS) &&
                  (now < NDP_MIN_MS_DELAY_BETWEEN_RAS))) ||
                ((now - NDP_MIN_MS_DELAY_BETWEEN_RAS) > netif->ipv6.last_ra)) {
                next_ra_delay += NDP_MIN_MS_DELAY_BETWEEN_RAS;
            }
            _evtimer_add(netif, GNRC_IPV6_NIB_SND_MC_RA, &netif->ipv6.snd_mc_ra,
                         next_ra_delay);
        }
    }
#if GNRC_IPV6_NIB_CONF_6LR
    else if (gnrc_netif_is_rtr(netif) && gnrc_netif_is_rtr_adv(netif)) {
        _snd_rtr_advs(netif, &ipv6->src, false);
    }
#endif  /* GNRC_IPV6_NIB_CONF_6LR */
}
#endif  /* GNRC_IPV6_NIB_CONF_ROUTER */

static inline uint32_t _min(uint32_t a, uint32_t b)
{
    return (a < b) ? a : b;
}

static void _handle_rtr_adv(gnrc_netif_t *netif, const ipv6_hdr_t *ipv6,
                            const ndp_rtr_adv_t *rtr_adv, size_t icmpv6_len)
{
    size_t tmp_len = icmpv6_len - sizeof(ndp_rtr_adv_t);
    _nib_dr_entry_t *dr = NULL;
    ndp_opt_t *opt;

#if GNRC_IPV6_NIB_CONF_MULTIHOP_P6C
    sixlowpan_nd_opt_abr_t *abro = NULL;
    _nib_abr_entry_t *abr = NULL;
#endif  /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */
    uint32_t next_timeout = UINT32_MAX;

    assert(netif != NULL);
    /* check validity, see: https://tools.ietf.org/html/rfc4861#section-6.1.1 */
    /* checksum is checked by GNRC's ICMPv6 module */
    if (!(ipv6_addr_is_link_local(&ipv6->src)) ||
        (ipv6->hl != 255U) || (rtr_adv->code != 0U) ||
        (icmpv6_len < sizeof(ndp_rtr_adv_t)) ||
        (!gnrc_netif_is_6ln(netif) &&
         (byteorder_ntohs(rtr_adv->ltime) > NDP_RTR_ADV_LTIME_SEC_MAX))) {
        DEBUG("nib: Received router advertisement is invalid. "
              "Discarding silently\n");
        DEBUG("     - IP Hop Limit: %u (should be 255)\n", ipv6->hl);
        DEBUG("     - ICMP code: %u (should be 0)\n", rtr_adv->code);
        DEBUG("     - ICMP length: %u (should > %u)\n", (unsigned)icmpv6_len,
              (unsigned)sizeof(ndp_rtr_adv_t));
        DEBUG("     - Source address: %s (should be link-local)\n",
              ipv6_addr_to_str(addr_str, &ipv6->src, sizeof(addr_str)));
        DEBUG("     - Router lifetime: %u (should be <= 9000 on non-6LN)\n",
              byteorder_ntohs(rtr_adv->ltime));
        return;
    }
    /* pre-check option length */
    FOREACH_OPT(rtr_adv, opt, tmp_len) {
        if (tmp_len > icmpv6_len) {
            DEBUG("nib: Payload length (%u) of RA doesn't align with options\n",
                  (unsigned)icmpv6_len);
            return;
        }
        if (opt->len == 0U) {
            DEBUG("nib: Option of length 0 detected. "
                  "Discarding router advertisement silently\n");
            return;
        }
#if GNRC_IPV6_NIB_CONF_MULTIHOP_P6C
        if (opt->type == NDP_OPT_ABR) {
            if (abro != NULL) {
                DEBUG("nib: More than one ABRO. "
                      "Discarding router advertisement silently\n");
                return;
            }
            abro = (sixlowpan_nd_opt_abr_t *)opt;
        }
#endif  /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */
    }
    DEBUG("nib: Received valid router advertisement:\n");
    DEBUG("     - Source address: %s\n",
          ipv6_addr_to_str(addr_str, &ipv6->src, sizeof(addr_str)));
    DEBUG("     - Destination address: %s\n",
          ipv6_addr_to_str(addr_str, &ipv6->dst, sizeof(addr_str)));
    DEBUG("     - Cur Hop Limit: %u\n", rtr_adv->cur_hl);
    DEBUG("     - Flags: %c%c\n",
          (rtr_adv->flags & NDP_RTR_ADV_FLAGS_M) ? 'M' : '-',
          (rtr_adv->flags & NDP_RTR_ADV_FLAGS_O) ? 'O' : '-');
    DEBUG("     - Router Lifetime: %us\n", byteorder_ntohs(rtr_adv->ltime));
    DEBUG("     - Reachable Time: %" PRIu32 "ms\n",
          byteorder_ntohl(rtr_adv->reach_time));
    DEBUG("     - Retrans Timer: %" PRIu32 "ms\n",
          byteorder_ntohl(rtr_adv->retrans_timer));
#if GNRC_IPV6_NIB_CONF_MULTIHOP_P6C
    if (abro != NULL) {
        if ((abr = _handle_abro(abro)) == NULL) {
            DEBUG("nib: could not allocate space for new border router or "
                  "there is no new information in the RA. "
                  "Discarding silently\n");
            return;
        }
        /* UINT16_MAX * 60 * 1000 < UINT32_MAX so there are no overflows */
        next_timeout = _min(next_timeout,
                            byteorder_ntohs(abro->ltime) * SEC_PER_MIN *
                            MS_PER_SEC);
    }
#if !GNRC_IPV6_NIB_CONF_6LBR
    else if (gnrc_netif_is_6lr(netif)) {
        DEBUG("nib: multihop prefix and context dissemination on router activated,\n"
              "     but no ABRO found. Discarding router advertisement silently\n");
        return;
    }
#endif  /* !GNRC_IPV6_NIB_CONF_6LBR */
#endif  /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */
    if (rtr_adv->ltime.u16 != 0) {
        uint16_t rtr_ltime = byteorder_ntohs(rtr_adv->ltime);

        dr = _nib_drl_add(&ipv6->src, netif->pid);
        if (dr != NULL) {
            _evtimer_add(dr, GNRC_IPV6_NIB_RTR_TIMEOUT, &dr->rtr_timeout,
                         rtr_ltime * MS_PER_SEC);
        }
        else {
            DEBUG("nib: default router list is full. Ignoring RA from %s\n",
                  ipv6_addr_to_str(addr_str, &ipv6->src, sizeof(addr_str)));
            return;
        }
        /* UINT16_MAX * 1000 < UINT32_MAX so there are no overflows */
        next_timeout = _min(next_timeout, rtr_ltime * MS_PER_SEC);
    }
    else {
        dr = _nib_drl_get(&ipv6->src, netif->pid);

        DEBUG("nib: router lifetime was 0. Removing router and routes via it.");
        if (dr != NULL) {
            _handle_rtr_timeout(dr);
        }
        dr = NULL;
    }
    if (rtr_adv->cur_hl != 0) {
        netif->cur_hl = rtr_adv->cur_hl;
    }
#if GNRC_IPV6_NIB_CONF_ARSM
    if (rtr_adv->reach_time.u32 != 0) {
        uint32_t reach_time = byteorder_ntohl(rtr_adv->reach_time);

        if (reach_time != netif->ipv6.reach_time_base) {
            _evtimer_add(netif, GNRC_IPV6_NIB_RECALC_REACH_TIME,
                         &netif->ipv6.recalc_reach_time,
                         GNRC_IPV6_NIB_CONF_REACH_TIME_RESET);
            netif->ipv6.reach_time_base = reach_time;
            _recalc_reach_time(&netif->ipv6);
        }
    }
#endif  /* GNRC_IPV6_NIB_CONF_ARSM */
    if (rtr_adv->retrans_timer.u32 != 0) {
        netif->ipv6.retrans_time = byteorder_ntohl(rtr_adv->retrans_timer);
    }
#if GNRC_IPV6_NIB_CONF_6LN
    if ((dr != NULL) && gnrc_netif_is_6ln(netif) &&
        !gnrc_netif_is_6lbr(netif)) {
        /* (register addresses already assigned but not valid yet)*/
        for (int i = 0; i < GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
            if ((netif->ipv6.addrs_flags[i] != 0) &&
                (netif->ipv6.addrs_flags[i] != GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID)) {
                _handle_rereg_address(&netif->ipv6.addrs[i]);
            }
        }
    }
#endif  /* GNRC_IPV6_NIB_CONF_6LN */
    tmp_len = icmpv6_len - sizeof(ndp_rtr_adv_t);
    FOREACH_OPT(rtr_adv, opt, tmp_len) {
        switch (opt->type) {
            case NDP_OPT_SL2A:
                _handle_sl2ao(netif, ipv6, (const icmpv6_hdr_t *)rtr_adv,
                              opt);

                break;
            case NDP_OPT_MTU:
                _handle_mtuo(netif, (const icmpv6_hdr_t *)rtr_adv,
                             (ndp_opt_mtu_t *)opt);
                break;
            case NDP_OPT_PI: {
                uint32_t min_pfx_timeout;
#if GNRC_IPV6_NIB_CONF_MULTIHOP_P6C
                min_pfx_timeout = _handle_pio(netif,
                                              (const icmpv6_hdr_t *)rtr_adv,
                                              (ndp_opt_pi_t *)opt, abr);
#else   /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */
                min_pfx_timeout = _handle_pio(netif,
                                              (const icmpv6_hdr_t *)rtr_adv,
                                              (ndp_opt_pi_t *)opt);
#endif  /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */
                next_timeout = _min(next_timeout, min_pfx_timeout);
                break;
            }
            /* ABRO was already secured in the option check above */
#if GNRC_IPV6_NIB_CONF_6LN
            case NDP_OPT_6CTX:
#if GNRC_IPV6_NIB_CONF_MULTIHOP_P6C
                next_timeout = _min(_handle_6co((icmpv6_hdr_t *)rtr_adv,
                                                (sixlowpan_nd_opt_6ctx_t *)opt,
                                                abr), next_timeout);
#else   /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */
                next_timeout = _min(_handle_6co((icmpv6_hdr_t *)rtr_adv,
                                                (sixlowpan_nd_opt_6ctx_t *)opt),
                                    next_timeout);
#endif  /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */
                break;
#endif  /* GNRC_IPV6_NIB_CONF_6LN */
#if GNRC_IPV6_NIB_CONF_DNS
            case NDP_OPT_RDNSS:
                next_timeout = _min(_handle_rdnsso(netif,
                                                   (icmpv6_hdr_t *)rtr_adv,
                                                   (ndp_opt_rdnss_t *)opt),
                                    next_timeout);
                break;
#endif
            default:
                break;
        }
    }
    /* stop sending router solicitations
     * see https://tools.ietf.org/html/rfc4861#section-6.3.7 */
    evtimer_del(&_nib_evtimer, &netif->ipv6.search_rtr.event);
#if GNRC_IPV6_NIB_CONF_6LN
    if (gnrc_netif_is_6ln(netif) && !gnrc_netif_is_6lbr(netif)) {
        _set_rtr_adv(netif);
        /* but re-fetch information from router in time */
        _evtimer_add(netif, GNRC_IPV6_NIB_SEARCH_RTR,
                     &netif->ipv6.search_rtr, (next_timeout >> 2) * 3);
        /* i.e. 3/4 of the time before the earliest expires */
    }
#endif  /* GNRC_IPV6_NIB_CONF_6LN */
}

static inline size_t _get_l2src(const gnrc_netif_t *netif, uint8_t *l2src)
{
#if GNRC_NETIF_L2ADDR_MAXLEN > 0
    memcpy(l2src, netif->l2addr, netif->l2addr_len);
    return netif->l2addr_len;
#else   /* GNRC_NETIF_L2ADDR_MAXLEN > 0 */
    (void)netif;
    (void)l2src;
    return 0;
#endif  /* GNRC_NETIF_L2ADDR_MAXLEN > 0 */
}

static void _send_delayed_nbr_adv(const gnrc_netif_t *netif,
                                  const ipv6_addr_t *tgt,
                                  const ipv6_addr_t *dst,
                                  gnrc_pktsnip_t *reply_aro)
{
    gnrc_pktsnip_t *nbr_adv, *extra_opts = reply_aro;
    _nib_onl_entry_t *nce;
    uint8_t reply_flags = NDP_NBR_ADV_FLAGS_S;

#if GNRC_IPV6_NIB_CONF_ROUTER
    if (gnrc_netif_is_rtr(netif)) {
        reply_flags |= NDP_NBR_ADV_FLAGS_R;
    }
#endif  /* GNRC_IPV6_NIB_CONF_ROUTER */
#if GNRC_NETIF_L2ADDR_MAXLEN > 0
    if (ipv6_addr_is_multicast(dst)) {
        uint8_t l2addr[GNRC_NETIF_L2ADDR_MAXLEN];
        size_t l2addr_len = _get_l2src(netif, l2addr);

        if (l2addr_len > 0) {
            extra_opts = gnrc_ndp_opt_tl2a_build(l2addr, l2addr_len,
                                                 extra_opts);
            if (extra_opts == NULL) {
                DEBUG("nib: No space left in packet buffer. Not replying NS");
                gnrc_pktbuf_release(reply_aro);
                return;
            }
        }
        else {
            reply_flags |= NDP_NBR_ADV_FLAGS_O;
        }
    }
    else {
        reply_flags |= NDP_NBR_ADV_FLAGS_O;
    }
#else /* GNRC_NETIF_L2ADDR_MAXLEN > 0 */
    reply_flags |= NDP_NBR_ADV_FLAGS_O;
#endif  /* GNRC_NETIF_L2ADDR_MAXLEN > 0 */
    /* discard const qualifier */
    nbr_adv = gnrc_ndp_nbr_adv_build(tgt, reply_flags, extra_opts);
    if (nbr_adv == NULL) {
        DEBUG("nib: No space left in packet buffer. Not replying NS");
        gnrc_pktbuf_release(extra_opts);
        return;
    }
    nce = _nib_onl_get(tgt, netif->pid);
    if ((nce != NULL) && (nce->mode & _NC)) {
        /* usually this should be the case, but when NCE is full, just
         * ignore the sending. Other nodes in this anycast group are
         * then preferred */
        _evtimer_add(nce, GNRC_IPV6_NIB_SND_NA, &nce->snd_na,
                     random_uint32_range(0, NDP_MAX_ANYCAST_MS_DELAY));
    }
}

static void _handle_nbr_sol(gnrc_netif_t *netif, const ipv6_hdr_t *ipv6,
                            const ndp_nbr_sol_t *nbr_sol, size_t icmpv6_len)
{
    size_t tmp_len = icmpv6_len - sizeof(ndp_nbr_sol_t);
    int tgt_idx;
    ndp_opt_t *opt;

    /* check validity, see: https://tools.ietf.org/html/rfc4861#section-7.1.1 */
    /* checksum is checked by GNRC's ICMPv6 module */
    if ((ipv6->hl != 255U) || (nbr_sol->code != 0U) ||
        (icmpv6_len < sizeof(ndp_nbr_sol_t)) ||
        ipv6_addr_is_multicast(&nbr_sol->tgt) ||
        (ipv6_addr_is_unspecified(&ipv6->src) &&
         !ipv6_addr_is_solicited_node(&ipv6->dst))) {
        DEBUG("nib: Received neighbor solicitation is invalid. Discarding silently\n");
        DEBUG("     - IP Hop Limit: %u (should be 255)\n", ipv6->hl);
        DEBUG("     - ICMP code: %u (should be 0)\n", nbr_sol->code);
        DEBUG("     - ICMP length: %u (should > %u)\n", (unsigned)icmpv6_len,
              (unsigned)sizeof(ndp_nbr_sol_t));
        DEBUG("     - Target address: %s (should not be multicast)\n",
              ipv6_addr_to_str(addr_str, &nbr_sol->tgt, sizeof(addr_str)));
        DEBUG("     - Source address: %s\n",
              ipv6_addr_to_str(addr_str, &ipv6->src, sizeof(addr_str)));
        DEBUG("     - Destination address: %s (should be of format "
              "ff02::1:ffxx:xxxx if source address is ::)\n",
              ipv6_addr_to_str(addr_str, &ipv6->dst, sizeof(addr_str)));
        return;
    }
    /* check if target is assigned only now in case the length was wrong */
    tgt_idx = gnrc_netif_ipv6_addr_idx(netif, &nbr_sol->tgt);
    if (tgt_idx < 0) {
        DEBUG("nib: Target address %s is not assigned to the local interface\n",
              ipv6_addr_to_str(addr_str, &nbr_sol->tgt, sizeof(addr_str)));
        return;
    }
    /* pre-check option length */
    FOREACH_OPT(nbr_sol, opt, tmp_len) {
        if (tmp_len > icmpv6_len) {
            DEBUG("nib: Payload length (%u) of NS doesn't align with options\n",
                  (unsigned)icmpv6_len);
            return;
        }
        if (opt->len == 0U) {
            DEBUG("nib: Option of length 0 detected. "
                  "Discarding neighbor solicitation silently\n");
            return;
        }
    }
    DEBUG("nib: Received valid neighbor solicitation:\n");
    DEBUG("     - Target address: %s\n",
          ipv6_addr_to_str(addr_str, &nbr_sol->tgt, sizeof(addr_str)));
    DEBUG("     - Source address: %s\n",
          ipv6_addr_to_str(addr_str, &ipv6->src, sizeof(addr_str)));
    DEBUG("     - Destination address: %s\n",
          ipv6_addr_to_str(addr_str, &ipv6->dst, sizeof(addr_str)));
#if GNRC_IPV6_NIB_CONF_SLAAC
    gnrc_netif_t *tgt_netif = gnrc_netif_get_by_ipv6_addr(&nbr_sol->tgt);

    if (tgt_netif != NULL) {
        int idx = gnrc_netif_ipv6_addr_idx(tgt_netif, &nbr_sol->tgt);

        if (gnrc_netif_ipv6_addr_dad_trans(tgt_netif, idx)) {
            if (!ipv6_addr_is_unspecified(&ipv6->src)) {
                /* (see https://tools.ietf.org/html/rfc4862#section-5.4.3) */
                DEBUG("nib: Neighbor is performing AR, but target address is "
                      "still TENTATIVE for us => Ignoring NS\n");
                return;
            }
            /* cancel validation timer */
            evtimer_del(&_nib_evtimer,
                        &tgt_netif->ipv6.addrs_timers[idx].event);
            _remove_tentative_addr(tgt_netif, &nbr_sol->tgt);
            return;
        }
    }
#endif  /* GNRC_IPV6_NIB_CONF_SLAAC */
    if (ipv6_addr_is_unspecified(&ipv6->src)) {
        gnrc_ndp_nbr_adv_send(&nbr_sol->tgt, netif, &ipv6->src, false, NULL);
    }
    else {
        gnrc_pktsnip_t *reply_aro = NULL;
#if GNRC_IPV6_NIB_CONF_6LR
        ndp_opt_t *sl2ao = NULL;
        sixlowpan_nd_opt_ar_t *aro = NULL;
#else   /* GNRC_IPV6_NIB_CONF_6LR */
#define sl2ao   (NULL)
#define aro     (NULL)
#endif  /* GNRC_IPV6_NIB_CONF_6LR */
        tmp_len = icmpv6_len - sizeof(ndp_nbr_sol_t);

        if (!(netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR)) {
            /* Set STALE NCE if link-layer has no addresses */
            _nib_nc_add(&ipv6->src, netif->pid,
                        GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE);
        }
        FOREACH_OPT(nbr_sol, opt, tmp_len) {
            switch (opt->type) {
                case NDP_OPT_SL2A:
#if GNRC_IPV6_NIB_CONF_6LR
                    if (gnrc_netif_is_6lr(netif)) {
                        DEBUG("nib: Storing SL2AO for later handling\n");
                        sl2ao = opt;
                        break;
                    }
#endif  /* GNRC_IPV6_NIB_CONF_6LR */
                    _handle_sl2ao(netif, ipv6, (const icmpv6_hdr_t *)nbr_sol,
                                  opt);
                    break;
#if GNRC_IPV6_NIB_CONF_6LR
                case NDP_OPT_AR:
                    DEBUG("nib: Storing ARO for later handling\n");
                    aro = (sixlowpan_nd_opt_ar_t *)opt;
                    break;
#endif  /* GNRC_IPV6_NIB_CONF_6LR */
                default:
                    DEBUG("nib: Ignoring unrecognized option type %u for NS\n",
                          opt->type);
                    break;
            }
        }
        reply_aro = _copy_and_handle_aro(netif, ipv6, nbr_sol, aro, sl2ao);
        /* check if target address is anycast */
        if (netif->ipv6.addrs_flags[tgt_idx] & GNRC_NETIF_IPV6_ADDRS_FLAGS_ANYCAST) {
            _send_delayed_nbr_adv(netif, &nbr_sol->tgt, &ipv6->dst, reply_aro);
        }
        else {
            gnrc_ndp_nbr_adv_send(&nbr_sol->tgt, netif, &ipv6->src,
                                  ipv6_addr_is_multicast(&ipv6->dst),
                                  reply_aro);
        }
    }
}

static void _handle_nbr_adv(gnrc_netif_t *netif, const ipv6_hdr_t *ipv6,
                            const ndp_nbr_adv_t *nbr_adv, size_t icmpv6_len)
{
    size_t tmp_len = icmpv6_len - sizeof(ndp_nbr_adv_t);
    ndp_opt_t *opt;
    _nib_onl_entry_t *nce;

    /* check validity, see: https://tools.ietf.org/html/rfc4861#section-7.1.2 */
    /* checksum is checked by GNRC's ICMPv6 module */
    if ((ipv6->hl != 255U) || (nbr_adv->code != 0U) ||
        (icmpv6_len < sizeof(ndp_nbr_adv_t)) ||
        ipv6_addr_is_multicast(&nbr_adv->tgt) ||
        (ipv6_addr_is_multicast(&ipv6->dst) &&
         (nbr_adv->flags & NDP_NBR_ADV_FLAGS_S))) {
        DEBUG("nib: Received neighbor advertisement is invalid. Discarding silently\n");
        DEBUG("     - IP Hop Limit: %u (should be 255)\n", ipv6->hl);
        DEBUG("     - ICMP code: %u (should be 0)\n", nbr_adv->code);
        DEBUG("     - ICMP length: %u (should > %u)\n", (unsigned)icmpv6_len,
              (unsigned)sizeof(ndp_nbr_adv_t));
        DEBUG("     - Target address: %s (should not be multicast)\n",
              ipv6_addr_to_str(addr_str, &nbr_adv->tgt, sizeof(addr_str)));
        DEBUG("     - Destination address: %s\n",
              ipv6_addr_to_str(addr_str, &ipv6->dst, sizeof(addr_str)));
        DEBUG("     - Flags: %c%c%c (S must not be set if destination is multicast)\n",
              (nbr_adv->flags & NDP_NBR_ADV_FLAGS_R) ? 'R' : '-',
              (nbr_adv->flags & NDP_NBR_ADV_FLAGS_S) ? 'S' : '-',
              (nbr_adv->flags & NDP_NBR_ADV_FLAGS_O) ? 'O' : '-');
        return;
    }
    /* pre-check option length */
    FOREACH_OPT(nbr_adv, opt, tmp_len) {
        if (tmp_len > icmpv6_len) {
            DEBUG("nib: Payload length (%u) of NA doesn't align with options\n",
                  (unsigned)icmpv6_len);
            return;
        }
        if (opt->len == 0U) {
            DEBUG("nib: Option of length 0 detected. "
                  "Discarding neighbor advertisement silently\n");
            return;
        }
    }
    DEBUG("nib: Received valid neighbor advertisement:\n");
    DEBUG("     - Target address: %s\n",
          ipv6_addr_to_str(addr_str, &nbr_adv->tgt, sizeof(addr_str)));
    DEBUG("     - Source address: %s\n",
          ipv6_addr_to_str(addr_str, &ipv6->src, sizeof(addr_str)));
    DEBUG("     - Destination address: %s\n",
          ipv6_addr_to_str(addr_str, &ipv6->dst, sizeof(addr_str)));
    DEBUG("     - Flags: %c%c%c\n",
          (nbr_adv->flags & NDP_NBR_ADV_FLAGS_R) ? 'R' : '-',
          (nbr_adv->flags & NDP_NBR_ADV_FLAGS_S) ? 'S' : '-',
          (nbr_adv->flags & NDP_NBR_ADV_FLAGS_O) ? 'O' : '-');
#if GNRC_IPV6_NIB_CONF_SLAAC
    gnrc_netif_t *tgt_netif = gnrc_netif_get_by_ipv6_addr(&nbr_adv->tgt);

    if (tgt_netif != NULL) {
        int idx = gnrc_netif_ipv6_addr_idx(tgt_netif, &nbr_adv->tgt);

        if (gnrc_netif_ipv6_addr_dad_trans(tgt_netif, idx)) {
            /* cancel validation timer */
            evtimer_del(&_nib_evtimer,
                        &tgt_netif->ipv6.addrs_timers[idx].event);
            _remove_tentative_addr(tgt_netif, &nbr_adv->tgt);
            return;
        }
        /* else case beyond scope of RFC4862:
         * https://tools.ietf.org/html/rfc4862#section-5.4.4 */
    }
#endif  /* GNRC_IPV6_NIB_CONF_SLAAC */
    if (((nce = _nib_onl_get(&nbr_adv->tgt, netif->pid)) != NULL) &&
        (nce->mode & _NC)) {
#if GNRC_IPV6_NIB_CONF_ARSM
        bool tl2ao_avail = false;
#endif  /* GNRC_IPV6_NIB_CONF_ARSM */
#if GNRC_IPV6_NIB_CONF_6LN
        uint8_t aro_status = _ADDR_REG_STATUS_UNAVAIL;
#endif

        tmp_len = icmpv6_len - sizeof(ndp_nbr_adv_t);
        FOREACH_OPT(nbr_adv, opt, tmp_len) {
            switch (opt->type) {
#if GNRC_IPV6_NIB_CONF_ARSM
                case NDP_OPT_TL2A:
                    _handle_adv_l2(netif, nce, (icmpv6_hdr_t *)nbr_adv, opt);
                    tl2ao_avail = true;
                    break;
#endif  /* GNRC_IPV6_NIB_CONF_ARSM */
#if GNRC_IPV6_NIB_CONF_6LN
                case NDP_OPT_AR:
                    aro_status = _handle_aro(netif, ipv6,
                                             (const icmpv6_hdr_t *)nbr_adv,
                                             (const sixlowpan_nd_opt_ar_t *)opt,
                                             opt, nce);
                    break;
#endif  /* GNRC_IPV6_NIB_CONF_6LN */
                default:
                    DEBUG("nib: Ignoring unrecognized option type %u for NA\n",
                          opt->type);
            }
        }
#if GNRC_IPV6_NIB_CONF_ARSM
        if (!tl2ao_avail && (nbr_adv->flags & NDP_NBR_ADV_FLAGS_S) &&
            (_get_nud_state(nce) != GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE)) {
            /* reachability confirmed without TL2AO */
            _set_reachable(netif, nce);
        }
        if (!(netif->flags & GNRC_NETIF_FLAGS_HAS_L2ADDR)) {
            _handle_adv_l2(netif, nce, (icmpv6_hdr_t *)nbr_adv, NULL);
        }
#endif  /* GNRC_IPV6_NIB_CONF_ARSM */
#if GNRC_IPV6_NIB_CONF_SLAAC && GNRC_IPV6_NIB_CONF_6LN
        /* 6Lo-ND duplicate address detection (DAD) was ignored by neighbor, try
         * traditional DAD */
        if ((aro_status == _ADDR_REG_STATUS_UNAVAIL) &&
            gnrc_netif_is_6ln(netif)) {
            _handle_dad(&ipv6->dst);
        }
#elif GNRC_IPV6_NIB_CONF_6LN
        (void)aro_status;
#endif
    }
}

#if GNRC_IPV6_NIB_CONF_QUEUE_PKT
static gnrc_pktqueue_t *_alloc_queue_entry(gnrc_pktsnip_t *pkt)
{
    for (int i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        if (_queue_pool[i].pkt == NULL) {
            _queue_pool[i].pkt = pkt;
            return &_queue_pool[i];
        }
    }
    return NULL;
}
#endif  /* GNRC_IPV6_NIB_CONF_QUEUE_PKT */

static bool _resolve_addr(const ipv6_addr_t *dst, gnrc_netif_t *netif,
                          gnrc_pktsnip_t *pkt, gnrc_ipv6_nib_nc_t *nce,
                          _nib_onl_entry_t *entry)
{
    bool res = false;

    if ((netif != NULL) && (netif->device_type == NETDEV_TYPE_SLIP)) {
        /* XXX: Linux doesn't do neighbor discovery for SLIP so no use sending
         * NS and since SLIP doesn't have link-layer addresses anyway, we can
         * just return the interface without any link-layer addresses */
        memcpy(&nce->ipv6, dst, sizeof(nce->ipv6));
        nce->info = (netif->pid << _NIB_IF_POS);
        nce->l2addr_len = 0;
        return true;
    }
#if GNRC_IPV6_NIB_CONF_ARSM
    if ((entry != NULL) && (entry->mode & _NC) && _is_reachable(entry)) {
        if (_get_nud_state(entry) == GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE) {
            _set_nud_state(netif, entry, GNRC_IPV6_NIB_NC_INFO_NUD_STATE_DELAY);
            _evtimer_add(entry, GNRC_IPV6_NIB_DELAY_TIMEOUT,
                         &entry->nud_timeout, NDP_DELAY_FIRST_PROBE_MS);
        }
        DEBUG("nib: resolve address %s%%%u from neighbor cache\n",
              ipv6_addr_to_str(addr_str, &entry->ipv6, sizeof(addr_str)),
              _nib_onl_get_if(entry));
        _nib_nc_get(entry, nce);
        res = true;
    }
#else   /* GNRC_IPV6_NIB_CONF_ARSM */
    if (entry != NULL) {
        DEBUG("nib: resolve address %s%%%u from neighbor cache\n",
              ipv6_addr_to_str(addr_str, &entry->ipv6, sizeof(addr_str)),
              _nib_onl_get_if(entry));
        _nib_nc_get(entry, nce);
        res = true;
    }
#endif  /* GNRC_IPV6_NIB_CONF_ARSM */
    else if (!(res = _resolve_addr_from_ipv6(dst, netif, nce))) {
#if GNRC_IPV6_NIB_CONF_ARSM
        bool reset = false;
#endif  /* GNRC_IPV6_NIB_CONF_ARSM */

        DEBUG("nib: resolve address %s by probing neighbors\n",
              ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)));
        if ((entry == NULL) || !(entry->mode & _NC)) {
            entry = _nib_nc_add(dst, (netif != NULL) ? netif->pid : 0,
                                GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE);
            if (entry == NULL) {
                return false;
            }
#if GNRC_IPV6_NIB_CONF_ROUTER
            if (netif != NULL) {
                _call_route_info_cb(netif,
                                    GNRC_IPV6_NIB_ROUTE_INFO_TYPE_NSC,
                                    dst,
                                    (void *)GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE);
            }
#endif  /* GNRC_IPV6_NIB_CONF_ROUTER */
#if GNRC_IPV6_NIB_CONF_ARSM
            reset = true;
#endif  /* GNRC_IPV6_NIB_CONF_ARSM */
        }
#if GNRC_IPV6_NIB_CONF_ARSM
        else if (_get_nud_state(entry) == GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE) {
            /* reduce back-off to possibly resolve neighbor sooner again */
            entry->ns_sent = 3;
        }
#endif  /* GNRC_IPV6_NIB_CONF_ARSM */
        if (pkt != NULL) {
#if GNRC_IPV6_NIB_CONF_QUEUE_PKT
            if (_get_nud_state(entry) == GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE) {
                gnrc_pktqueue_t *queue_entry = _alloc_queue_entry(pkt);

                if (queue_entry != NULL) {
                    gnrc_pktqueue_add(&entry->pktqueue, queue_entry);
                }
            }
            else {
                gnrc_pktbuf_release_error(pkt, EHOSTUNREACH);
            }
#else   /* GNRC_IPV6_NIB_CONF_QUEUE_PKT */
            gnrc_pktbuf_release_error(pkt, EHOSTUNREACH);
#endif  /* GNRC_IPV6_NIB_CONF_QUEUE_PKT */
        }
#if GNRC_IPV6_NIB_CONF_ARSM
        _probe_nbr(entry, reset);
#endif  /* GNRC_IPV6_NIB_CONF_ARSM */
    }
    return res;
}

static void _handle_snd_na(gnrc_pktsnip_t *pkt)
{
#ifdef MODULE_GNRC_IPV6
    DEBUG("nib: Send delayed neighbor advertisement\n");
    if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_IPV6, GNRC_NETREG_DEMUX_CTX_ALL,
                                   pkt)) {
        DEBUG("nib: No receivers for neighbor advertisement\n");
        gnrc_pktbuf_release_error(pkt, EBADF);
    }
#else   /* MODULE_GNRC_IPV6 */
    (void)pkt;
    DEBUG("nib: No IPv6 module to send delayed neighbor advertisement\n");
#endif  /* MODULE_GNRC_IPV6 */
}

static void _handle_pfx_timeout(_nib_offl_entry_t *pfx)
{
    gnrc_netif_t *netif = gnrc_netif_get_by_pid(_nib_onl_get_if(pfx->next_hop));
    uint32_t now = (xtimer_now_usec64() / US_PER_MS) & UINT32_MAX;

    gnrc_netif_acquire(netif);
    if (now >= pfx->valid_until) {
        evtimer_del(&_nib_evtimer, &pfx->pfx_timeout.event);
        for (int i = 0; i < GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
            if (ipv6_addr_match_prefix(&netif->ipv6.addrs[i],
                                       &pfx->pfx) >= pfx->pfx_len) {
                gnrc_netif_ipv6_addr_remove_internal(netif,
                                                     &netif->ipv6.addrs[i]);
            }
        }
        pfx->mode &= ~_PL;
        _nib_offl_clear(pfx);
    }
    else if (now >= pfx->pref_until) {
        for (int i = 0; i < GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
            if (ipv6_addr_match_prefix(&netif->ipv6.addrs[i],
                                       &pfx->pfx) >= pfx->pfx_len) {
                netif->ipv6.addrs_flags[i] &= ~GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_MASK;
                netif->ipv6.addrs_flags[i] |= GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_DEPRECATED;
            }
        }
        _evtimer_add(pfx, GNRC_IPV6_NIB_PFX_TIMEOUT, &pfx->pfx_timeout,
                     pfx->valid_until - now);
    }
    gnrc_netif_release(netif);
}

static void _handle_rtr_timeout(_nib_dr_entry_t *router)
{
    if ((router->next_hop != NULL) && (router->next_hop->mode & _DRL)) {
        _nib_offl_entry_t *route = NULL;
        unsigned iface = _nib_onl_get_if(router->next_hop);
        ipv6_addr_t addr;

        memcpy(&addr, &router->next_hop, sizeof(addr));
        _nib_drl_remove(router);
        /* also remove all routes to that router */
        while ((route = _nib_offl_iter(route))) {
            if ((route->next_hop != NULL) &&
                (_nib_onl_get_if(route->next_hop) == iface) &&
                (ipv6_addr_equal(&route->next_hop->ipv6, &addr))) {
                route->mode = _EMPTY;
                route->next_hop->mode &= ~_DST;
                _nib_offl_clear(route);
                /* XXX routing protocol get's informed in case NUD
                 * determines ipv6->src (still in neighbor cache) to be
                 * unreachable */
            }
        }
    }
}

void _handle_search_rtr(gnrc_netif_t *netif)
{
#if !GNRC_IPV6_NIB_CONF_NO_RTR_SOL
    gnrc_netif_acquire(netif);
    if (!(gnrc_netif_is_rtr_adv(netif)) || gnrc_netif_is_6ln(netif)) {
        uint32_t next_rs = _evtimer_lookup(netif, GNRC_IPV6_NIB_SEARCH_RTR);
        uint32_t interval = _get_next_rs_interval(netif);

        if (next_rs > interval) {
            gnrc_ndp_rtr_sol_send(netif, &ipv6_addr_all_routers_link_local);
            if (netif->ipv6.rs_sent < 10U) {
                /* with more the backoff (required in RFC 6775) is truncated
                 * anyway and this way we prevent overflows. 10 is arbitrary, so
                 * we do not need a define here */
                netif->ipv6.rs_sent++;
            }
            if ((netif->ipv6.rs_sent < NDP_MAX_RS_NUMOF) ||
                gnrc_netif_is_6ln(netif)) {
                /* 6LN will solicitate indefinitely */
                _evtimer_add(netif, GNRC_IPV6_NIB_SEARCH_RTR,
                             &netif->ipv6.search_rtr, interval);
            }
        }
    }
    gnrc_netif_release(netif);
#else
    (void)netif;
#endif /* !GNRC_IPV6_NIB_CONF_NO_RTR_SOL */
}

#if GNRC_IPV6_NIB_CONF_DNS
static void _handle_rdnss_timeout(sock_udp_ep_t *dns_server)
{
    memset(dns_server, 0, sizeof(sock_udp_ep_t));
}
#endif

static void _handle_mtuo(gnrc_netif_t *netif, const icmpv6_hdr_t *icmpv6,
                         const ndp_opt_mtu_t *mtuo)
{
    if ((mtuo->len != NDP_OPT_MTU_LEN) || (icmpv6->type != ICMPV6_RTR_ADV)) {
        return;
    }
    if (byteorder_ntohl(mtuo->mtu) >= IPV6_MIN_MTU) {
        netif->ipv6.mtu = byteorder_ntohl(mtuo->mtu);
    }
}

#if GNRC_IPV6_NIB_CONF_DNS
static uint32_t _handle_rdnsso(gnrc_netif_t *netif, const icmpv6_hdr_t *icmpv6,
                               const ndp_opt_rdnss_t *rdnsso)
{
    uint32_t ltime = UINT32_MAX;
    const ipv6_addr_t *addr;

    if ((rdnsso->len < NDP_OPT_RDNSS_MIN_LEN) ||
        (icmpv6->type != ICMPV6_RTR_ADV)) {
        return ltime;
    }
    /* select first if unassigned, search possible address otherwise */
    addr = (sock_dns_server.port == 0) ? &rdnsso->addrs[0] : NULL;
    if (addr == NULL) {
        unsigned addrs_num = (rdnsso->len - 1) / 2;
        for (unsigned i = 0; i < addrs_num; i++) {
            if (memcmp(sock_dns_server.addr.ipv6,
                       &rdnsso->addrs[i],
                       sizeof(rdnsso->addrs[i])) == 0) {
                addr = &rdnsso->addrs[i];
                break;
            }
        }
    }
#if SOCK_HAS_IPV6
    ltime = byteorder_ntohl(rdnsso->ltime);
    if (addr != NULL) {
        if (ltime > 0) {
            sock_dns_server.port = SOCK_DNS_PORT;
            sock_dns_server.family = AF_INET6;
            sock_dns_server.netif = netif->pid;
            memcpy(sock_dns_server.addr.ipv6, rdnsso->addrs,
                   sizeof(sock_dns_server.addr.ipv6));

            if (ltime < UINT32_MAX) {
                /* the valid lifetime is given in seconds, but our timers work
                 * in milliseconds, so we have to scale down to the smallest
                 * possible value (UINT32_MAX - 1). This is however alright
                 * since we ask for a new router advertisement before this
                 * timeout expires */
                ltime = (ltime > (UINT32_MAX / MS_PER_SEC)) ?
                              (UINT32_MAX - 1) : ltime * MS_PER_SEC;
                _evtimer_add(&sock_dns_server, GNRC_IPV6_NIB_RDNSS_TIMEOUT,
                             &_rdnss_timeout, ltime);
            }
        }
        else {
            evtimer_del(&_nib_evtimer, &_rdnss_timeout.event);
            _handle_rdnss_timeout(&sock_dns_server);
        }
    }
#else
    (void)addr;
#endif
    return ltime;
}
#endif

static void _remove_prefix(const ipv6_addr_t *pfx, unsigned pfx_len)
{
    _nib_offl_entry_t *offl = NULL;

    while ((offl = _nib_offl_iter(offl))) {
        if ((offl->mode & _PL) &&
            (offl->pfx_len == pfx_len) &&
            (ipv6_addr_match_prefix(&offl->pfx, pfx) >= pfx_len)) {
            _nib_pl_remove(offl);
        }
    }
    return;
}

#if GNRC_IPV6_NIB_CONF_MULTIHOP_P6C
static uint32_t _handle_pio(gnrc_netif_t *netif, const icmpv6_hdr_t *icmpv6,
                            const ndp_opt_pi_t *pio, _nib_abr_entry_t *abr)
#else   /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */
static uint32_t _handle_pio(gnrc_netif_t *netif, const icmpv6_hdr_t *icmpv6,
                            const ndp_opt_pi_t *pio)
#endif  /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */
{
    uint32_t valid_ltime;
    uint32_t pref_ltime;

    valid_ltime = byteorder_ntohl(pio->valid_ltime);
    pref_ltime = byteorder_ntohl(pio->pref_ltime);
    if ((pio->len != NDP_OPT_PI_LEN) || (icmpv6->type != ICMPV6_RTR_ADV) ||
        ipv6_addr_is_link_local(&pio->prefix) || (valid_ltime < pref_ltime)) {
        DEBUG("nib: ignoring PIO with invalid data\n");
        return UINT32_MAX;
    }
    DEBUG("nib: received valid Prefix Information option:\n");
    DEBUG("     - Prefix: %s/%u\n",
          ipv6_addr_to_str(addr_str, &pio->prefix, sizeof(addr_str)),
          pio->prefix_len);
    DEBUG("     - Flags: %c%c\n",
          (pio->flags & NDP_OPT_PI_FLAGS_L) ? 'L' : '-',
          (pio->flags & NDP_OPT_PI_FLAGS_A) ? 'A' : '-');
    DEBUG("     - Valid lifetime: %" PRIu32 "\n",
          byteorder_ntohl(pio->valid_ltime));
    DEBUG("     - Preferred lifetime: %" PRIu32 "\n",
          byteorder_ntohl(pio->pref_ltime));

    if (pio->flags & NDP_OPT_PI_FLAGS_A) {
        _auto_configure_addr(netif, &pio->prefix, pio->prefix_len);
    }
    if ((pio->flags & NDP_OPT_PI_FLAGS_L) || gnrc_netif_is_6lr(netif)) {
        _nib_offl_entry_t *pfx;

        if (pio->valid_ltime.u32 == 0) {
            DEBUG("nib: PIO for %s/%u with lifetime 0. Removing prefix.\n",
                  ipv6_addr_to_str(addr_str, &pio->prefix, sizeof(addr_str)),
                  pio->prefix_len);
            _remove_prefix(&pio->prefix, pio->prefix_len);
            return UINT32_MAX;
        }

        if (valid_ltime < UINT32_MAX) { /* UINT32_MAX means infinite lifetime */
            /* the valid lifetime is given in seconds, but our timers work in
             * microseconds, so we have to scale down to the smallest possible
             * value (UINT32_MAX - 1). This is however alright since we ask for
             * a new router advertisement before this timeout expires */
            valid_ltime = (valid_ltime > (UINT32_MAX / MS_PER_SEC)) ?
                          (UINT32_MAX - 1) : valid_ltime * MS_PER_SEC;
        }
        if (pref_ltime < UINT32_MAX) { /* UINT32_MAX means infinite lifetime */
            /* same treatment for pref_ltime */
            pref_ltime = (pref_ltime > (UINT32_MAX / MS_PER_SEC)) ?
                         (UINT32_MAX - 1) : pref_ltime * MS_PER_SEC;
        }
        if ((pfx = _nib_pl_add(netif->pid, &pio->prefix, pio->prefix_len,
                               valid_ltime, pref_ltime))) {
#if GNRC_IPV6_NIB_CONF_MULTIHOP_P6C
            assert(abr != NULL);    /* should have been set in _handle_abro() */
            _nib_abr_add_pfx(abr, pfx);
#endif  /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */
            if (pio->flags & NDP_OPT_PI_FLAGS_L) {
                pfx->flags |= _PFX_ON_LINK;
            }
            if (pio->flags & NDP_OPT_PI_FLAGS_A) {
                pfx->flags |= _PFX_SLAAC;
            }
            return _min(pref_ltime, valid_ltime);
        }
    }
    return UINT32_MAX;
}

/** @} */
