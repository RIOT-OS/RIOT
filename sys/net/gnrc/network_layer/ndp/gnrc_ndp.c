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

#include <assert.h>
#include <string.h>
#include <inttypes.h>

#include "net/gnrc/icmpv6.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netif/internal.h"
#ifdef MODULE_GNRC_SIXLOWPAN_ND
#include "net/gnrc/sixlowpan/nd.h"
#endif
#include "net/ndp.h"

#include "net/gnrc/ndp.h"

#include "timex.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static char addr_str[IPV6_ADDR_MAX_STR_LEN];

gnrc_pktsnip_t *gnrc_ndp_nbr_sol_build(const ipv6_addr_t *tgt,
                                       gnrc_pktsnip_t *options)
{
    gnrc_pktsnip_t *pkt;

    assert((tgt != NULL) && !ipv6_addr_is_multicast(tgt));
    DEBUG("ndp: building neighbor solicitation message\n");
    pkt = gnrc_icmpv6_build(options, ICMPV6_NBR_SOL, 0, sizeof(ndp_nbr_sol_t));
    if (pkt != NULL) {
        ndp_nbr_sol_t *nbr_sol = pkt->data;
        nbr_sol->resv.u32 = 0;
        nbr_sol->tgt.u64[0].u64 = tgt->u64[0].u64;
        nbr_sol->tgt.u64[1].u64 = tgt->u64[1].u64;
    }
    else {
        DEBUG("ndp: NS not created due to no space in packet buffer\n");
    }
    return pkt;
}

gnrc_pktsnip_t *gnrc_ndp_nbr_adv_build(const ipv6_addr_t *tgt, uint8_t flags,
                                       gnrc_pktsnip_t *options)
{
    gnrc_pktsnip_t *pkt;

    assert((tgt != NULL) && !ipv6_addr_is_multicast(tgt));
    DEBUG("ndp: building neighbor advertisement message\n");
    pkt = gnrc_icmpv6_build(options, ICMPV6_NBR_ADV, 0, sizeof(ndp_nbr_adv_t));
    if (pkt != NULL) {
        ndp_nbr_adv_t *nbr_adv = pkt->data;
        nbr_adv->flags = (flags & NDP_NBR_ADV_FLAGS_MASK);
        nbr_adv->resv[0] = nbr_adv->resv[1] = nbr_adv->resv[2] = 0;
        nbr_adv->tgt.u64[0].u64 = tgt->u64[0].u64;
        nbr_adv->tgt.u64[1].u64 = tgt->u64[1].u64;
    }
    else {
        DEBUG("ndp: NA not created due to no space in packet buffer\n");
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
    else {
        DEBUG("ndp: RS not created due to no space in packet buffer\n");
    }
    return pkt;
}

gnrc_pktsnip_t *gnrc_ndp_rtr_adv_build(uint8_t cur_hl, uint8_t flags,
                                       uint16_t ltime, uint32_t reach_time,
                                       uint32_t retrans_timer,
                                       gnrc_pktsnip_t *options)
{
    gnrc_pktsnip_t *pkt;

    DEBUG("ndp: building router advertisement message\n");
    pkt = gnrc_icmpv6_build(options, ICMPV6_RTR_ADV, 0, sizeof(ndp_rtr_adv_t));
    if (pkt != NULL) {
        ndp_rtr_adv_t *rtr_adv = pkt->data;
        rtr_adv->cur_hl = cur_hl;
        rtr_adv->flags = (flags & NDP_RTR_ADV_FLAGS_MASK);
        rtr_adv->ltime = byteorder_htons(ltime);
        rtr_adv->reach_time = byteorder_htonl(reach_time);
        rtr_adv->retrans_timer = byteorder_htonl(retrans_timer);
    }
    else {
        DEBUG("ndp: RA not created due to no space in packet buffer\n");
    }
    return pkt;
}

static inline size_t _ceil8(uint8_t length)
{
    /* NDP options use units of 8 byte for their length field, so round up */
    return (length + 7U) & 0xf8U;
}

gnrc_pktsnip_t *gnrc_ndp_opt_build(uint8_t type, size_t size,
                                   gnrc_pktsnip_t *next)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(next, NULL, _ceil8(size),
                                          GNRC_NETTYPE_UNDEF);

    if (pkt != NULL) {
        ndp_opt_t *opt = pkt->data;
        opt->type = type;
        opt->len = (uint8_t)(pkt->size / 8);
    }
    else {
        DEBUG("ndp: option not created due to no space in packet buffer\n");
    }
    return pkt;
}

