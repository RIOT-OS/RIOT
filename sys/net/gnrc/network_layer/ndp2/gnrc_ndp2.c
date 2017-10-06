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

#include "net/gnrc/icmpv6.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netif.h"
#ifdef MODULE_GNRC_SIXLOWPAN_ND
#include "net/gnrc/sixlowpan/nd.h"
#endif
#include "net/ndp.h"

#include "net/gnrc/ndp2.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

gnrc_pktsnip_t *gnrc_ndp2_nbr_sol_build(const ipv6_addr_t *tgt,
                                        gnrc_pktsnip_t *options)
{
    gnrc_pktsnip_t *pkt;

    assert((tgt != NULL) && !ipv6_addr_is_multicast(tgt));
    DEBUG("ndp2: building neighbor solicitation message\n");
    pkt = gnrc_icmpv6_build(options, ICMPV6_NBR_SOL, 0, sizeof(ndp_nbr_sol_t));
    if (pkt != NULL) {
        ndp_nbr_sol_t *nbr_sol = pkt->data;
        nbr_sol->resv.u32 = 0;
        nbr_sol->tgt.u64[0].u64 = tgt->u64[0].u64;
        nbr_sol->tgt.u64[1].u64 = tgt->u64[1].u64;
    }
#if ENABLE_DEBUG
    else {
        DEBUG("ndp2: NS not created due to no space in packet buffer\n");
    }
#endif
    return pkt;
}

gnrc_pktsnip_t *gnrc_ndp2_nbr_adv_build(const ipv6_addr_t *tgt, uint8_t flags,
                                        gnrc_pktsnip_t *options)
{
    gnrc_pktsnip_t *pkt;

    assert((tgt != NULL) && !ipv6_addr_is_multicast(tgt));
    DEBUG("ndp2: building neighbor advertisement message\n");
    pkt = gnrc_icmpv6_build(options, ICMPV6_NBR_ADV, 0, sizeof(ndp_nbr_adv_t));
    if (pkt != NULL) {
        ndp_nbr_adv_t *nbr_adv = pkt->data;
        nbr_adv->flags = (flags & NDP_NBR_ADV_FLAGS_MASK);
        nbr_adv->resv[0] = nbr_adv->resv[1] = nbr_adv->resv[2] = 0;
        nbr_adv->tgt.u64[0].u64 = tgt->u64[0].u64;
        nbr_adv->tgt.u64[1].u64 = tgt->u64[1].u64;
    }
#if ENABLE_DEBUG
    else {
        DEBUG("ndp2: NA not created due to no space in packet buffer\n");
    }
#endif
    return pkt;
}

gnrc_pktsnip_t *gnrc_ndp2_rtr_sol_build(gnrc_pktsnip_t *options)
{
    gnrc_pktsnip_t *pkt;

    DEBUG("ndp2: building router solicitation message\n");
    pkt = gnrc_icmpv6_build(options, ICMPV6_RTR_SOL, 0, sizeof(ndp_rtr_sol_t));
    if (pkt != NULL) {
        ndp_rtr_sol_t *rtr_sol = pkt->data;
        rtr_sol->resv.u32 = 0;
    }
#if ENABLE_DEBUG
    else {
        DEBUG("ndp2: RS not created due to no space in packet buffer\n");
    }
#endif
    return pkt;
}

gnrc_pktsnip_t *gnrc_ndp2_rtr_adv_build(uint8_t cur_hl, uint8_t flags,
                                        uint16_t ltime, uint32_t reach_time,
                                        uint32_t retrans_timer,
                                        gnrc_pktsnip_t *options)
{
    gnrc_pktsnip_t *pkt;

    DEBUG("ndp2: building router advertisement message\n");
    pkt = gnrc_icmpv6_build(options, ICMPV6_RTR_ADV, 0, sizeof(ndp_rtr_adv_t));
    if (pkt != NULL) {
        ndp_rtr_adv_t *rtr_adv = pkt->data;
        rtr_adv->cur_hl = cur_hl;
        rtr_adv->flags = (flags & NDP_RTR_ADV_FLAGS_MASK);
        rtr_adv->ltime = byteorder_htons(ltime);
        rtr_adv->reach_time = byteorder_htonl(reach_time);
        rtr_adv->retrans_timer = byteorder_htonl(retrans_timer);
    }
#if ENABLE_DEBUG
    else {
        DEBUG("ndp2: RA not created due to no space in packet buffer\n");
    }
#endif
    return pkt;
}

