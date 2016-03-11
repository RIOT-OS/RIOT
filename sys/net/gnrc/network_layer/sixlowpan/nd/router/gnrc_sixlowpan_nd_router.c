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

#include "net/gnrc/ipv6.h"
#include "net/gnrc/ndp.h"
#include "net/gnrc/sixlowpan/ctx.h"
#include "net/gnrc/sixlowpan/nd.h"
#include "net/icmpv6.h"
#include "net/ndp.h"
#include "net/sixlowpan/nd.h"

#include "net/gnrc/sixlowpan/nd/router.h"

static gnrc_sixlowpan_nd_router_abr_t _abrs[GNRC_SIXLOWPAN_ND_ROUTER_ABR_NUMOF];
static gnrc_sixlowpan_nd_router_prf_t _prefixes[GNRC_SIXLOWPAN_ND_ROUTER_ABR_PRF_NUMOF];

static gnrc_sixlowpan_nd_router_abr_t *_get_abr(ipv6_addr_t *addr)
{
    gnrc_sixlowpan_nd_router_abr_t *abr = NULL;

    for (int i = 0; i < GNRC_SIXLOWPAN_ND_ROUTER_ABR_NUMOF; i++) {
        if (ipv6_addr_equal(&_abrs[i].addr, addr)) {
            return &_abrs[i];
        }

        if ((abr == NULL) && ipv6_addr_is_unspecified(&_abrs[i].addr)) {
            abr = &_abrs[i];
        }
    }

    return abr;
}

static gnrc_sixlowpan_nd_router_prf_t *_get_free_prefix(gnrc_ipv6_netif_t *ipv6_iface,
                                                        ipv6_addr_t *prefix,
                                                        size_t prefix_len)
{
    gnrc_sixlowpan_nd_router_prf_t *prf = NULL;

    for (int i = 0; i < GNRC_SIXLOWPAN_ND_ROUTER_ABR_PRF_NUMOF; i++) {
        if ((ipv6_addr_match_prefix(&_prefixes[i].prefix->addr, prefix) >= prefix_len) &&
            (_prefixes[i].prefix->prefix_len == prefix_len) &&
            (_prefixes[i].iface == ipv6_iface)) {
            return &_prefixes[i];
        }

        if ((prf == NULL) &&
            ((_prefixes[i].prefix == NULL) ||
             ipv6_addr_is_unspecified(&_prefixes[i].prefix->addr))) {
            prf = &_prefixes[i];
        }
    }

    return prf;
}

static void _add_prefix(kernel_pid_t iface, gnrc_sixlowpan_nd_router_abr_t *abr,
                        ndp_opt_pi_t *pi_opt)
{
    gnrc_sixlowpan_nd_router_prf_t *prf_ent;
    gnrc_ipv6_netif_t *ipv6_iface = gnrc_ipv6_netif_get(iface);
    ipv6_addr_t *prefix;

    if ((pi_opt->len != NDP_OPT_PI_LEN) || ipv6_addr_is_link_local(&pi_opt->prefix) ||
        (pi_opt->flags & NDP_OPT_PI_FLAGS_A) ||
        (pi_opt->flags & NDP_OPT_PI_FLAGS_L) ||
        (pi_opt->valid_ltime.u32 == 0)) {
        return;
    }

    prefix = gnrc_ipv6_netif_match_prefix(iface, &pi_opt->prefix);

    prf_ent = _get_free_prefix(ipv6_iface, &pi_opt->prefix, pi_opt->prefix_len);

    if ((prf_ent != NULL) && (prf_ent->iface == NULL)) {
        prf_ent->iface = ipv6_iface;
        prf_ent->prefix = container_of(prefix, gnrc_ipv6_netif_addr_t, addr);
        LL_PREPEND(abr->prfs, prf_ent);
    }
}

static void _add_ctx(gnrc_sixlowpan_nd_router_abr_t *abr, sixlowpan_nd_opt_6ctx_t *ctx_opt)
{
    if (((ctx_opt->ctx_len < 64) && (ctx_opt->len != 2)) ||
        ((ctx_opt->ctx_len >= 64) && (ctx_opt->len != 3))) {
        return;
    }
    bf_set(abr->ctxs, sixlowpan_nd_opt_6ctx_get_cid(ctx_opt));
}