static inline gnrc_pktsnip_t *_opt_l2a_build(const uint8_t *l2addr,
                                             uint8_t l2addr_len,
                                             gnrc_pktsnip_t *next,
                                             uint8_t type)
{
    gnrc_pktsnip_t *pkt = gnrc_ndp_opt_build(type,
                                             sizeof(ndp_opt_t) + l2addr_len,
                                             next);

    if (pkt != NULL) {
        ndp_opt_t *l2a_opt = pkt->data;

        memset(l2a_opt + 1, 0, pkt->size - sizeof(ndp_opt_t));
        memcpy(l2a_opt + 1, l2addr, l2addr_len);
    }
    return pkt;
}

gnrc_pktsnip_t *gnrc_ndp_opt_sl2a_build(const uint8_t *l2addr,
                                        uint8_t l2addr_len,
                                        gnrc_pktsnip_t *next)
{
    assert((l2addr != NULL) && (l2addr_len != 0));
    DEBUG("ndp: building source link-layer address option (l2addr: %s)\n",
          gnrc_netif_addr_to_str(l2addr, l2addr_len, addr_str));
    return _opt_l2a_build(l2addr, l2addr_len, next, NDP_OPT_SL2A);
}

gnrc_pktsnip_t *gnrc_ndp_opt_tl2a_build(const uint8_t *l2addr,
                                        uint8_t l2addr_len,
                                        gnrc_pktsnip_t *next)
{
    assert((l2addr != NULL) && (l2addr_len != 0));
    DEBUG("ndp: building target link-layer address option (l2addr: %s)\n",
          gnrc_netif_addr_to_str(l2addr, l2addr_len, addr_str));
    return _opt_l2a_build(l2addr, l2addr_len, next, NDP_OPT_TL2A);
}

gnrc_pktsnip_t *gnrc_ndp_opt_pi_build(const ipv6_addr_t *prefix,
                                      uint8_t prefix_len,
                                      uint32_t valid_ltime, uint32_t pref_ltime,
                                      uint8_t flags, gnrc_pktsnip_t *next)
{
    assert(prefix != NULL);
    assert(!ipv6_addr_is_link_local(prefix) && !ipv6_addr_is_multicast(prefix));
    assert(prefix_len <= 128);
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
    gnrc_pktsnip_t *pkt = gnrc_ndp_opt_build(NDP_OPT_MTU,
                                             sizeof(ndp_opt_mtu_t), next);

    if (pkt != NULL) {
        ndp_opt_mtu_t *mtu_opt = pkt->data;

        mtu_opt->resv.u16 = 0;
        mtu_opt->mtu = byteorder_htonl(mtu);
    }
    return pkt;
}

gnrc_pktsnip_t *gnrc_ndp_opt_rdnss_build(uint32_t ltime, ipv6_addr_t *addrs,
                                         unsigned addrs_num,
                                         gnrc_pktsnip_t *next)
{
    assert(addrs != NULL);
    assert(addrs_num > 0);
    size_t opt_size = sizeof(ndp_opt_t) + (sizeof(ipv6_addr_t) * addrs_num);
    gnrc_pktsnip_t *pkt = gnrc_ndp_opt_build(NDP_OPT_RDNSS, opt_size, next);

    if (pkt != NULL) {
        ndp_opt_rdnss_impl_t *rdnss_opt = pkt->data;
        rdnss_opt->resv.u16 = 0;
        rdnss_opt->ltime = byteorder_htonl(ltime);
        for (unsigned i = 0; i < addrs_num; i++) {
            memcpy(&rdnss_opt->addrs[i], &addrs[i],
                   sizeof(rdnss_opt->addrs[i]));
        }
    }
    return pkt;
}

static gnrc_pktsnip_t *_build_headers(gnrc_netif_t *netif,
                                      const ipv6_addr_t *src,
                                      const ipv6_addr_t *dst,
                                      gnrc_pktsnip_t *payload);
static inline size_t _get_l2src(const gnrc_netif_t *netif, uint8_t *l2src);

