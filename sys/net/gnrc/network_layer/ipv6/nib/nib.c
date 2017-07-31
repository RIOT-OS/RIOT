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
#include "net/gnrc/netif2/internal.h"
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
static void _handle_nbr_sol(gnrc_netif2_t *netif, const ipv6_hdr_t *ipv6,
                            const ndp_nbr_sol_t *nbr_sol, size_t icmpv6_len);
static void _handle_nbr_adv(gnrc_netif2_t *netif, const ipv6_hdr_t *ipv6,
                            const ndp_nbr_adv_t *nbr_adv, size_t icmpv6_len);

static bool _resolve_addr(const ipv6_addr_t *dst, gnrc_netif2_t *netif,
                          gnrc_pktsnip_t *pkt, gnrc_ipv6_nib_nc_t *nce,
                          _nib_onl_entry_t *entry);

static void _handle_snd_na(gnrc_pktsnip_t *pkt);
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

void gnrc_ipv6_nib_init_iface(gnrc_netif2_t *netif)
{
    ipv6_addr_t addr = IPV6_ADDR_UNSPECIFIED;

    assert(netif != NULL);
    DEBUG("nib: Initialize interface %u\n", netif->pid);
    gnrc_netif2_acquire(netif);
    /* TODO:
     * - set link-local address here for stateless address auto-configuration
     *   and 6LN
     * - join solicited nodes group of link-local address here for address
     *   resolution here
     * - join all router group of link-local address here on router node here
     * - become an router advertising interface here on non-6LR here */

    _init_iface_arsm(netif);
    netif->ipv6.retrans_time = NDP_RETRANS_TIMER_MS;
    netif->ipv6.na_sent = 0;
#if GNRC_IPV6_NIB_CONF_ROUTER
    netif->ipv6.rtr_ltime = 1800U;
    netif->ipv6.last_ra = UINT32_MAX;
    netif->ipv6.ra_sent = 0;
    netif->flags |= GNRC_NETIF2_FLAGS_IPV6_FORWARDING;
#if !GNRC_IPV6_NIB_CONF_6LR || GNRC_IPV6_NIB_CONF_6LBR
    netif->flags |= GNRC_NETIF2_FLAGS_IPV6_RTR_ADV;
#endif
#if GNRC_IPV6_NIB_CONF_6LBR
    netif->flags |= GNRC_NETIF2_FLAGS_6LO_ABR;
#endif
    memcpy(&addr, &ipv6_addr_all_routers_link_local, sizeof(addr));
    if (gnrc_netif2_ipv6_group_join(netif, &addr) < 0) {
        LOG_ERROR("nib: Can't join link-local all-routers on interface %u\n",
                  netif->pid);
        return;
    }
#endif
#if GNRC_IPV6_NIB_CONF_6LN
    netif->ipv6.rs_sent = 0;
#endif
    memcpy(&addr, &ipv6_addr_all_nodes_link_local, sizeof(addr));
    if (gnrc_netif2_ipv6_group_join(netif, &addr) < 0) {
        LOG_ERROR("nib: Can't join link-local all-nodes on interface %u\n",
                  netif->pid);
        return;
    }
#if GNRC_IPV6_NIB_CONF_6LN || GNRC_IPV6_NIB_CONF_SLAAC
#if GNRC_IPV6_NIB_CONF_6LN
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
#endif
    uint8_t flags = GNRC_NETIF2_IPV6_ADDRS_FLAGS_STATE_VALID;
    /* TODO: set TENTATIVE as soon as there is a SLAAC implementation if not
     * 6LN ;-) */

    gnrc_netif2_ipv6_get_iid(netif, (eui64_t *)&addr.u64[1]);
    ipv6_addr_set_link_local_prefix(&addr);
    if (gnrc_netif2_ipv6_addr_add(netif, &addr, 64U, flags) < 0) {
        LOG_ERROR("nib: Can't add link-local address on interface %u\n",
                  netif->pid);
        return;
    }
#if GNRC_IPV6_NIB_CONF_ARSM
    /* TODO: SHOULD delay join between 0 and MAX_RTR_SOLICITATION_DELAY */
    ipv6_addr_set_solicited_nodes(&addr, &addr);
    if (gnrc_netif2_ipv6_group_join(netif, &addr) < 0) {
        LOG_ERROR("nib: Can't join solicited-nodes of link-local address on "
                  "interface %u\n", netif->pid);
        return;
    }
#endif
#if GNRC_IPV6_NIB_CONF_SLAAC
    /* TODO send NS to solicited nodes and wait netif->ipv6.retrans_time to
     * confirm uniqueness of the link-local address */
#endif
#endif
    gnrc_netif2_release(netif);
}