static inline size_t _ceil8(uint8_t length)
{
    /* NDP options use units of 8 byte for their length field, so round up */
    return (length + 7U) & 0xf8U;
}

gnrc_pktsnip_t *gnrc_ndp2_opt_build(uint8_t type, size_t size,
                                    gnrc_pktsnip_t *next)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(next, NULL, _ceil8(size),
                                          GNRC_NETTYPE_UNDEF);

    if (pkt != NULL) {
        ndp_opt_t *opt = pkt->data;
        opt->type = type;
        opt->len = (uint8_t)(pkt->size / 8);
    }
#if ENABLE_DEBUG
    else {
        DEBUG("ndp2: option not created due to no space in packet buffer\n");
    }
#endif
    return pkt;
}

static inline gnrc_pktsnip_t *_opt_l2a_build(const uint8_t *l2addr,
                                             uint8_t l2addr_len,
                                             gnrc_pktsnip_t *next,
                                             uint8_t type)
{
    gnrc_pktsnip_t *pkt = gnrc_ndp2_opt_build(type,
                                              sizeof(ndp_opt_t) + l2addr_len,
                                              next);

    if (pkt != NULL) {
        ndp_opt_t *l2a_opt = pkt->data;

        memset(l2a_opt + 1, 0, pkt->size - sizeof(ndp_opt_t));
        memcpy(l2a_opt + 1, l2addr, l2addr_len);
    }
    return pkt;
}

gnrc_pktsnip_t *gnrc_ndp2_opt_sl2a_build(const uint8_t *l2addr,
                                         uint8_t l2addr_len,
                                         gnrc_pktsnip_t *next)
{
    assert((l2addr != NULL) && (l2addr_len != 0));
    DEBUG("ndp2: building source link-layer address option (l2addr: %s)\n",
          gnrc_netif_addr_to_str(addr_str, sizeof(addr_str), l2addr,
                                 l2addr_len));
    return _opt_l2a_build(l2addr, l2addr_len, next, NDP_OPT_SL2A);
}

gnrc_pktsnip_t *gnrc_ndp2_opt_tl2a_build(const uint8_t *l2addr,
                                         uint8_t l2addr_len,
                                         gnrc_pktsnip_t *next)
{
    assert((l2addr != NULL) && (l2addr_len != 0));
    DEBUG("ndp2: building target link-layer address option (l2addr: %s)\n",
          gnrc_netif_addr_to_str(addr_str, sizeof(addr_str), l2addr,
                                 l2addr_len));
    return _opt_l2a_build(l2addr, l2addr_len, next, NDP_OPT_TL2A);
}