void gnrc_ndp_nbr_sol_send(const ipv6_addr_t *tgt, gnrc_netif_t *netif,
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

    DEBUG("ndp: send neighbor solicitation (iface: %" PRIkernel_pid ", "
          "src: %s, ", netif->pid,
          ipv6_addr_to_str(addr_str, (src != NULL) ? src : &ipv6_addr_unspecified,
                           sizeof(addr_str)));
    DEBUG("tgt: %s, ", ipv6_addr_to_str(addr_str, tgt, sizeof(addr_str)));
    DEBUG("dst: %s)\n", ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)));
    gnrc_netif_acquire(netif);
    do {    /* XXX hidden goto */
        /* check if there is a fitting source address to target */
        if (src == NULL) {
            src = gnrc_netif_ipv6_addr_best_src(netif, tgt, false);
        }

        /* add SL2AO based on interface and source address */
        if ((src != NULL) && !ipv6_addr_is_unspecified(src)) {
            l2src_len = _get_l2src(netif, l2src);

            if (l2src_len > 0) {
                /* add source address link-layer address option */
                hdr = gnrc_ndp_opt_sl2a_build(l2src, l2src_len, pkt);

                if (hdr == NULL) {
                    DEBUG("ndp: error allocating SL2AO.\n");
                    break;
                }
                pkt = hdr;
            }
        }
        /* add neighbor solicitation header */
        hdr = gnrc_ndp_nbr_sol_build(tgt, pkt);
        if (hdr == NULL) {
            DEBUG("ndp: error allocating neighbor solicitation.\n");
            break;
        }
        pkt = hdr;
        /* add remaining headers */
        hdr = _build_headers(netif, src, dst, pkt);
        if (hdr == NULL) {
            DEBUG("ndp: error adding lower-layer headers.\n");
            break;
        }
        else {
            pkt = hdr;
            if (gnrc_netapi_dispatch_send(GNRC_NETTYPE_NDP,
                                          GNRC_NETREG_DEMUX_CTX_ALL,
                                          pkt) == 0) {
                DEBUG("ndp: unable to send neighbor solicitation\n");
                break;
            }
        }
        gnrc_netif_release(netif);
        return;
    } while (0);
    gnrc_pktbuf_release(pkt);
    gnrc_netif_release(netif);
}

void gnrc_ndp_nbr_adv_send(const ipv6_addr_t *tgt, gnrc_netif_t *netif,
                           const ipv6_addr_t *dst, bool supply_tl2a,
                           gnrc_pktsnip_t *ext_opts)
{
    ipv6_addr_t real_dst;
    gnrc_pktsnip_t *hdr, *pkt = ext_opts;
    uint8_t adv_flags = 0;

    assert((tgt != NULL) && !ipv6_addr_is_multicast(tgt));
    assert((netif != NULL) && (dst != NULL));
    DEBUG("ndp: send neighbor advertisement (iface: %" PRIkernel_pid
          ", tgt: %s, ", netif->pid,
          ipv6_addr_to_str(addr_str, tgt, sizeof(addr_str)));
    DEBUG("dst: %s, supply_tl2a: %d)\n",
          ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)), supply_tl2a);
    gnrc_netif_acquire(netif);
    do {    /* XXX: hidden goto */
        int tgt_idx;
        gnrc_netif_t *tgt_netif = gnrc_netif_get_by_ipv6_addr(tgt);

        if (tgt_netif == NULL) {
            DEBUG("ndp: tgt not assigned to interface. Abort sending\n");
            break;
        }

        tgt_idx = gnrc_netif_ipv6_addr_idx(tgt_netif, tgt);
        assert(tgt_idx >= 0);

        if (gnrc_netif_is_rtr(netif) && gnrc_netif_is_rtr_adv(netif)) {
            adv_flags |= NDP_NBR_ADV_FLAGS_R;
        }
        if (ipv6_addr_is_unspecified(dst)) {
            memcpy(&real_dst, &ipv6_addr_all_nodes_link_local,
                   sizeof(ipv6_addr_t));
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
            l2tgt_len = _get_l2src(netif, l2tgt);

            if (l2tgt_len > 0) {
                /* add target address link-layer address option */
                hdr = gnrc_ndp_opt_tl2a_build(l2tgt, l2tgt_len, pkt);

                if (hdr == NULL) {
                    DEBUG("ndp: error allocating TL2AO.\n");
                    break;
                }
                pkt = hdr;
            }
        }
        /* TODO: also check if the node provides proxy services for tgt */
        if ((pkt != NULL) &&
            (tgt_netif->ipv6.addrs_flags[tgt_idx] &
             GNRC_NETIF_IPV6_ADDRS_FLAGS_ANYCAST)) {
            /* TL2A is not supplied and tgt is not anycast */
            adv_flags |= NDP_NBR_ADV_FLAGS_O;
        }
        /* add neighbor advertisement header */
        hdr = gnrc_ndp_nbr_adv_build(tgt, adv_flags, pkt);
        if (hdr == NULL) {
            DEBUG("ndp: error allocating neighbor advertisement.\n");
            break;
        }
        pkt = hdr;
        /* add remaining headers */
        hdr = _build_headers(netif, NULL, &real_dst, pkt);
        if (hdr == NULL) {
            DEBUG("ndp: error adding lower-layer headers.\n");
            break;
        }
        else {
            pkt = hdr;
            if (gnrc_netapi_dispatch_send(GNRC_NETTYPE_NDP,
                                                       GNRC_NETREG_DEMUX_CTX_ALL,
                                                       pkt) == 0) {
                DEBUG("ndp: unable to send neighbor advertisement\n");
                break;
            }
        }
        gnrc_netif_release(netif);
        return;
    } while (0);
    gnrc_pktbuf_release(pkt);
    gnrc_netif_release(netif);
}

