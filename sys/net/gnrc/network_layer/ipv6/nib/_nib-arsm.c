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

#include "xtimer.h"
#include "net/gnrc/ndp.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/netif/internal.h"
#ifdef MODULE_GNRC_SIXLOWPAN_ND
#include "net/gnrc/sixlowpan/nd.h"
#endif  /* MODULE_GNRC_SIXLOWPAN_ND */

#include "_nib-arsm.h"
#include "_nib-router.h"
#include "_nib-6lr.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static char addr_str[IPV6_ADDR_MAX_STR_LEN];

/**
 * @brief   Determines supposed link-layer address from interface and option
 *          length
 *
 * @param[in] netif A network interface.
 * @param[in] opt   A SL2AO or TL2AO.
 *
 * @return  The length of the L2 address carried in @p opt.
 */
static inline unsigned _get_l2addr_len(gnrc_netif_t *netif,
                                       const ndp_opt_t *opt);

void _snd_ns(const ipv6_addr_t *tgt, gnrc_netif_t *netif,
             const ipv6_addr_t *src, const ipv6_addr_t *dst)
{
    gnrc_pktsnip_t *ext_opt = NULL;

#ifdef MODULE_GNRC_SIXLOWPAN_ND
    _nib_dr_entry_t *dr = _nib_drl_get_dr();

    assert(netif != NULL);
    /* add ARO based on interface */
    if ((src != NULL) && gnrc_netif_is_6ln(netif) &&
        (_nib_onl_get_if(dr->next_hop) == (unsigned)netif->pid) &&
        ipv6_addr_equal(&dr->next_hop->ipv6, dst)) {
        netdev_t *dev = netif->dev;
        uint8_t l2src[GNRC_NETIF_L2ADDR_MAXLEN];
        size_t l2src_len = (uint16_t)dev->driver->get(dev, NETOPT_ADDRESS_LONG,
                                                      l2src, sizeof(l2src));
        if (l2src_len != sizeof(eui64_t)) {
            DEBUG("nib: can't get EUI-64 of the interface for ARO\n");
            return;
        }
        ext_opt = gnrc_sixlowpan_nd_opt_ar_build(0, GNRC_SIXLOWPAN_ND_AR_LTIME,
                                                 (eui64_t *)l2src, NULL);
        if (ext_opt == NULL) {
            DEBUG("nib: error allocating ARO.\n");
            return;
        }
    }
#endif  /* MODULE_GNRC_SIXLOWPAN_ND */
    gnrc_ndp_nbr_sol_send(tgt, netif, src, dst, ext_opt);
}

void _snd_uc_ns(_nib_onl_entry_t *nbr, bool reset)
{
    gnrc_netif_t *netif = gnrc_netif_get_by_pid(_nib_onl_get_if(nbr));

    assert(netif != NULL);
    gnrc_netif_acquire(netif);
    DEBUG("nib: unicast to %s (retrans. timer = %ums)\n",
          ipv6_addr_to_str(addr_str, &nbr->ipv6, sizeof(addr_str)),
          (unsigned)netif->ipv6.retrans_time);
#if GNRC_IPV6_NIB_CONF_ARSM
    if (reset) {
        nbr->ns_sent = 0;
    }
#else   /* GNRC_IPV6_NIB_CONF_ARSM */
    (void)reset;
#endif  /* GNRC_IPV6_NIB_CONF_ARSM */
    _snd_ns(&nbr->ipv6, netif, NULL, &nbr->ipv6);
    _evtimer_add(nbr, GNRC_IPV6_NIB_SND_UC_NS, &nbr->nud_timeout,
                 netif->ipv6.retrans_time);
    gnrc_netif_release(netif);
#if GNRC_IPV6_NIB_CONF_ARSM
    nbr->ns_sent++;
#endif  /* GNRC_IPV6_NIB_CONF_ARSM */
}