gnrc_pktsnip_t *gnrc_ndp2_opt_pi_build(const ipv6_addr_t *prefix,
                                       uint8_t prefix_len,
                                       uint32_t valid_ltime, uint32_t pref_ltime,
                                       uint8_t flags, gnrc_pktsnip_t *next)
{
    assert(prefix != NULL);
    assert(!ipv6_addr_is_link_local(prefix) && !ipv6_addr_is_multicast(prefix));
    assert(prefix_len <= 128);
    gnrc_pktsnip_t *pkt = gnrc_ndp2_opt_build(NDP_OPT_PI, sizeof(ndp_opt_pi_t),
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

gnrc_pktsnip_t *gnrc_ndp2_opt_mtu_build(uint32_t mtu, gnrc_pktsnip_t *next)
{
    gnrc_pktsnip_t *pkt = gnrc_ndp2_opt_build(NDP_OPT_MTU,
                                              sizeof(ndp_opt_mtu_t), next);

    if (pkt != NULL) {
        ndp_opt_mtu_t *mtu_opt = pkt->data;

        mtu_opt->resv.u16 = 0;
        mtu_opt->mtu = byteorder_htonl(mtu);
    }
    return pkt;
}

static gnrc_pktsnip_t *_build_headers(gnrc_ipv6_netif_t *netif,
                                      const ipv6_addr_t *src,
                                      const ipv6_addr_t *dst,
                                      gnrc_pktsnip_t *payload);
static size_t _get_l2src(gnrc_ipv6_netif_t *netif, uint8_t *l2src,
                         size_t l2src_maxlen);

void gnrc_ndp2_nbr_sol_send(const ipv6_addr_t *tgt, gnrc_ipv6_netif_t *netif,
                            const ipv6_addr_t *src, const ipv6_addr_t *dst,
                            gnrc_pktsnip_t *ext_opts)
{
    assert((tgt != NULL) && !ipv6_addr_is_multicast(tgt));
    assert((netif != NULL) && (dst != NULL));
    gnrc_pktsnip_t *hdr, *pkt = ext_opts;
    /* cppcheck-suppress variableScope
     * (reason: also used in MODULE_GNRC_SIXLOWPAN_ND compile path) */
    uint8_t l2src[8];
    /* cppcheck-suppress variableScope
     * (reason: also used in MODULE_GNRC_SIXLOWPAN_ND compile path) */
    size_t l2src_len = 0;

    DEBUG("ndp2: send neighbor solicitation (iface: %" PRIkernel_pid ", "
          "src: %s, ", netif->pid,
          ipv6_addr_to_str(addr_str, (src != NULL) ? src : &ipv6_addr_unspecified,
                           sizeof(addr_str)));
    DEBUG("tgt: %s, ", ipv6_addr_to_str(addr_str, tgt, sizeof(addr_str)));
    DEBUG("dst: %s)\n", ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)));
    /* check if there is a fitting source address to target */
    if (src == NULL) {
        src = gnrc_ipv6_netif_find_best_src_addr(netif->pid, tgt, false);
    }

    /* add SL2AO based on interface and source address */
    if ((src != NULL) && !ipv6_addr_is_unspecified(src)) {
        l2src_len = _get_l2src(netif, l2src, sizeof(l2src));

        if (l2src_len > 0) {
            /* add source address link-layer address option */
            hdr = gnrc_ndp2_opt_sl2a_build(l2src, l2src_len, pkt);

            if (hdr == NULL) {
                DEBUG("ndp2: error allocating SL2AO.\n");
                gnrc_pktbuf_release(pkt);
                return;
            }
            pkt = hdr;
        }
    }
    /* add neighbor solicitation header */
    hdr = gnrc_ndp2_nbr_sol_build(tgt, pkt);
    if (hdr == NULL) {
        DEBUG("ndp2: error allocating neighbor solicitation.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    pkt = hdr;
    /* add remaining headers */
    hdr = _build_headers(netif, src, dst, pkt);
    if (hdr == NULL) {
        DEBUG("ndp2: error adding lower-layer headers.\n");
        gnrc_pktbuf_release(pkt);
    }
    else if (gnrc_netapi_dispatch_send(GNRC_NETTYPE_NDP2,
                                       GNRC_NETREG_DEMUX_CTX_ALL, hdr) == 0) {
        DEBUG("ndp2: unable to send neighbor solicitation\n");
        gnrc_pktbuf_release(hdr);
    }
}

void gnrc_ndp2_nbr_adv_send(const ipv6_addr_t *tgt, gnrc_ipv6_netif_t *netif,
                            const ipv6_addr_t *dst, bool supply_tl2a,
                            gnrc_pktsnip_t *ext_opts)
{
    ipv6_addr_t real_dst;
    gnrc_pktsnip_t *hdr, *pkt = ext_opts;
    uint8_t adv_flags = 0;

    assert((tgt != NULL) && !ipv6_addr_is_multicast(tgt));
    assert((netif != NULL) && (dst != NULL));
    DEBUG("ndp2: send neighbor advertisement (iface: %" PRIkernel_pid
          ", tgt: %s, ", netif->pid,
          ipv6_addr_to_str(addr_str, tgt, sizeof(addr_str)));
    DEBUG("dst: %s, supply_tl2a: %d)\n",
          ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)), supply_tl2a);
    if ((netif->flags & GNRC_IPV6_NETIF_FLAGS_ROUTER) &&
        (netif->flags & GNRC_IPV6_NETIF_FLAGS_RTR_ADV)) {
        adv_flags |= NDP_NBR_ADV_FLAGS_R;
    }
    if (ipv6_addr_is_unspecified(dst)) {
        memcpy(&real_dst, &ipv6_addr_all_nodes_link_local, sizeof(ipv6_addr_t));
        supply_tl2a = true;
    }
    else {
        memcpy(&real_dst, dst, sizeof(real_dst));
        adv_flags |= NDP_NBR_ADV_FLAGS_S;
    }
    /* add SL2AO based on target address */
    if (supply_tl2a) {
        uint8_t l2tgt[8];
        size_t l2tgt_len;
        /* we previously checked if we are the target, so we can take our L2tgt */
        l2tgt_len = _get_l2src(netif, l2tgt, sizeof(l2tgt));

        if (l2tgt_len > 0) {
            /* add target address link-layer address option */
            hdr = gnrc_ndp2_opt_tl2a_build(l2tgt, l2tgt_len, pkt);

            if (hdr == NULL) {
                DEBUG("ndp2: error allocating TL2AO.\n");
                gnrc_pktbuf_release(ext_opts);
                return;
            }
            pkt = hdr;
        }
    }
    /* TODO: also check if the node provides proxy services for tgt */
    if ((pkt != NULL) &&
        (!gnrc_ipv6_netif_addr_is_non_unicast(tgt) || supply_tl2a)) {
        /* TL2A is not supplied and tgt is not anycast */
        adv_flags |= NDP_NBR_ADV_FLAGS_O;
    }
    /* add neighbor advertisement header */
    hdr = gnrc_ndp2_nbr_adv_build(tgt, adv_flags, pkt);
    if (hdr == NULL) {
        DEBUG("ndp2: error allocating neighbor advertisement.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    pkt = hdr;
    /* add remaining headers */
    hdr = _build_headers(netif, NULL, &real_dst, pkt);
    if (hdr == NULL) {
        DEBUG("ndp2: error adding lower-layer headers.\n");
        gnrc_pktbuf_release(pkt);
    }
    else if (gnrc_netapi_dispatch_send(GNRC_NETTYPE_NDP2,
                                       GNRC_NETREG_DEMUX_CTX_ALL, hdr) == 0) {
        DEBUG("ndp2: unable to send neighbor advertisement\n");
        gnrc_pktbuf_release(hdr);
    }
}

void gnrc_ndp2_rtr_sol_send(gnrc_ipv6_netif_t *netif, const ipv6_addr_t *dst)
{
    gnrc_pktsnip_t *hdr, *pkt = NULL;
    ipv6_addr_t *src = NULL;

    assert(netif != NULL);
    if (dst == NULL) {
        dst = &ipv6_addr_all_routers_link_local;
    }
    DEBUG("ndp2: send router solicitation (iface: %" PRIkernel_pid
          ", dst: %s)\n", netif->pid,
          ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)));
    /* add SL2AO => check if there is a fitting source address to target */
    if ((src = gnrc_ipv6_netif_find_best_src_addr(netif->pid, dst,
                                                  false)) != NULL) {
        uint8_t l2src[8];
        size_t l2src_len = _get_l2src(netif, l2src, sizeof(l2src));
        if (l2src_len > 0) {
            /* add source address link-layer address option */
            pkt = gnrc_ndp2_opt_sl2a_build(l2src, l2src_len, NULL);
            if (pkt == NULL) {
                DEBUG("ndp2: error allocating SL2AO.\n");
                gnrc_pktbuf_release(pkt);
                return;
            }
        }
    }
    /* add router solicitation header */
    hdr = gnrc_ndp2_rtr_sol_build(pkt);
    if (hdr == NULL) {
        DEBUG("ndp2: error allocating router solicitation.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    pkt = hdr;
    /* add remaining headers */
    hdr = _build_headers(netif, src, dst, pkt);
    if (hdr == NULL) {
        DEBUG("ndp2: error adding lower-layer headers.\n");
        gnrc_pktbuf_release(pkt);
    }
    else if (gnrc_netapi_dispatch_send(GNRC_NETTYPE_NDP2,
                                       GNRC_NETREG_DEMUX_CTX_ALL, hdr) == 0) {
        DEBUG("ndp2: unable to send router advertisement\n");
        gnrc_pktbuf_release(hdr);
    }
}

void gnrc_ndp2_rtr_adv_send(gnrc_ipv6_netif_t *netif, const ipv6_addr_t *src,
                            const ipv6_addr_t *dst, bool fin,
                            gnrc_pktsnip_t *ext_opts)
{
#if GNRC_IPV6_NIB_CONF_ROUTER
    gnrc_pktsnip_t *hdr = NULL, *pkt = ext_opts;
    uint32_t reach_time = 0, retrans_timer = 0;
    uint16_t adv_ltime = 0;
    uint8_t cur_hl = 0;

    if (dst == NULL) {
        dst = &ipv6_addr_all_nodes_link_local;
    }
    DEBUG("ndp2: send router advertisement (iface: %" PRIkernel_pid ", dst: %s%s\n",
          netif->pid, ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)),
          fin ? ", final" : "");
    if (netif->flags & GNRC_IPV6_NETIF_FLAGS_ADV_MTU) {
        if ((hdr = gnrc_ndp2_opt_mtu_build(netif->mtu, pkt)) == NULL) {
            DEBUG("ndp rtr: no space left in packet buffer\n");
            return;
        }
        pkt = hdr;
    }
    if (src == NULL) {
        /* get address from source selection algorithm.
         * Only link local addresses may be used (RFC 4861 section 4.1) */
        src = gnrc_ipv6_netif_find_best_src_addr(netif->pid, dst, true);
    }
    /* add SL2A for source address */
    if (src != NULL) {
        DEBUG(" - SL2A\n");
        uint8_t l2src[8];
        size_t l2src_len;
        /* optimization note: MAY also be omitted to facilitate in-bound load balancing over
         * replicated interfaces.
         * source: https://tools.ietf.org/html/rfc4861#section-6.2.3 */
        l2src_len = _get_l2src(netif, l2src, sizeof(l2src));
        if (l2src_len > 0) {
            /* add source address link-layer address option */
            hdr = gnrc_ndp2_opt_sl2a_build(l2src, l2src_len, pkt);

            if (hdr == NULL) {
                DEBUG("ndp2: error allocating Source Link-layer address option.\n");
                gnrc_pktbuf_release(pkt);
                return;
            }
            pkt = hdr;
        }
    }
    if (netif->flags & GNRC_IPV6_NETIF_FLAGS_ADV_CUR_HL) {
        cur_hl = netif->cur_hl;
    }
    if (netif->flags & GNRC_IPV6_NETIF_FLAGS_ADV_REACH_TIME) {

        if (netif->reach_time > (3600 * US_PER_SEC)) { /* reach_time > 1 hour */
            reach_time = (3600 * MS_PER_SEC);
        }
        else {
            reach_time = netif->reach_time / US_PER_MS;
        }
    }
    if (netif->flags & GNRC_IPV6_NETIF_FLAGS_ADV_RETRANS_TIMER) {
        retrans_timer = netif->retrans_timer / US_PER_MS;
    }
    if (!fin) {
        /* TODO set netif dependent adv_ltime */
        adv_ltime = 1800U;
    }
    hdr = gnrc_ndp2_rtr_adv_build(cur_hl,
                                  (netif->flags & (GNRC_IPV6_NETIF_FLAGS_OTHER_CONF |
                                                        GNRC_IPV6_NETIF_FLAGS_MANAGED)) >> 8,
                                  adv_ltime, reach_time, retrans_timer, pkt);
    if (hdr == NULL) {
        DEBUG("ndp2: error allocating router advertisement.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    pkt = hdr;
    hdr = _build_headers(netif, src, dst, pkt);
    if (hdr == NULL) {
        DEBUG("ndp2: error adding lower-layer headers.\n");
        gnrc_pktbuf_release(pkt);
    }
    else if (gnrc_netapi_dispatch_send(GNRC_NETTYPE_NDP2,
                                       GNRC_NETREG_DEMUX_CTX_ALL, hdr) == 0) {
        DEBUG("ndp2: unable to send router solicitation\n");
        gnrc_pktbuf_release(hdr);
    }
#else
    (void)netif;
    (void)src;
    (void)dst;
    (void)fin;
    DEBUG("ndp2: not a router, dropping ext_opts\n");
    gnrc_pktbuf_release(ext_opts);
#endif  /* GNRC_IPV6_NIB_CONF_ROUTER */
}

static gnrc_pktsnip_t *_build_headers(gnrc_ipv6_netif_t *netif,
                                      const ipv6_addr_t *src,
                                      const ipv6_addr_t *dst,
                                      gnrc_pktsnip_t *payload)
{
    gnrc_pktsnip_t *l2hdr;
    gnrc_pktsnip_t *iphdr = gnrc_ipv6_hdr_build(payload, src, dst);

    if (iphdr == NULL) {
        DEBUG("ndp2: error allocating IPv6 header.\n");
        return NULL;
    }
    ((ipv6_hdr_t *)iphdr->data)->hl = 255;
    /* add netif header for send interface specification */
    l2hdr = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    if (l2hdr == NULL) {
        DEBUG("ndp2: error allocating netif header.\n");
        gnrc_pktbuf_remove_snip(iphdr, iphdr);
        return NULL;
    }
    ((gnrc_netif_hdr_t *)l2hdr->data)->if_pid = netif->pid;
    LL_PREPEND(iphdr, l2hdr);
    return l2hdr;
}

static size_t _get_l2src(gnrc_ipv6_netif_t *netif, uint8_t *l2src,
                         size_t l2src_maxlen)
{
    bool try_long = false;
    int res;
    uint16_t l2src_len;
    /* maximum address length that fits into a minimum length (8) S/TL2A option */
    const uint16_t max_short_len = 6;

    /* try getting source address */
    if ((gnrc_netapi_get(netif->pid, NETOPT_SRC_LEN, 0, &l2src_len,
                         sizeof(l2src_len)) >= 0) &&
        (l2src_len > max_short_len)) {
        try_long = true;
    }

    if (try_long && ((res = gnrc_netapi_get(netif->pid, NETOPT_ADDRESS_LONG, 0,
                                            l2src,
                                            l2src_maxlen)) > max_short_len)) {
        l2src_len = (uint16_t)res;
    }
    else if ((res = gnrc_netapi_get(netif->pid, NETOPT_ADDRESS, 0, l2src,
                                    l2src_maxlen)) >= 0) {
        l2src_len = (uint16_t)res;
    }
    else {
        DEBUG("ndp2: no link-layer address found.\n");
        l2src_len = 0;
    }

    return l2src_len;
}

/** @} */