void gnrc_ndp_rtr_sol_send(gnrc_netif_t *netif, const ipv6_addr_t *dst)
{
    gnrc_pktsnip_t *hdr, *pkt = NULL;

    assert(netif != NULL);
    if (dst == NULL) {
        dst = &ipv6_addr_all_routers_link_local;
    }
    DEBUG("ndp: send router solicitation (iface: %" PRIkernel_pid
          ", dst: %s)\n", netif->pid,
          ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)));
    gnrc_netif_acquire(netif);
    do {    /* XXX: hidden goto */
        ipv6_addr_t *src = NULL;

        /* add SL2AO => check if there is a fitting source address to target */
        if ((src = gnrc_netif_ipv6_addr_best_src(netif, dst, false)) != NULL) {
            uint8_t l2src[8];
            size_t l2src_len = _get_l2src(netif, l2src);
            if (l2src_len > 0) {
                /* add source address link-layer address option */
                pkt = gnrc_ndp_opt_sl2a_build(l2src, l2src_len, NULL);
                if (pkt == NULL) {
                    DEBUG("ndp: error allocating SL2AO.\n");
                    break;
                }
            }
        }
        /* add router solicitation header */
        hdr = gnrc_ndp_rtr_sol_build(pkt);
        if (hdr == NULL) {
            DEBUG("ndp: error allocating router solicitation.\n");
            break;
        }
        pkt = hdr;
        /* add remaining headers */
        hdr = _build_headers(netif, src, dst, pkt);
        if (hdr == NULL) {
            DEBUG("ndp: error adding lower-layer headers.\n");
            break;
        }
        else {
            pkt = hdr;
            if (gnrc_netapi_dispatch_send(GNRC_NETTYPE_NDP,
                                          GNRC_NETREG_DEMUX_CTX_ALL,
                                          pkt) == 0) {
                DEBUG("ndp: unable to send router advertisement\n");
                break;
            }
        }
        gnrc_netif_release(netif);
        return;
    } while (0);
    gnrc_pktbuf_release(pkt);
    gnrc_netif_release(netif);
}

void gnrc_ndp_rtr_adv_send(gnrc_netif_t *netif, const ipv6_addr_t *src,
                           const ipv6_addr_t *dst, bool fin,
                           gnrc_pktsnip_t *ext_opts)
{
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER)
    gnrc_pktsnip_t *hdr = NULL, *pkt = ext_opts;
    uint32_t reach_time = 0, retrans_timer = 0;
    uint16_t adv_ltime = 0;
    uint8_t cur_hl = 0;
    uint8_t flags = 0;

    if (dst == NULL) {
        dst = &ipv6_addr_all_nodes_link_local;
    }
    DEBUG("ndp: send router advertisement (iface: %" PRIkernel_pid ", dst: %s%s\n",
          netif->pid, ipv6_addr_to_str(addr_str, dst, sizeof(addr_str)),
          fin ? ", final" : "");
    gnrc_netif_acquire(netif);
    do {    /* XXX: hidden goto */
        if (netif->flags & GNRC_NETIF_FLAGS_IPV6_ADV_MTU) {
            if ((hdr = gnrc_ndp_opt_mtu_build(netif->ipv6.mtu, pkt)) == NULL) {
                DEBUG("ndp rtr: no space left in packet buffer\n");
                break;
            }
            pkt = hdr;
        }
        if (src == NULL) {
            /* get address from source selection algorithm.
             * Only link local addresses may be used (RFC 4861 section 4.1) */
            src = gnrc_netif_ipv6_addr_best_src(netif, dst, true);

            if (src == NULL) {
                DEBUG("ndp rtr: no VALID source address found for RA\n");
                break;
            }
        }
        /* add SL2A for source address */
        if (src != NULL) {
            DEBUG(" - SL2A\n");
            uint8_t l2src[8];
            size_t l2src_len;
            /* optimization note: MAY also be omitted to facilitate in-bound load balancing over
             * replicated interfaces.
             * source: https://tools.ietf.org/html/rfc4861#section-6.2.3 */
            l2src_len = _get_l2src(netif, l2src);
            if (l2src_len > 0) {
                /* add source address link-layer address option */
                hdr = gnrc_ndp_opt_sl2a_build(l2src, l2src_len, pkt);

                if (hdr == NULL) {
                    DEBUG("ndp: error allocating Source Link-layer address "
                          "option.\n");
                    break;
                }
                pkt = hdr;
            }
        }
        if (netif->flags & GNRC_NETIF_FLAGS_IPV6_ADV_CUR_HL) {
            cur_hl = netif->cur_hl;
        }
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ARSM)
        /* netif->ipv6.reach_time_base is only available with Address Resolution
         * State Machine */
        if (netif->flags & GNRC_NETIF_FLAGS_IPV6_ADV_REACH_TIME) {
            if (netif->ipv6.reach_time_base > (3600 * MS_PER_SEC)) {
                /* reach_time > 1 hour */
                reach_time = (3600 * MS_PER_SEC);
            }
            else {
                reach_time = netif->ipv6.reach_time_base;
            }
        }