void _handle_sl2ao(gnrc_netif_t *netif, const ipv6_hdr_t *ipv6,
                   const icmpv6_hdr_t *icmpv6, const ndp_opt_t *sl2ao)
{
    assert(netif != NULL);
    _nib_onl_entry_t *nce = _nib_onl_get(&ipv6->src, netif->pid);
    unsigned l2addr_len;

    l2addr_len = _get_l2addr_len(netif, sl2ao);
    if (l2addr_len == 0U) {
        DEBUG("nib: Unexpected SL2AO length. Ignoring SL2AO\n");
        return;
    }
#if GNRC_IPV6_NIB_CONF_ARSM
    if ((nce != NULL) && (nce->mode & _NC) &&
        ((nce->l2addr_len != l2addr_len) ||
         (memcmp(nce->l2addr, sl2ao + 1, nce->l2addr_len) != 0)) &&
        /* a 6LR MUST NOT modify an existing NCE based on an SL2AO in an RS
         * see https://tools.ietf.org/html/rfc6775#section-6.3 */
        !_rtr_sol_on_6lr(netif, icmpv6)) {
        DEBUG("nib: L2 address differs. Setting STALE\n");
        evtimer_del(&_nib_evtimer, &nce->nud_timeout.event);
        _set_nud_state(netif, nce, GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE);
    }
#endif  /* GNRC_IPV6_NIB_CONF_ARSM */
    if ((nce == NULL) || !(nce->mode & _NC)) {
        DEBUG("nib: Creating NCE for (ipv6 = %s, iface = %u, nud_state = STALE)\n",
              ipv6_addr_to_str(addr_str, &ipv6->src, sizeof(addr_str)),
              netif->pid);
        nce = _nib_nc_add(&ipv6->src, netif->pid,
                          GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE);
        if (nce != NULL) {
            if (icmpv6->type == ICMPV6_NBR_SOL) {
                nce->info &= ~GNRC_IPV6_NIB_NC_INFO_IS_ROUTER;
            }
#if GNRC_IPV6_NIB_CONF_MULTIHOP_DAD && GNRC_IPV6_NIB_CONF_6LR
            else if (_rtr_sol_on_6lr(netif, icmpv6)) {
                DEBUG("nib: Setting newly created entry to tentative\n");
                _set_ar_state(nce, GNRC_IPV6_NIB_NC_INFO_AR_STATE_TENTATIVE);
                _evtimer_add(nce, GNRC_IPV6_NIB_ADDR_REG_TIMEOUT,
                             &nce->addr_reg_timeout,
                             SIXLOWPAN_ND_TENTATIVE_NCE_SEC_LTIME * MS_PER_SEC);
            }
#endif  /* GNRC_IPV6_NIB_CONF_MULTIHOP_DAD && GNRC_IPV6_NIB_CONF_6LR */
        }
#if ENABLE_DEBUG
        else {
            DEBUG("nib: Neighbor cache full\n");
        }
#endif  /* ENABLE_DEBUG */
    }
    /* not else to include NCE created in nce == NULL branch */
    if ((nce != NULL) && (nce->mode & _NC)) {
        if (icmpv6->type == ICMPV6_RTR_ADV) {
            DEBUG("nib: %s%%%u is a router\n",
                  ipv6_addr_to_str(addr_str, &nce->ipv6, sizeof(addr_str)),
                  netif->pid);
            nce->info |= GNRC_IPV6_NIB_NC_INFO_IS_ROUTER;
        }
        else if (icmpv6->type != ICMPV6_NBR_SOL) {
            DEBUG("nib: %s%%%u is probably not a router\n",
                  ipv6_addr_to_str(addr_str, &nce->ipv6, sizeof(addr_str)),
                  netif->pid);
            nce->info &= ~GNRC_IPV6_NIB_NC_INFO_IS_ROUTER;
        }
#if GNRC_IPV6_NIB_CONF_ARSM
        /* a 6LR MUST NOT modify an existing NCE based on an SL2AO in an RS
         * see https://tools.ietf.org/html/rfc6775#section-6.3 */
        if (!_rtr_sol_on_6lr(netif, icmpv6)) {
            nce->l2addr_len = l2addr_len;
            memcpy(nce->l2addr, sl2ao + 1, l2addr_len);
        }
#endif  /* GNRC_IPV6_NIB_CONF_ARSM */
    }
}