#ifdef MODULE_GNRC_SIXLOWPAN_ND_BORDER_ROUTER
static inline bool _is_me(ipv6_addr_t *addr)
{
    ipv6_addr_t *res;

    return (gnrc_ipv6_netif_find_by_addr(&res, addr) != KERNEL_PID_UNDEF);
}
#else
#define _is_me(ignore)  (false)
#endif

void gnrc_sixlowpan_nd_router_set_rtr_adv(gnrc_ipv6_netif_t *netif, bool enable)
{
    if (enable && (gnrc_ipv6_netif_add_addr(netif->pid, &ipv6_addr_all_routers_link_local, 128,
                                            GNRC_IPV6_NETIF_ADDR_FLAGS_NON_UNICAST) != NULL)) {
        mutex_lock(&netif->mutex);
        netif->flags |= GNRC_IPV6_NETIF_FLAGS_RTR_ADV;
        netif->adv_ltime = GNRC_IPV6_NETIF_DEFAULT_ROUTER_LTIME;
#ifdef MODULE_GNRC_NDP_ROUTER
        /* for border router these values have to be initialized, too */
        netif->max_adv_int = GNRC_IPV6_NETIF_DEFAULT_MAX_ADV_INT;
        netif->min_adv_int = GNRC_IPV6_NETIF_DEFAULT_MIN_ADV_INT;
#endif
        mutex_unlock(&netif->mutex);
    }
    else {
        netif->flags &= ~GNRC_IPV6_NETIF_FLAGS_RTR_ADV;
        gnrc_ipv6_netif_remove_addr(netif->pid, (ipv6_addr_t *)&ipv6_addr_all_routers_link_local);
    }
}

gnrc_sixlowpan_nd_router_abr_t *gnrc_sixlowpan_nd_router_abr_get(void)
{
    if (ipv6_addr_is_unspecified(&_abrs[0].addr)) {
        return NULL;
    }
    return _abrs;
}

bool gnrc_sixlowpan_nd_router_abr_older(sixlowpan_nd_opt_abr_t *abr_opt)
{
    gnrc_sixlowpan_nd_router_abr_t *abr;
    uint32_t version;

    if (abr_opt->len != SIXLOWPAN_ND_OPT_ABR_LEN) {
        /* invalid option received */
        return true;
    }

    if (_is_me(&abr_opt->braddr)) {
        return false;
    }

    abr = _get_abr(&abr_opt->braddr);

    if (abr == NULL) {
        return false;
    }

    version = (uint32_t)byteorder_ntohs(abr_opt->vlow);
    version |= ((uint32_t)byteorder_ntohs(abr_opt->vhigh)) << 16;

    return (version < abr->version);
}

void gnrc_sixlowpan_nd_router_abr_remove(gnrc_sixlowpan_nd_router_abr_t *abr)
{
    for (int i = 0; i < GNRC_SIXLOWPAN_CTX_SIZE; i++) {
        if (bf_isset(abr->ctxs, i)) {
            gnrc_sixlowpan_ctx_remove(i);
            bf_unset(abr->ctxs, i);
        }
    }

    while (abr->prfs != NULL) {
        gnrc_sixlowpan_nd_router_prf_t *prefix = abr->prfs;
        LL_DELETE(abr->prfs, prefix);
        gnrc_ipv6_netif_remove_addr(prefix->iface->pid, &prefix->prefix->addr);
        prefix->next = NULL;
        prefix->iface = NULL;
        prefix->prefix = NULL;
    }
    ipv6_addr_set_unspecified(&abr->addr);
    abr->version = 0;
}

