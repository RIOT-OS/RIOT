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

#include "net/ipv6/addr.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/ipv6/netif.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ndp2.h"
#include "net/gnrc/pktqueue.h"
#include "net/gnrc/sixlowpan/nd.h"
#include "net/ndp.h"
#include "net/sixlowpan/nd.h"

#include "_nib-internal.h"
#include "_nib-arsm.h"
#include "_nib-6ln.h"
#include "_nib-6lr.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"
#if ENABLE_DEBUG
#include "xtimer.h"
#endif

#if ENABLE_DEBUG
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

#if GNRC_IPV6_NIB_CONF_QUEUE_PKT
static gnrc_pktqueue_t _queue_pool[GNRC_IPV6_NIB_NUMOF];
#endif

/**
 * @internal
 * @{
 */
static void _handle_nbr_sol(kernel_pid_t iface, const ipv6_hdr_t *ipv6,
                            const ndp_nbr_sol_t *nbr_sol, size_t icmpv6_len);
static void _handle_nbr_adv(kernel_pid_t iface, const ipv6_hdr_t *ipv6,
                            const ndp_nbr_adv_t *nbr_adv, size_t icmpv6_len);

static bool _resolve_addr(const ipv6_addr_t *dst, kernel_pid_t iface,
                          gnrc_pktsnip_t *pkt, gnrc_ipv6_nib_nc_t *nce,
                          _nib_onl_entry_t *entry);

static void _handle_snd_na(gnrc_pktsnip_t *pkt);

/* interface flag checks */
#if GNRC_IPV6_NIB_CONF_ROUTER
static inline bool _is_rtr(const gnrc_ipv6_netif_t *netif)
{
    return (netif->flags & GNRC_IPV6_NETIF_FLAGS_ROUTER);
}
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

void gnrc_ipv6_nib_init_iface(kernel_pid_t iface)
{
    _nib_iface_t *nib_iface;

    assert(iface > KERNEL_PID_UNDEF);
    DEBUG("nib: Initialize interface %u\n", (unsigned)iface);
    mutex_lock(&_nib_mutex);
    nib_iface = _nib_iface_get(iface);
#ifdef TEST_SUITES
    if (nib_iface == NULL) {
        /* in the unittests old NC and NIB are mixed, so this function leads to
         * crashes. To prevent this we early exit here, if the interface was
         * not found
         * TODO: remove when gnrc_ipv6_nc is removed.
         */
        mutex_unlock(&_nib_mutex);
        return;
    }
#else
    assert(nib_iface != NULL);
#endif
    /* TODO:
     * - set link-local address here for stateless address auto-configuration
     *   and 6LN
     * - join solicited nodes group of link-local address here for address
     *   resolution here
     * - join all router group of link-local address here on router node here
     * - become an router advertising interface here on non-6LR here */

    _init_iface_arsm(nib_iface);
    nib_iface->rs_sent = 0;
    nib_iface->na_sent = 0;
#if GNRC_IPV6_NIB_CONF_ROUTER
    nib_iface->last_ra = UINT32_MAX;
    nib_iface->ra_sent = 0;
#endif
    mutex_unlock(&_nib_mutex);
}

int gnrc_ipv6_nib_get_next_hop_l2addr(const ipv6_addr_t *dst,
                                      kernel_pid_t iface, gnrc_pktsnip_t *pkt,
                                      gnrc_ipv6_nib_nc_t *nce)
{
    int res = 0;

    mutex_lock(&_nib_mutex);
    do {    /* XXX: hidden goto ;-) */
        if (ipv6_addr_is_link_local(dst)) {
            /* TODO: Prefix-based on-link determination */
            if ((iface == KERNEL_PID_UNDEF) ||
                !_resolve_addr(dst, iface, pkt, nce,
                               _nib_onl_get(dst, iface))) {
                res = -EHOSTUNREACH;
                break;
            }
        }
        else {
            /* TODO: Off-link next hop determination */
            res = -EHOSTUNREACH;
        }
    } while (0);
    mutex_unlock(&_nib_mutex);
    return res;
}