static inline unsigned _get_l2addr_len(gnrc_netif_t *netif,
                                       const ndp_opt_t *opt)
{
    switch (netif->device_type) {
#ifdef MODULE_CC110X
        case NETDEV_TYPE_CC110X:
            (void)opt;
            return sizeof(uint8_t);
#endif  /* MODULE_CC110X */
#ifdef MODULE_NETDEV_ETH
        case NETDEV_TYPE_ETHERNET:
            (void)opt;
            return ETHERNET_ADDR_LEN;
#endif  /* MODULE_NETDEV_ETH */
#ifdef MODULE_NRFMIN
        case NETDEV_TYPE_NRFMIN:
            (void)opt;
            return sizeof(uint16_t);
#endif  /* MODULE_NRFMIN */
#if defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE)
        case NETDEV_TYPE_IEEE802154:
            switch (opt->len) {
                case 1U:
                    return IEEE802154_SHORT_ADDRESS_LEN;
                case 2U:
                    return IEEE802154_LONG_ADDRESS_LEN;
                default:
                    return 0U;
            }
#endif  /* defined(MODULE_NETDEV_IEEE802154) || defined(MODULE_XBEE) */
#ifdef MODULE_ESP_NOW
        case NETDEV_TYPE_RAW:
            (void)opt;
            return ETHERNET_ADDR_LEN;
#endif  /* MODULE_ESP_NOW */
        default:
            (void)opt;
            return 0U;
    }
}

#if GNRC_IPV6_NIB_CONF_ARSM
/**
 * @brief   Calculates exponential back-off for retransmission timer for
 *          neighbor solicitations
 *
 * @param[in] ns_sent       Neighbor solicitations sent up until now.
 * @param[in] retrans_timer Currently configured retransmission timer.
 *
 * @return  exponential back-off of the retransmission timer
 */
static inline uint32_t _exp_backoff_retrans_timer(uint8_t ns_sent,
                                                  uint32_t retrans_timer);
#if GNRC_IPV6_NIB_CONF_REDIRECT
/**
 * @brief   Checks if the carrier of the TL2AO was a redirect message
 *
 * @param[in] icmpv6    An ICMPv6 header.
 * @param[in] tl2ao     A TL2AO.
 *
 * @return  result of icmpv6_hdr_t::type == ICMPV6_REDIRECT for @p icmp and
 *          ndp_opt_t::type == NDP_OPT_TL2A for @p tl2ao.
 */
static inline bool _redirect_with_tl2ao(icmpv6_hdr_t *icmpv6, ndp_opt_t *tl2ao);
#else   /* GNRC_IPV6_NIB_CONF_REDIRECT */
/* just fall through if redirect not handled */
#define _redirect_with_tl2ao(a, b)  (false)
#endif  /* GNRC_IPV6_NIB_CONF_REDIRECT */

static inline bool _oflag_set(const ndp_nbr_adv_t *nbr_adv);
static inline bool _sflag_set(const ndp_nbr_adv_t *nbr_adv);
static inline bool _rflag_set(const ndp_nbr_adv_t *nbr_adv);

/**
 * @brief   Checks if the information in the TL2AO would change the
 *          corresponding neighbor cache entry
 *
 * @param[in] nce               A neighbor cache entry.
 * @param[in] tl2ao             The TL2AO.
 * @param[in] netif             The interface the TL2AO came over.
 * @param[in] tl2ao_addr_len    Length of the L2 address in the TL2AO.
 *
 * @return  `true`, if the TL2AO changes the NCE.
 * @return  `false`, if the TL2AO does not change the NCE.
 */
static inline bool _tl2ao_changes_nce(_nib_onl_entry_t *nce,
                                      const ndp_opt_t *tl2ao,
                                      gnrc_netif_t *netif,
                                      unsigned tl2ao_addr_len);

void _handle_snd_ns(_nib_onl_entry_t *nbr)
{
    const uint16_t state = _get_nud_state(nbr);

    DEBUG("nib: Retransmit neighbor solicitation\n");
    switch (state) {
        case GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE:
            if (nbr->ns_sent >= NDP_MAX_MC_SOL_NUMOF) {
                _nib_nc_remove(nbr);
                return;
            }
            _probe_nbr(nbr, false);
            break;
        case GNRC_IPV6_NIB_NC_INFO_NUD_STATE_PROBE:
            if (nbr->ns_sent >= NDP_MAX_UC_SOL_NUMOF) {
                gnrc_netif_t *netif = gnrc_netif_get_by_pid(_nib_onl_get_if(nbr));

                _set_nud_state(netif, nbr,
                               GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE);
            }
            /* intentionally falls through */
        case GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE:
            _probe_nbr(nbr, false);
            break;
        default:
            break;
    }
}