#endif /* CONFIG_GNRC_IPV6_NIB_ARSM */
        if (netif->flags & GNRC_NETIF_FLAGS_IPV6_ADV_RETRANS_TIMER) {
            retrans_timer = netif->ipv6.retrans_time;
        }
        if (!fin) {
            adv_ltime = netif->ipv6.rtr_ltime;
        }
        if (netif->ipv6.aac_mode & GNRC_NETIF_AAC_DHCP) {
            flags |= NDP_RTR_ADV_FLAGS_M;
            if (netif->flags & GNRC_NETIF_FLAGS_IPV6_ADV_O_FLAG) {
                flags |= NDP_RTR_ADV_FLAGS_O;
            }
        }
        hdr = gnrc_ndp_rtr_adv_build(cur_hl, flags, adv_ltime, reach_time,
                                     retrans_timer, pkt);
        if (hdr == NULL) {
            DEBUG("ndp: error allocating router advertisement.\n");
            break;
        }
        pkt = hdr;
        hdr = _build_headers(netif, src, dst, pkt);
        if (hdr == NULL) {
            DEBUG("ndp: error adding lower-layer headers.\n");
            break;
        }
        else {
            pkt = hdr;
            if (gnrc_netapi_dispatch_send(GNRC_NETTYPE_NDP,
                                          GNRC_NETREG_DEMUX_CTX_ALL,
                                          pkt) == 0) {
                DEBUG("ndp: unable to send router solicitation\n");
                break;
            }
        }
        gnrc_netif_release(netif);
        return;
    } while (0);
    gnrc_pktbuf_release(pkt);
    gnrc_netif_release(netif);
#else
    (void)netif;
    (void)src;
    (void)dst;
    (void)fin;
    DEBUG("ndp: not a router, dropping ext_opts\n");
    gnrc_pktbuf_release(ext_opts);
#endif  /* CONFIG_GNRC_IPV6_NIB_ROUTER */
}

static gnrc_pktsnip_t *_build_headers(gnrc_netif_t *netif,
                                      const ipv6_addr_t *src,
                                      const ipv6_addr_t *dst,
                                      gnrc_pktsnip_t *payload)
{
    gnrc_pktsnip_t *l2hdr;
    gnrc_pktsnip_t *iphdr = gnrc_ipv6_hdr_build(payload, src, dst);

    if (iphdr == NULL) {
        DEBUG("ndp: error allocating IPv6 header.\n");
        return NULL;
    }
    ((ipv6_hdr_t *)iphdr->data)->hl = NDP_HOP_LIMIT;
    /* add netif header for send interface specification */
    l2hdr = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    if (l2hdr == NULL) {
        DEBUG("ndp: error allocating netif header.\n");
        gnrc_pktbuf_remove_snip(iphdr, iphdr);
        return NULL;
    }
    gnrc_netif_hdr_set_netif(l2hdr->data, netif);
    return gnrc_pkt_prepend(iphdr, l2hdr);
}

static inline size_t _get_l2src(const gnrc_netif_t *netif, uint8_t *l2src)
{
#if GNRC_NETIF_L2ADDR_MAXLEN > 0
    memcpy(l2src, netif->l2addr, netif->l2addr_len);
    return netif->l2addr_len;
#else
    (void)netif;
    (void)l2src;
    return 0;
#endif
}

/** @} */
