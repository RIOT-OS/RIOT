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

#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ndp.h"
#include "net/gnrc/netif/internal.h"
#include "net/gnrc/sixlowpan/nd.h"
#if GNRC_IPV6_NIB_CONF_DNS
#include "net/sock/dns.h"
#endif

#if GNRC_IPV6_NIB_CONF_MULTIHOP_P6C
#include "_nib-6ln.h"
#endif  /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */
#include "_nib-router.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if GNRC_IPV6_NIB_CONF_ROUTER
static char addr_str[IPV6_ADDR_MAX_STR_LEN];

static void _snd_ra(gnrc_netif_t *netif, const ipv6_addr_t *dst,
                    bool final, _nib_abr_entry_t *abr);

void _handle_reply_rs(_nib_onl_entry_t *host)
{
    gnrc_netif_t *netif = gnrc_netif_get_by_pid(_nib_onl_get_if(host));

    assert(netif != NULL);
    gnrc_netif_acquire(netif);
    if (gnrc_netif_is_rtr_adv(netif)) {
        _snd_rtr_advs(netif, &host->ipv6, false);
    }
    gnrc_netif_release(netif);
}

void _handle_snd_mc_ra(gnrc_netif_t *netif)
{
    gnrc_netif_acquire(netif);
    assert(netif != NULL);
    if (!gnrc_netif_is_6ln(netif)) {
        bool final_ra = (netif->ipv6.ra_sent > (UINT8_MAX - NDP_MAX_FIN_RA_NUMOF));
        uint32_t next_ra_time = random_uint32_range(NDP_MIN_RA_INTERVAL_MS,
                                                    NDP_MAX_RA_INTERVAL_MS);
        uint32_t next_scheduled = _evtimer_lookup(netif, GNRC_IPV6_NIB_SND_MC_RA);

        /* router has router advertising interface or the RA is one of the
         * (now deactivated) routers final one (and there is no next
         * scheduled within the possible time for next_ra_time) */
        if ((final_ra && (next_scheduled > NDP_MAX_RA_INTERVAL_MS)) ||
            gnrc_netif_is_rtr_adv(netif)) {
            _snd_rtr_advs(netif, NULL, final_ra);
            netif->ipv6.last_ra = (xtimer_now_usec64() / US_PER_MS) & UINT32_MAX;
            if ((netif->ipv6.ra_sent < NDP_MAX_INIT_RA_NUMOF) || final_ra) {
                if ((netif->ipv6.ra_sent < NDP_MAX_INIT_RA_NUMOF) &&
                    (next_ra_time > NDP_MAX_INIT_RA_INTERVAL)) {
                    next_ra_time = NDP_MAX_INIT_RA_INTERVAL;
                }
                netif->ipv6.ra_sent++;
            }
            /* netif->ipv6.ra_sent overflowed => this was our last final RA */
            if (netif->ipv6.ra_sent != 0) {
                _evtimer_add(netif, GNRC_IPV6_NIB_SND_MC_RA, &netif->ipv6.snd_mc_ra,
                             next_ra_time);
            }
        }
    }
    gnrc_netif_release(netif);
}

void _snd_rtr_advs(gnrc_netif_t *netif, const ipv6_addr_t *dst, bool final)
{
#if GNRC_IPV6_NIB_CONF_MULTIHOP_P6C
    _nib_abr_entry_t *abr = NULL;

    DEBUG("nib: Send router advertisements for each border router:\n");
    while ((abr = _nib_abr_iter(abr))) {
        DEBUG("    - %s\n", ipv6_addr_to_str(addr_str, &abr->addr,
                                             sizeof(addr_str)));
        _snd_ra(netif, dst, final, abr);
    }
#else   /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */
    _snd_ra(netif, dst, final, NULL);
#endif  /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */
}

static gnrc_pktsnip_t *_offl_to_pio(_nib_offl_entry_t *offl,
                                    gnrc_pktsnip_t *ext_opts)
{
    uint32_t now = (xtimer_now_usec64() / US_PER_MS) & UINT32_MAX;
    gnrc_pktsnip_t *pio;
    uint8_t flags = 0;
    uint32_t valid_ltime = (offl->valid_until == UINT32_MAX) ? UINT32_MAX :
                           ((offl->valid_until - now) / MS_PER_SEC);
    uint32_t pref_ltime = (offl->pref_until == UINT32_MAX) ? UINT32_MAX :
                          ((offl->pref_until - now) / MS_PER_SEC);

    DEBUG("nib: Build PIO for %s/%u\n",
          ipv6_addr_to_str(addr_str, &offl->pfx, sizeof(addr_str)),
          offl->pfx_len);
    if (offl->flags & _PFX_ON_LINK) {
        flags |= NDP_OPT_PI_FLAGS_L;
    }
    if (offl->flags & _PFX_SLAAC) {
        flags |= NDP_OPT_PI_FLAGS_A;
    }
    pio = gnrc_ndp_opt_pi_build(&offl->pfx, offl->pfx_len, valid_ltime,
                                pref_ltime, flags, ext_opts);

    if ((pio == NULL) && (ext_opts != NULL)) {
        DEBUG("nib: No space left in packet buffer. Not adding PIO\n");
        return NULL;
    }
    return pio;
}