void gnrc_ipv6_nib_handle_pkt(kernel_pid_t iface, const ipv6_hdr_t *ipv6,
                              const icmpv6_hdr_t *icmpv6, size_t icmpv6_len)
{
    DEBUG("nib: Handle packet (icmpv6->type = %u)\n", icmpv6->type);
    mutex_lock(&_nib_mutex);
    switch (icmpv6->type) {
#if GNRC_IPV6_NIB_CONF_ROUTER
        case ICMPV6_RTR_SOL:
            /* TODO */
            break;
#endif  /* GNRC_IPV6_NIB_CONF_ROUTER */
        case ICMPV6_RTR_ADV:
            /* TODO */
            break;
        case ICMPV6_NBR_SOL:
            _handle_nbr_sol(iface, ipv6, (ndp_nbr_sol_t *)icmpv6, icmpv6_len);
            break;
        case ICMPV6_NBR_ADV:
            _handle_nbr_adv(iface, ipv6, (ndp_nbr_adv_t *)icmpv6, icmpv6_len);
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
            _nib_iface_recalc_reach_time(ctx);
            break;
#endif  /* GNRC_IPV6_NIB_CONF_ARSM */
        case GNRC_IPV6_NIB_SND_NA:
            _handle_snd_na(ctx);
            break;
        case GNRC_IPV6_NIB_SEARCH_RTR:
            /* TODO */
            break;
        case GNRC_IPV6_NIB_RECONFIRM_RTR:
            /* TODO */
            break;
#if GNRC_IPV6_NIB_CONF_ROUTER
        case GNRC_IPV6_NIB_REPLY_RS:
            /* TODO */
            break;
        case GNRC_IPV6_NIB_SND_MC_RA:
            /* TODO */
            break;
#endif  /* GNRC_IPV6_NIB_CONF_ROUTER */
#if GNRC_IPV6_NIB_CONF_6LN
        case GNRC_IPV6_NIB_ADDR_REG_TIMEOUT:
            /* TODO */
            break;
        case GNRC_IPV6_NIB_6LO_CTX_TIMEOUT:
            /* TODO */
            break;
#endif  /* GNRC_IPV6_NIB_CONF_6LN */
#if GNRC_IPV6_NIB_CONF_MULTIHOP_P6C
        case GNRC_IPV6_NIB_ABR_TIMEOUT:
            /* TODO */
            break;
#endif  /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */
        case GNRC_IPV6_NIB_PFX_TIMEOUT:
            /* TODO */
            break;
        case GNRC_IPV6_NIB_RTR_TIMEOUT:
            /* TODO */
            break;
        default:
            break;
    }
    mutex_unlock(&_nib_mutex);
}

/* Iterator for NDP options in a packet */
#define FOREACH_OPT(ndp_pkt, opt, icmpv6_len) \
    for (opt = (ndp_opt_t *)(ndp_pkt + 1); \
         icmpv6_len > 0; \
         icmpv6_len -= (opt->len << 3), \
         opt = (ndp_opt_t *)(((uint8_t *)opt) + (opt->len << 3)))

static size_t _get_l2src(kernel_pid_t iface, uint8_t *l2src,
                         size_t l2src_maxlen)
{
    bool try_long = false;
    int res;
    uint16_t l2src_len;
    /* maximum address length that fits into a minimum length (8) S/TL2A
     * option */
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
        DEBUG("nib: No link-layer address found.\n");
        l2src_len = 0;
    }

    return l2src_len;
}