int gnrc_ipv6_nib_get_next_hop_l2addr(const ipv6_addr_t *dst,
                                      gnrc_netif2_t *netif, gnrc_pktsnip_t *pkt,
                                      gnrc_ipv6_nib_nc_t *nce)
{
    int res = 0;

    DEBUG("nib: get next hop link-layer address of %s%%%u\n",
          ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)),
          (netif != NULL) ? (unsigned)netif->pid : 0U);
    gnrc_netif2_acquire(netif);
    mutex_lock(&_nib_mutex);
    do {    /* XXX: hidden goto ;-) */
        if (ipv6_addr_is_link_local(dst)) {
            /* TODO: Prefix-based on-link determination */
            if ((netif == NULL) ||
                !_resolve_addr(dst, netif, pkt, nce,
                               _nib_onl_get(dst, netif->pid))) {
                res = -EHOSTUNREACH;
                break;
            }
        }
        else {
            /* TODO: Off-link next hop determination;
             *       might need netif locking */
            res = -EHOSTUNREACH;
        }
    } while (0);
    mutex_unlock(&_nib_mutex);
    gnrc_netif2_release(netif);
    return res;
}

void gnrc_ipv6_nib_handle_pkt(gnrc_netif2_t *netif, const ipv6_hdr_t *ipv6,
                              const icmpv6_hdr_t *icmpv6, size_t icmpv6_len)
{
    DEBUG("nib: Handle packet (icmpv6->type = %u)\n", icmpv6->type);
    assert(netif != NULL);
    gnrc_netif2_acquire(netif);
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
    gnrc_netif2_release(netif);
}