/* router-only functions from net/gnrc/sixlowpan/nd.h */
void gnrc_sixlowpan_nd_opt_abr_handle(kernel_pid_t iface, ndp_rtr_adv_t *rtr_adv, int sicmpv6_size,
                                      sixlowpan_nd_opt_abr_t *abr_opt)
{
    uint16_t opt_offset = 0;
    uint8_t *buf = (uint8_t *)(rtr_adv + 1);
    gnrc_sixlowpan_nd_router_abr_t *abr;
    uint32_t t = 0;

    if (_is_me(&abr_opt->braddr)) {
        return;
    }
    /* validity and version was checked in previously called
     * gnrc_sixlowpan_nd_router_abr_older() */
    abr = _get_abr(&abr_opt->braddr);

    if (abr == NULL) {
        return;
    }

    abr->ltime = byteorder_ntohs(abr_opt->ltime);

    if (abr->ltime == 0) {
        abr->ltime = GNRC_SIXLOWPAN_ND_BORDER_ROUTER_DEFAULT_LTIME;
        return;
    }

    sicmpv6_size -= sizeof(ndp_rtr_adv_t);

    while (sicmpv6_size > 0) {
        ndp_opt_t *opt = (ndp_opt_t *)(buf + opt_offset);

        switch (opt->type) {
            case NDP_OPT_PI:
                _add_prefix(iface, abr, (ndp_opt_pi_t *)opt);

            case NDP_OPT_6CTX:
                _add_ctx(abr, (sixlowpan_nd_opt_6ctx_t *)opt);

            default:
                break;
        }

        opt_offset += (opt->len * 8);
        sicmpv6_size -= (opt->len * 8);
    }

    abr->version = (uint32_t)byteorder_ntohs(abr_opt->vlow);
    abr->version |= ((uint32_t)byteorder_ntohs(abr_opt->vhigh)) << 16;
    abr->addr.u64[0] = abr_opt->braddr.u64[0];
    abr->addr.u64[1] = abr_opt->braddr.u64[1];
    memset(abr->ctxs, 0, sizeof(abr->ctxs));
    abr->prfs = NULL;

    t = abr->ltime * 60 * SEC_IN_USEC;

    xtimer_remove(&abr->ltimer);
    abr->ltimer_msg.type = GNRC_SIXLOWPAN_ND_MSG_ABR_TIMEOUT;
    abr->ltimer_msg.content.ptr = (char *) abr;
    xtimer_set_msg(&abr->ltimer, t, &abr->ltimer_msg, gnrc_ipv6_pid);
}

gnrc_pktsnip_t *gnrc_sixlowpan_nd_opt_6ctx_build(uint8_t prefix_len, uint8_t flags, uint16_t ltime,
                                                 ipv6_addr_t *prefix, gnrc_pktsnip_t *next)
{
    gnrc_pktsnip_t *pkt = gnrc_ndp_opt_build(NDP_OPT_6CTX,
                                             sizeof(sixlowpan_nd_opt_6ctx_t) + (prefix_len / 8),
                                             next);

    if (pkt != NULL) {
        sixlowpan_nd_opt_6ctx_t *ctx_opt = pkt->data;
        ctx_opt->ctx_len = prefix_len;
        ctx_opt->resv_c_cid = flags;
        ctx_opt->resv.u16 = 0;
        ctx_opt->ltime = byteorder_htons(ltime);
        /* Bits beyond prefix_len MUST be 0 */
        memset(ctx_opt + 1, 0, pkt->size - sizeof(sixlowpan_nd_opt_6ctx_t));
        ipv6_addr_init_prefix((ipv6_addr_t *)(ctx_opt + 1), prefix, prefix_len);
    }

    return pkt;
}

gnrc_pktsnip_t *gnrc_sixlowpan_nd_opt_abr_build(uint32_t version, uint16_t ltime,
                                                ipv6_addr_t *braddr, gnrc_pktsnip_t *next)
{
    gnrc_pktsnip_t *pkt = gnrc_ndp_opt_build(NDP_OPT_ABR, sizeof(sixlowpan_nd_opt_abr_t), next);

    if (pkt != NULL) {
        sixlowpan_nd_opt_abr_t *abr_opt = pkt->data;
        abr_opt->vlow = byteorder_htons(version & 0xffff);
        abr_opt->vhigh = byteorder_htons(version >> 16);
        abr_opt->ltime = byteorder_htons(ltime);
        abr_opt->braddr.u64[0] = braddr->u64[0];
        abr_opt->braddr.u64[1] = braddr->u64[1];
    }

    return pkt;
}