static void _send_delayed_nbr_adv(const gnrc_ipv6_netif_t *netif,
                                  const ipv6_addr_t *tgt,
                                  const ipv6_addr_t *dst,
                                  gnrc_pktsnip_t *reply_aro)
{
    gnrc_pktsnip_t *nbr_adv, *extra_opts = reply_aro;
    _nib_onl_entry_t *nce;
    uint8_t reply_flags = NDP_NBR_ADV_FLAGS_S;

#if GNRC_IPV6_NIB_CONF_ROUTER
    if (_is_rtr(netif)) {
        reply_flags |= NDP_NBR_ADV_FLAGS_R;
    }
#endif
    if (ipv6_addr_is_multicast(dst)) {
        uint8_t l2addr[GNRC_IPV6_NIB_L2ADDR_MAX_LEN];
        size_t l2addr_len = _get_l2src(netif->pid, l2addr, sizeof(l2addr));
        if (l2addr_len > 0) {
            extra_opts = gnrc_ndp2_opt_tl2a_build(l2addr, l2addr_len,
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
    nbr_adv = gnrc_ndp2_nbr_adv_build(tgt, reply_flags, extra_opts);
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
        _evtimer_add(nce, GNRC_IPV6_NIB_SND_NA,
                     &nce->snd_na,
                     random_uint32_range(0, NDP_MAX_ANYCAST_MS_DELAY));
    }
}

static void _handle_nbr_sol(kernel_pid_t iface, const ipv6_hdr_t *ipv6,
                            const ndp_nbr_sol_t *nbr_sol, size_t icmpv6_len)
{
    size_t tmp_len = icmpv6_len - sizeof(ndp_nbr_sol_t);
    ndp_opt_t *opt;
    ipv6_addr_t *local;

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
        DEBUG("     - ICMP length: %u (should > %u)\n", icmpv6_len,
              sizeof(ndp_nbr_sol_t));
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
    local = gnrc_ipv6_netif_find_addr(iface, &nbr_sol->tgt);
    if (local == NULL) {
        DEBUG("nib: Target address %s is not assigned to a local interface\n",
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
    /* TODO SLAAC behavior */
#endif  /* GNRC_IPV6_NIB_CONF_SLAAC */
    if (!ipv6_addr_is_unspecified(&ipv6->src)) {
#if GNRC_IPV6_NIB_CONF_6LR
        ndp_opt_t *sl2ao = NULL;
        sixlowpan_nd_opt_ar_t *aro = NULL;
#else   /* GNRC_IPV6_NIB_CONF_6LR */
#define sl2ao   (NULL)
#define aro     (NULL)
#endif  /* GNRC_IPV6_NIB_CONF_6LR */
        gnrc_ipv6_netif_t *netif;
        gnrc_pktsnip_t *reply_aro = NULL;
        tmp_len = icmpv6_len - sizeof(ndp_nbr_sol_t);

        netif = gnrc_ipv6_netif_get(iface);
        /* TODO: Set STALE NCE if link-layer has no addresses */
        FOREACH_OPT(nbr_sol, opt, tmp_len) {
            switch (opt->type) {
                case NDP_OPT_SL2A:
#if GNRC_IPV6_NIB_CONF_6LR
                    if (_is_6lr(netif)) {
                        DEBUG("nib: Storing SL2AO for later handling\n");
                        sl2ao = opt;
                        break;
                    }
#endif  /* GNRC_IPV6_NIB_CONF_6LR */
                    _handle_sl2ao(iface, ipv6, (const icmpv6_hdr_t *)nbr_sol,
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
            }
        }
        reply_aro = _copy_and_handle_aro(iface, ipv6, nbr_sol, aro, sl2ao);
        /* check if target address is anycast */
        if (gnrc_ipv6_netif_addr_is_non_unicast(local)) {
            _send_delayed_nbr_adv(netif, &nbr_sol->tgt, &ipv6->dst, reply_aro);
        }
        else {
            gnrc_ndp2_nbr_adv_send(&nbr_sol->tgt, netif, &ipv6->src,
                                   ipv6_addr_is_multicast(&ipv6->dst),
                                   reply_aro);
        }
    }
}

static void _handle_nbr_adv(kernel_pid_t iface, const ipv6_hdr_t *ipv6,
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
        DEBUG("     - ICMP length: %u (should > %u)\n", icmpv6_len,
              sizeof(ndp_nbr_adv_t));
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
    /* TODO SLAAC behavior */
#endif
    if (((nce = _nib_onl_get(&nbr_adv->tgt, iface)) != NULL) &&
        (nce->mode & _NC)) {
#if GNRC_IPV6_NIB_CONF_ARSM
        bool tl2ao_avail = false;
#endif

        tmp_len = icmpv6_len - sizeof(ndp_nbr_adv_t);
        FOREACH_OPT(nbr_adv, opt, tmp_len) {
            switch (opt->type) {
#if GNRC_IPV6_NIB_CONF_ARSM
                case NDP_OPT_TL2A:
                    _handle_adv_l2(iface, nce, (icmpv6_hdr_t *)nbr_adv, opt);
                    tl2ao_avail = true;
                    break;
#endif
#if GNRC_IPV6_NIB_CONF_6LN
                case NDP_OPT_AR:
                    _handle_aro(iface, ipv6, (const icmpv6_hdr_t *)nbr_adv,
                                (const sixlowpan_nd_opt_ar_t *)opt, opt, nce);
                    break;
#endif
                default:
                    DEBUG("nib: Ignoring unrecognized option type %u for NA\n",
                          opt->type);
            }
        }
#if GNRC_IPV6_NIB_CONF_ARSM
        if (!tl2ao_avail && (nbr_adv->flags & NDP_NBR_ADV_FLAGS_S) &&
            (_get_nud_state(nce) != GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE)) {
            /* reachability confirmed without TL2AO */
            _set_reachable(iface, nce);
        }
        /* TODO: handling for of advertised link-layer with link-layers without
         * addresses */
        /* _handle_adv_l2(iface, nce, (icmpv6_hdr_t *)nbr_adv, NULL); */
#endif
    }
}

static inline bool _is_reachable(_nib_onl_entry_t *entry)
{
    (void)entry; /* _get_nud_state() might just resolved to UNMANAGED as macro */
    switch (_get_nud_state(entry)) {
        case GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE:
        case GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE:
            return false;
        default:
            return true;
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
#endif

static bool _resolve_addr(const ipv6_addr_t *dst, kernel_pid_t iface,
                          gnrc_pktsnip_t *pkt, gnrc_ipv6_nib_nc_t *nce,
                          _nib_onl_entry_t *entry)
{
    bool res = false;
#if GNRC_IPV6_NIB_CONF_ARSM
    if ((entry != NULL) && (entry->mode & _NC) && _is_reachable(entry)) {
        if (_get_nud_state(entry) == GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE) {
            _set_nud_state(entry, GNRC_IPV6_NIB_NC_INFO_NUD_STATE_DELAY);
            _evtimer_add(entry, GNRC_IPV6_NIB_DELAY_TIMEOUT,
                         &entry->nud_timeout, NDP_DELAY_FIRST_PROBE_MS);
        }
        _nib_nc_get(entry, nce);
        res = true;
    }
#else
    if (entry != NULL) {
        _nib_nc_get(entry, nce);
        res = true;
    }
#endif
    else if (!(res = _resolve_addr_from_ipv6(dst, iface, nce))) {
#if GNRC_IPV6_NIB_CONF_ARSM
        bool reset = false;
#endif
        if ((entry == NULL) || !(entry->mode & _NC)) {
            entry = _nib_nc_add(dst, iface,
                                GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE);
            if (entry == NULL) {
                return false;
            }
#if GNRC_IPV6_NIB_CONF_ARSM
            reset = true;
#endif
        }
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
#endif
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
#else
    (void)pkt;
    DEBUG("nib: No IPv6 module to send delayed neighbor advertisement\n");
#endif
}

/** @} */