void _handle_state_timeout(_nib_onl_entry_t *nbr)
{
    uint16_t new_state = GNRC_IPV6_NIB_NC_INFO_NUD_STATE_PROBE;

    switch (_get_nud_state(nbr)) {
        case GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE:
            DEBUG("nib: Timeout reachability\n");
            new_state = GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE;
            /* intentionally falls through */
        case GNRC_IPV6_NIB_NC_INFO_NUD_STATE_DELAY: {
            gnrc_netif_t *netif = gnrc_netif_get_by_pid(_nib_onl_get_if(nbr));

            _set_nud_state(netif, nbr, new_state);
            if (new_state == GNRC_IPV6_NIB_NC_INFO_NUD_STATE_PROBE) {
                DEBUG("nib: Timeout DELAY state\n");
                _probe_nbr(nbr, true);
            }
            break;
        }
    }
}

void _probe_nbr(_nib_onl_entry_t *nbr, bool reset)
{
    const uint16_t state = _get_nud_state(nbr);

    DEBUG("nib: Probing ");
    switch (state) {
        case GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED:
            DEBUG("UNMANAGED entry %s => skipping\n",
                  ipv6_addr_to_str(addr_str, &nbr->ipv6, sizeof(addr_str)));
            break;
        case GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE:
        case GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE: {
                gnrc_netif_t *netif = gnrc_netif_get_by_pid(_nib_onl_get_if(nbr));
                uint32_t next_ns = _evtimer_lookup(nbr,
                                                   GNRC_IPV6_NIB_SND_MC_NS);

                assert(netif != NULL);
                gnrc_netif_acquire(netif);
                if (next_ns > netif->ipv6.retrans_time) {
                    ipv6_addr_t sol_nodes;
                    uint32_t retrans_time = netif->ipv6.retrans_time;

                    DEBUG("multicast to %s's solicited nodes ",
                          ipv6_addr_to_str(addr_str, &nbr->ipv6,
                                           sizeof(addr_str)));
                    if (reset) {
                        nbr->ns_sent = 0;
                    }
                    if (state == GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE) {
                        /* first 3 retransmissions in PROBE, assume 1 higher to
                         * not send after netif->ipv6.retrans_timer sec again,
                         * but the next backoff after that => subtract 2 */
                        retrans_time = _exp_backoff_retrans_timer(nbr->ns_sent - 2,
                                                                  retrans_time);
                    }
                    DEBUG("(retrans. timer = %ums)\n", (unsigned)retrans_time);
                    ipv6_addr_set_solicited_nodes(&sol_nodes, &nbr->ipv6);
                    _snd_ns(&nbr->ipv6, netif, NULL, &sol_nodes);
                    _evtimer_add(nbr, GNRC_IPV6_NIB_SND_MC_NS, &nbr->nud_timeout,
                                 retrans_time);
                    if (nbr->ns_sent < (NDP_MAX_NS_NUMOF + 2)) {
                        /* cap ns_sent at NDP_MAX_NS_NUMOF to prevent backoff
                         * overflow */
                        nbr->ns_sent++;
                    }
                }
#if ENABLE_DEBUG
                else {
                    DEBUG("multicast to %s's solicited nodes (skipping since there is already "
                          "a multicast NS within %ums)\n",
                          ipv6_addr_to_str(addr_str, &nbr->ipv6,
                                           sizeof(addr_str)),
                          (unsigned)netif->ipv6.retrans_time);
                }
#endif  /* ENABLE_DEBUG */
                gnrc_netif_release(netif);
            }
            break;
        case GNRC_IPV6_NIB_NC_INFO_NUD_STATE_PROBE:
        default:
            _snd_uc_ns(nbr, reset);
            break;
    }
}

