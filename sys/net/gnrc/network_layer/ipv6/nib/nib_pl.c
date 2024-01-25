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
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <kernel_defines.h>

#include "net/gnrc/ipv6/nib/pl.h"
#include "net/gnrc/netif/internal.h"
#include "timex.h"
#include "evtimer.h"

#include "_nib-internal.h"
#include "_nib-router.h"

int gnrc_ipv6_nib_pl_set(unsigned iface,
                         const ipv6_addr_t *pfx, unsigned pfx_len,
                         uint32_t valid_ltime, uint32_t pref_ltime)
{
    _nib_offl_entry_t *dst;
    ipv6_addr_t tmp = IPV6_ADDR_UNSPECIFIED;

    assert((pfx != NULL));
    if (pfx_len > IPV6_ADDR_BIT_LEN) {
        pfx_len = IPV6_ADDR_BIT_LEN;
    }
    ipv6_addr_init_prefix(&tmp, pfx, pfx_len);
    /* pfx_len == 0 implicitly checked, since this leaves tmp unspecified */
    if (ipv6_addr_is_unspecified(&tmp) || ipv6_addr_is_link_local(pfx) ||
        ipv6_addr_is_multicast(pfx) || (pref_ltime > valid_ltime)) {
        return -EINVAL;
    }
    _nib_acquire();
    dst = _nib_pl_add(iface, pfx, pfx_len, valid_ltime,
                      pref_ltime);
    if (dst == NULL) {
        _nib_release();
        return -ENOMEM;
    }
#ifdef MODULE_GNRC_NETIF
    gnrc_netif_t *netif = gnrc_netif_get_by_pid(iface);

    if (netif == NULL) {
        _nib_release();
        return 0;
    }
    gnrc_netif_acquire(netif);
    /* prefixes within a 6Lo-ND-performing network are typically off-link, the
     * border router however should configure the prefix as on-link to only do
     * address resolution towards the LoWPAN and not the upstream interface
     * See https://github.com/RIOT-OS/RIOT/pull/10627 and follow-ups
     */
    if (!gnrc_netif_is_6ln(netif) || gnrc_netif_is_6lbr(netif)) {
        dst->flags |= _PFX_ON_LINK;
    }

    /* Auto-configuration only works if the prefix is more than a single address */
    if ((netif->ipv6.aac_mode & GNRC_NETIF_AAC_AUTO) && (pfx_len < 128)) {
        dst->flags |= _PFX_SLAAC;
    }
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LBR) && IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C)
    if (gnrc_netif_is_6lbr(netif)) {
        _nib_abr_entry_t *abr = NULL;

        while ((abr = _nib_abr_iter(abr))) {
            abr->version++;
            _nib_abr_add_pfx(abr, dst);
        }
    }
#endif
    gnrc_netif_release(netif);
#endif  /* MODULE_GNRC_NETIF */
    _nib_release();
#if defined(MODULE_GNRC_NETIF) && IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER)
    /* update prefixes down-stream */
    _handle_snd_mc_ra(netif);
#endif
    return 0;
}

void gnrc_ipv6_nib_pl_del(unsigned iface,
                          const ipv6_addr_t *pfx, unsigned pfx_len)
{
    _nib_offl_entry_t *dst = NULL;

    assert(pfx != NULL);
    _nib_acquire();
    while ((dst = _nib_offl_iter(dst)) != NULL) {
        assert(dst->next_hop != NULL);
        if ((pfx_len == dst->pfx_len) &&
            ((iface == 0) || (iface == _nib_onl_get_if(dst->next_hop))) &&
            (ipv6_addr_match_prefix(pfx, &dst->pfx) >= pfx_len)) {

            /* notify downstream nodes about the prefix removal */
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER)
            gnrc_netif_t *netif = gnrc_netif_get_by_pid(iface);

            if (netif && (netif->flags & GNRC_NETIF_FLAGS_IPV6_RTR_ADV)) {
                _snd_rtr_advs_drop_pfx(netif, &ipv6_addr_all_nodes_link_local, dst);
            }
#endif
            /* remove the prefix & associated address*/
            _nib_offl_remove_prefix(dst);
            break;
        }
    }
    _nib_release();
}

bool gnrc_ipv6_nib_pl_iter(unsigned iface, void **state,
                           gnrc_ipv6_nib_pl_t *entry)
{
    _nib_offl_entry_t *dst = *state;

    _nib_acquire();
    while ((dst = _nib_offl_iter(dst)) != NULL) {
        const _nib_onl_entry_t *node = dst->next_hop;
        if ((node != NULL) && (dst->mode & _PL) &&
            ((iface == 0) || (_nib_onl_get_if(node) == iface))) {
            entry->pfx_len = dst->pfx_len;
            ipv6_addr_set_unspecified(&entry->pfx);
            ipv6_addr_init_prefix(&entry->pfx, &dst->pfx, dst->pfx_len);
            entry->iface = _nib_onl_get_if(node);
            entry->valid_until = dst->valid_until;
            entry->pref_until = dst->pref_until;
            break;
        }
    }
    _nib_release();
    *state = dst;
    return (*state != NULL);
}

void gnrc_ipv6_nib_pl_print(gnrc_ipv6_nib_pl_t *entry)
{
    char addr_str[IPV6_ADDR_MAX_STR_LEN];
    ipv6_addr_t pfx = IPV6_ADDR_UNSPECIFIED;
    uint32_t now = evtimer_now_msec();

    ipv6_addr_init_prefix(&pfx, &entry->pfx, entry->pfx_len);
    printf("%s/%u ", ipv6_addr_to_str(addr_str, &pfx, sizeof(addr_str)),
           entry->pfx_len);
    printf("dev #%u ", entry->iface);
    if (entry->valid_until < UINT32_MAX) {
        printf(" expires %lu sec", (entry->valid_until - now) / MS_PER_SEC);
    }
    if (entry->pref_until < UINT32_MAX) {
        printf(" deprecates %lu sec", (now >= entry->pref_until ? 0 : entry->pref_until - now) / MS_PER_SEC);
    }
    puts("");
}

/** @} */