static gnrc_pktsnip_t *_build_ext_opts(gnrc_netif_t *netif,
                                       _nib_abr_entry_t *abr)
{
    gnrc_pktsnip_t *ext_opts = NULL;
    _nib_offl_entry_t *pfx = NULL;
    unsigned id = netif->pid;

#if GNRC_IPV6_NIB_CONF_DNS && SOCK_HAS_IPV6
    uint32_t rdnss_ltime = _evtimer_lookup(&sock_dns_server,
                                           GNRC_IPV6_NIB_RDNSS_TIMEOUT);

    if ((rdnss_ltime < UINT32_MAX) &&
        (!ipv6_addr_is_link_local((ipv6_addr_t *)sock_dns_server.addr.ipv6))) {
        gnrc_pktsnip_t *rdnsso = gnrc_ndp_opt_rdnss_build(
                rdnss_ltime * MS_PER_SEC,
                (ipv6_addr_t *)&sock_dns_server.addr,
                1U, ext_opts
            );
        if (rdnsso == NULL) {
            /* gnrc_ndp_opt_rdnss_build() only returns NULL when pktbuf is full
             * in this configuration */
            DEBUG("nib: No space left in packet buffer. Not adding RDNSSO\n");
            return NULL;
        }
        ext_opts = rdnsso;
    }
#endif  /* GNRC_IPV6_NIB_CONF_DNS */
#if GNRC_IPV6_NIB_CONF_MULTIHOP_P6C
    uint16_t ltime;
    gnrc_pktsnip_t *abro;

#ifdef MODULE_GNRC_SIXLOWPAN_CTX
    for (int i = 0; i < GNRC_SIXLOWPAN_CTX_SIZE; i++) {
        gnrc_sixlowpan_ctx_t *ctx;
        if (bf_isset(abr->ctxs, i) &&
            ((ctx = gnrc_sixlowpan_ctx_lookup_id(i)) != NULL)) {
            gnrc_pktsnip_t *sixco = gnrc_sixlowpan_nd_opt_6ctx_build(
                                            ctx->prefix_len, ctx->flags_id,
                                            ctx->ltime, &ctx->prefix, NULL);
            if (sixco == NULL) {
                DEBUG("nib: No space left in packet buffer. Not adding 6LO\n");
                return NULL;
            }
            ext_opts = sixco;
        }
    }
#endif  /* MODULE_GNRC_SIXLOWPAN_CTX */
    while ((pfx = _nib_abr_iter_pfx(abr, pfx))) {
        if (_nib_onl_get_if(pfx->next_hop) == id) {
            if ((ext_opts = _offl_to_pio(pfx, ext_opts)) == NULL) {
                return NULL;
            }
        }
    }
    ltime = (gnrc_netif_is_6lbr(netif)) ?
            (SIXLOWPAN_ND_OPT_ABR_LTIME_DEFAULT) :
            (abr->valid_until - _now_min());
    (void)ltime;    /* gnrc_sixlowpan_nd_opt_abr_build might evaluate to NOP */
    abro = gnrc_sixlowpan_nd_opt_abr_build(abr->version, ltime, &abr->addr,
                                           ext_opts);
    if (abro == NULL) {
        DEBUG("nib: No space left in packet buffer. Not adding ABRO\n");
        return NULL;
    }
    ext_opts = abro;
#else   /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */
    (void)abr;
    while ((pfx = _nib_offl_iter(pfx))) {
        if ((pfx->mode & _PL) && (_nib_onl_get_if(pfx->next_hop) == id)) {
            if ((ext_opts = _offl_to_pio(pfx, ext_opts)) == NULL) {
                return NULL;
            }
        }
    }
#endif  /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */
    return ext_opts;
}

void _set_rtr_adv(gnrc_netif_t *netif)
{
    DEBUG("nib: set RTR_ADV flag for interface %i\n", netif->pid);
    netif->ipv6.ra_sent = 0;
    netif->flags |= GNRC_NETIF_FLAGS_IPV6_RTR_ADV;
    _handle_snd_mc_ra(netif);
}

static void _snd_ra(gnrc_netif_t *netif, const ipv6_addr_t *dst,
                    bool final, _nib_abr_entry_t *abr)
{
    gnrc_pktsnip_t *ext_opts = NULL;

    if (!final) {
        ext_opts = _build_ext_opts(netif, abr);
    }

    gnrc_ndp_rtr_adv_send(netif, NULL, dst, final, ext_opts);
}
#else  /* GNRC_IPV6_NIB_CONF_ROUTER */
typedef int dont_be_pedantic;
#endif /* GNRC_IPV6_NIB_CONF_ROUTER */

/** @} */