void _handle_adv_l2(gnrc_netif_t *netif, _nib_onl_entry_t *nce,
                    const icmpv6_hdr_t *icmpv6, const ndp_opt_t *tl2ao)
{
    unsigned l2addr_len = 0;

    assert(nce != NULL);
    assert(netif != NULL);
    if (tl2ao != NULL) {
        l2addr_len = _get_l2addr_len(netif, tl2ao);
        if (l2addr_len == 0U) {
            DEBUG("nib: Unexpected TL2AO length. Ignoring TL2AO\n");
            return;
        }
    }
    if ((_get_nud_state(nce) == GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE) ||
        _oflag_set((ndp_nbr_adv_t *)icmpv6) ||
        _redirect_with_tl2ao(icmpv6, tl2ao) ||
        _tl2ao_changes_nce(nce, tl2ao, netif, l2addr_len)) {
        bool nce_was_incomplete =
            (_get_nud_state(nce) == GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE);
        if (tl2ao != NULL) {
            nce->l2addr_len = l2addr_len;
            memcpy(nce->l2addr, tl2ao + 1, l2addr_len);
        }
        else {
            nce->l2addr_len = 0;
        }
        if (_sflag_set((ndp_nbr_adv_t *)icmpv6)) {
            _set_reachable(netif, nce);
        }
        else if ((icmpv6->type != ICMPV6_NBR_ADV) ||
                 !_sflag_set((ndp_nbr_adv_t *)icmpv6) ||
                 (!nce_was_incomplete &&
                  _tl2ao_changes_nce(nce, tl2ao, netif, l2addr_len))) {
            DEBUG("nib: Set %s%%%u to STALE\n",
                  ipv6_addr_to_str(addr_str, &nce->ipv6, sizeof(addr_str)),
                  (unsigned)netif->pid);
            evtimer_del(&_nib_evtimer, &nce->nud_timeout.event);
            _set_nud_state(netif, nce, GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE);
        }
        if (_oflag_set((ndp_nbr_adv_t *)icmpv6) ||
            ((icmpv6->type == ICMPV6_NBR_ADV) && nce_was_incomplete)) {
            if (_rflag_set((ndp_nbr_adv_t *)icmpv6)) {
                nce->info |= GNRC_IPV6_NIB_NC_INFO_IS_ROUTER;
            }
            else {
                nce->info &= ~GNRC_IPV6_NIB_NC_INFO_IS_ROUTER;
            }
        }
#if GNRC_IPV6_NIB_CONF_QUEUE_PKT && MODULE_GNRC_IPV6
        /* send queued packets */
        gnrc_pktqueue_t *ptr;
        DEBUG("nib: Sending queued packets\n");
        while ((ptr = gnrc_pktqueue_remove_head(&nce->pktqueue)) != NULL) {
            if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_IPV6,
                                           GNRC_NETREG_DEMUX_CTX_ALL,
                                           ptr->pkt)) {
                DEBUG("nib: No receivers for packet\n");
                gnrc_pktbuf_release_error(ptr->pkt, EBADF);
            }
            ptr->pkt = NULL;
        }
#endif  /* GNRC_IPV6_NIB_CONF_QUEUE_PKT */
        if ((icmpv6->type == ICMPV6_NBR_ADV) &&
            !_sflag_set((ndp_nbr_adv_t *)icmpv6) &&
            (_get_nud_state(nce) == GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE) &&
            _tl2ao_changes_nce(nce, tl2ao, netif, l2addr_len)) {
            evtimer_del(&_nib_evtimer, &nce->nud_timeout.event);
            _set_nud_state(netif, nce, GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE);
        }
    }
    else if ((icmpv6->type == ICMPV6_NBR_ADV) &&
             (_get_nud_state(nce) != GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE) &&
             (_get_nud_state(nce) != GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED) &&
             _sflag_set((ndp_nbr_adv_t *)icmpv6) &&
             !_tl2ao_changes_nce(nce, tl2ao, netif, l2addr_len)) {
        _set_reachable(netif, nce);
    }
}

void _recalc_reach_time(gnrc_netif_ipv6_t *netif)
{
    const uint32_t half = (netif->reach_time_base >> 1);

    netif->reach_time = random_uint32_range(half,
                                            netif->reach_time_base + half);
    _evtimer_add(netif, GNRC_IPV6_NIB_RECALC_REACH_TIME,
                 &netif->recalc_reach_time,
                 GNRC_IPV6_NIB_CONF_REACH_TIME_RESET);
}