#ifdef MODULE_GNRC_SIXLOWPAN_ND_BORDER_ROUTER
gnrc_sixlowpan_nd_router_abr_t *gnrc_sixlowpan_nd_router_abr_create(ipv6_addr_t *addr,
                                                                    unsigned int ltime)
{
    assert(addr != NULL);
    gnrc_sixlowpan_nd_router_abr_t *abr = _get_abr(addr);
    if (abr == NULL) {
        return NULL;
    }
    /* TODO: store and get this somewhere stable */
    abr->version = 0;
    abr->ltime = (uint16_t)ltime;
    abr->addr.u64[0] = addr->u64[0];
    abr->addr.u64[1] = addr->u64[1];
    memset(abr->ctxs, 0, sizeof(abr->ctxs));
    abr->prfs = NULL;
    return abr;
}

int gnrc_sixlowpan_nd_router_abr_add_prf(gnrc_sixlowpan_nd_router_abr_t* abr,
                                         gnrc_ipv6_netif_t *iface, gnrc_ipv6_netif_addr_t *prefix)
{
    assert((iface != NULL) && (prefix != NULL));
    gnrc_sixlowpan_nd_router_prf_t *prf_ent;
    if ((abr < _abrs) || (abr > (_abrs + GNRC_SIXLOWPAN_ND_ROUTER_ABR_NUMOF))) {
        return -ENOENT;
    }
    prf_ent = _get_free_prefix(iface, &prefix->addr, prefix->prefix_len);
    if (prf_ent == NULL) {
        return -ENOMEM;
    }
    if (prf_ent->iface == NULL) {
        prf_ent->iface = iface;
        prf_ent->prefix = prefix;
        LL_PREPEND(abr->prfs, prf_ent);
        abr->version++; /* TODO: store somewhere stable */
    }
    return 0;
}


void gnrc_sixlowpan_nd_router_abr_rem_prf(gnrc_sixlowpan_nd_router_abr_t *abr,
                                          gnrc_ipv6_netif_t *iface, gnrc_ipv6_netif_addr_t *prefix)
{
    assert((iface != NULL) && (prefix != NULL));
    gnrc_sixlowpan_nd_router_prf_t *prf_ent = abr->prfs, *prev = NULL;
    if ((abr < _abrs) || (abr > (_abrs + GNRC_SIXLOWPAN_ND_ROUTER_ABR_NUMOF))) {
        return;
    }
    while (prf_ent) {
        if (prf_ent->prefix == prefix) {
            if (prev == NULL) {
                abr->prfs = prf_ent->next;
            }
            else {
                prev->next = prf_ent->next;
            }
            prf_ent->next = NULL;
            prf_ent->iface = NULL;
            prf_ent->prefix = NULL;
            abr->version++; /* TODO: store somewhere stable */
            break;
        }
        prev = prf_ent;
        prf_ent = prf_ent->next;
    }
}

int gnrc_sixlowpan_nd_router_abr_add_ctx(gnrc_sixlowpan_nd_router_abr_t *abr, uint8_t cid)
{
    if ((abr < _abrs) || (abr > (_abrs + GNRC_SIXLOWPAN_ND_ROUTER_ABR_NUMOF))) {
        return -ENOENT;
    }
    if (cid >= GNRC_SIXLOWPAN_CTX_SIZE) {
        return -EINVAL;
    }
    if (bf_isset(abr->ctxs, cid)) {
        return -EADDRINUSE;
    }
    bf_set(abr->ctxs, cid);
    abr->version++; /* TODO: store somewhere stable */
    return 0;
}

void gnrc_sixlowpan_nd_router_abr_rem_ctx(gnrc_sixlowpan_nd_router_abr_t *abr, uint8_t cid)
{
    if ((abr < _abrs) || (abr > (_abrs + GNRC_SIXLOWPAN_ND_ROUTER_ABR_NUMOF))) {
        return;
    }
    if (cid >= GNRC_SIXLOWPAN_CTX_SIZE) {
        return;
    }
    bf_unset(abr->ctxs, cid);
    abr->version++; /* TODO: store somewhere stable */
    return;
}
#endif
/** @} */