void gnrc_ipv6_nib_handle_timer_event(void *ctx, uint16_t type)
{
    DEBUG("nib: Handle timer event (ctx = %p, type = 0x%04x, now = %ums)\n",
          ctx, type, (unsigned)xtimer_now_usec() / 1000);
    mutex_lock(&_nib_mutex);
    switch (type) {
        /* TODO: remember netif locking if ctx is a gnrc_netif2_t */
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

#if GNRC_IPV6_NIB_CONF_ROUTER
void gnrc_ipv6_nib_change_rtr_adv_iface(gnrc_netif2_t *netif, bool enable)
{
    if (enable) {
        netif->flags |= GNRC_NETIF2_FLAGS_IPV6_RTR_ADV;
        /* TODO: start router advertisements */
    }
    else {
        netif->flags &= ~GNRC_NETIF2_FLAGS_IPV6_RTR_ADV;
        /* TODO:
         *  - start final router advertisements,
         *  - start router solicitations? */
    }
}
#endif

/* Iterator for NDP options in a packet */
#define FOREACH_OPT(ndp_pkt, opt, icmpv6_len) \
    for (opt = (ndp_opt_t *)(ndp_pkt + 1); \
         icmpv6_len > 0; \
         icmpv6_len -= (opt->len << 3), \
         opt = (ndp_opt_t *)(((uint8_t *)opt) + (opt->len << 3)))

static inline size_t _get_l2src(const gnrc_netif2_t *netif, uint8_t *l2src)
{
#if GNRC_NETIF2_L2ADDR_MAXLEN > 0
    memcpy(l2src, netif->l2addr, netif->l2addr_len);
    return netif->l2addr_len;
#else
    (void)netif;
    (void)l2src;
    return 0;
#endif
}

static void _send_delayed_nbr_adv(const gnrc_netif2_t *netif,
                                  const ipv6_addr_t *tgt,
                                  const ipv6_addr_t *dst,
                                  gnrc_pktsnip_t *reply_aro)
{
    gnrc_pktsnip_t *nbr_adv, *extra_opts = reply_aro;
    _nib_onl_entry_t *nce;
    uint8_t reply_flags = NDP_NBR_ADV_FLAGS_S;

#if GNRC_IPV6_NIB_CONF_ROUTER
    if (gnrc_netif2_is_rtr(netif)) {
        reply_flags |= NDP_NBR_ADV_FLAGS_R;
    }
#endif
#if GNRC_NETIF2_L2ADDR_MAXLEN > 0
    if (ipv6_addr_is_multicast(dst)) {
        uint8_t l2addr[GNRC_NETIF2_L2ADDR_MAXLEN];
        size_t l2addr_len = _get_l2src(netif, l2addr);

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
#else /* GNRC_NETIF2_L2ADDR_MAXLEN > 0 */
    reply_flags |= NDP_NBR_ADV_FLAGS_O;
#endif
    /* discard const qualifier */
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

static void _handle_nbr_sol(gnrc_netif2_t *netif, const ipv6_hdr_t *ipv6,
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
    tgt_idx = gnrc_netif2_ipv6_addr_idx(netif, &nbr_sol->tgt);
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
    /* TODO SLAAC behavior */
#endif  /* GNRC_IPV6_NIB_CONF_SLAAC */
    if (!ipv6_addr_is_unspecified(&ipv6->src)) {
        gnrc_pktsnip_t *reply_aro = NULL;
#if GNRC_IPV6_NIB_CONF_6LR
        ndp_opt_t *sl2ao = NULL;
        sixlowpan_nd_opt_ar_t *aro = NULL;
#else   /* GNRC_IPV6_NIB_CONF_6LR */
#define sl2ao   (NULL)
#define aro     (NULL)
#endif  /* GNRC_IPV6_NIB_CONF_6LR */
        tmp_len = icmpv6_len - sizeof(ndp_nbr_sol_t);

        if (!(netif->flags & GNRC_NETIF2_FLAGS_HAS_L2ADDR)) {
            /* Set STALE NCE if link-layer has no addresses */
            _nib_nc_add(&ipv6->src, netif->pid,
                        GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE);
        }
        FOREACH_OPT(nbr_sol, opt, tmp_len) {
            switch (opt->type) {
                case NDP_OPT_SL2A:
#if GNRC_IPV6_NIB_CONF_6LR
                    if (gnrc_netif2_is_6lr(netif)) {
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
            }
        }
        reply_aro = _copy_and_handle_aro(netif, ipv6, nbr_sol, aro, sl2ao);
        /* check if target address is anycast */
        if (netif->ipv6.addrs_flags[tgt_idx] & GNRC_NETIF2_IPV6_ADDRS_FLAGS_ANYCAST) {
            _send_delayed_nbr_adv(netif, &nbr_sol->tgt, &ipv6->dst, reply_aro);
        }
        else {
            gnrc_ndp2_nbr_adv_send(&nbr_sol->tgt, netif, &ipv6->src,
                                   ipv6_addr_is_multicast(&ipv6->dst),
                                   reply_aro);
        }
    }
}

static void _handle_nbr_adv(gnrc_netif2_t *netif, const ipv6_hdr_t *ipv6,
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
    if (((nce = _nib_onl_get(&nbr_adv->tgt, netif->pid)) != NULL) &&
        (nce->mode & _NC)) {
#if GNRC_IPV6_NIB_CONF_ARSM
        bool tl2ao_avail = false;
#endif

        tmp_len = icmpv6_len - sizeof(ndp_nbr_adv_t);
        FOREACH_OPT(nbr_adv, opt, tmp_len) {
            switch (opt->type) {
#if GNRC_IPV6_NIB_CONF_ARSM
                case NDP_OPT_TL2A:
                    _handle_adv_l2(netif, nce, (icmpv6_hdr_t *)nbr_adv, opt);
                    tl2ao_avail = true;
                    break;
#endif
#if GNRC_IPV6_NIB_CONF_6LN
                case NDP_OPT_AR:
                    _handle_aro(netif, ipv6, (const icmpv6_hdr_t *)nbr_adv,
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
            _set_reachable(netif, nce);
        }
        if (!(netif->flags & GNRC_NETIF2_FLAGS_HAS_L2ADDR)) {
            _handle_adv_l2(netif, nce, (icmpv6_hdr_t *)nbr_adv, NULL);
        }
#endif
    }
}

#if GNRC_IPV6_NIB_CONF_ARSM
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
#endif

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

static bool _resolve_addr(const ipv6_addr_t *dst, gnrc_netif2_t *netif,
                          gnrc_pktsnip_t *pkt, gnrc_ipv6_nib_nc_t *nce,
                          _nib_onl_entry_t *entry)
{
    bool res = false;
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
#else
    if (entry != NULL) {
        DEBUG("nib: resolve address %s%%%u from neighbor cache\n",
              ipv6_addr_to_str(addr_str, &entry->ipv6, sizeof(addr_str)),
              _nib_onl_get_if(entry));
        _nib_nc_get(entry, nce);
        res = true;
    }
#endif
    else if (!(res = _resolve_addr_from_ipv6(dst, netif, nce))) {
#if GNRC_IPV6_NIB_CONF_ARSM
        bool reset = false;
#endif

        DEBUG("nib: resolve address %s by probing neighbors\n",
              ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)));
        if ((entry == NULL) || !(entry->mode & _NC)) {
            entry = _nib_nc_add(dst, (netif != NULL) ? netif->pid : 0,
                                GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE);
            if (entry == NULL) {
                return false;
            }
#if GNRC_IPV6_NIB_CONF_ROUTER
            if ((netif != NULL) && (netif->ipv6.route_info_cb != NULL)) {
                netif->ipv6.route_info_cb(GNRC_IPV6_NIB_ROUTE_INFO_TYPE_NSC,
                      dst, (void *)GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE);
            }
#endif
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