void _set_reachable(gnrc_netif_t *netif, _nib_onl_entry_t *nce)
{
    DEBUG("nib: Set %s%%%u to REACHABLE for %ums\n",
          ipv6_addr_to_str(addr_str, &nce->ipv6, sizeof(addr_str)),
          netif->pid, (unsigned)netif->ipv6.reach_time);
    _set_nud_state(netif, nce, GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE);
    _evtimer_add(nce, GNRC_IPV6_NIB_REACH_TIMEOUT, &nce->nud_timeout,
                 netif->ipv6.reach_time);
}

void _set_nud_state(gnrc_netif_t *netif, _nib_onl_entry_t *nce,
                    uint16_t state)
{
    nce->info &= ~GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK;
    nce->info |= state;

#if GNRC_IPV6_NIB_CONF_ROUTER
    gnrc_netif_acquire(netif);
    if (netif != NULL) {
        _call_route_info_cb(netif, GNRC_IPV6_NIB_ROUTE_INFO_TYPE_NSC,
                            &nce->ipv6, (void *)((intptr_t)state));
    }
    gnrc_netif_release(netif);
#else   /* GNRC_IPV6_NIB_CONF_ROUTER */
    (void)netif;
#endif  /* GNRC_IPV6_NIB_CONF_ROUTER */
}

bool _is_reachable(_nib_onl_entry_t *entry)
{
    switch (_get_nud_state(entry)) {
        case GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE:
        case GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE:
            return false;
        default:
            return true;
    }
}

/* internal functions */
static inline uint32_t _exp_backoff_retrans_timer(uint8_t ns_sent,
                                                  uint32_t retrans_timer)
{
    uint32_t tmp = random_uint32_range(NDP_MIN_RANDOM_FACTOR,
                                       NDP_MAX_RANDOM_FACTOR);

    /* backoff according to  https://tools.ietf.org/html/rfc7048 with
     * BACKOFF_MULTIPLE == 2 */
    tmp = ((1 << ns_sent) * retrans_timer * tmp) / US_PER_MS;
    /* random factors were statically multiplied with 1000 ^ */
    if (tmp > NDP_MAX_RETRANS_TIMER_MS) {
        tmp = NDP_MAX_RETRANS_TIMER_MS;
    }
    return tmp;
}

#if GNRC_IPV6_NIB_CONF_REDIRECT
static inline bool _redirect_with_tl2ao(icmpv6_hdr_t *icmpv6, ndp_opt_t *tl2ao)
{
    return (icmpv6->type == ICMPV6_REDIRECT) && (tl2ao != NULL);
}
#endif  /* GNRC_IPV6_NIB_CONF_REDIRECT */

static inline bool _tl2ao_changes_nce(_nib_onl_entry_t *nce,
                                      const ndp_opt_t *tl2ao,
                                      gnrc_netif_t *netif,
                                      unsigned tl2ao_addr_len)
{
    return ((tl2ao != NULL) &&
            (((nce->l2addr_len != tl2ao_addr_len) &&
              (memcmp(nce->l2addr, tl2ao + 1, tl2ao_addr_len) != 0)) ||
             (_nib_onl_get_if(nce) != (unsigned)netif->pid)));
}

static inline bool _oflag_set(const ndp_nbr_adv_t *nbr_adv)
{
    return (nbr_adv->type == ICMPV6_NBR_ADV) &&
           (nbr_adv->flags & NDP_NBR_ADV_FLAGS_O);
}

static inline bool _sflag_set(const ndp_nbr_adv_t *nbr_adv)
{
    return (nbr_adv->type == ICMPV6_NBR_ADV) &&
           (nbr_adv->flags & NDP_NBR_ADV_FLAGS_S);
}

static inline bool _rflag_set(const ndp_nbr_adv_t *nbr_adv)
{
    return (nbr_adv->type == ICMPV6_NBR_ADV) &&
           (nbr_adv->flags & NDP_NBR_ADV_FLAGS_R);
}
#endif /* GNRC_IPV6_NIB_CONF_ARSM */

/** @} */
