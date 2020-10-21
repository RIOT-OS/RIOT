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
#include <stdio.h>
#include <kernel_defines.h>

#include "net/gnrc/ipv6/nib/abr.h"
#include "net/gnrc/netif.h"

#include "_nib-6ln.h"
#include "_nib-internal.h"

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C)
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LBR)
int gnrc_ipv6_nib_abr_add(const ipv6_addr_t *addr)
{
    _nib_abr_entry_t *abr;
    _nib_offl_entry_t *offl = NULL;
    gnrc_netif_t *netif = gnrc_netif_get_by_ipv6_addr(addr);

    assert(netif != NULL);
    _nib_acquire();
    if ((abr = _nib_abr_add(addr)) == NULL) {
        _nib_release();
        return -ENOMEM;
    }
    abr->valid_until = 0U;
    /* Associate all existing prefixes in the prefix list of the border router's
     * downstream interface to the authoritative border router so they are
     * advertised in a Router Advertisement with the Authoritative Border Router
     * Option (ABRO) in a respective Prefix Information Option (PIO)
     * (see https://tools.ietf.org/html/rfc6775#section-8.1.1). */
    while ((offl = _nib_offl_iter(offl))) {
        if ((offl->mode & _PL) &&
            (_nib_onl_get_if(offl->next_hop) == (unsigned)netif->pid)) {
            _nib_abr_add_pfx(abr, offl);
        }
    }
#ifdef MODULE_GNRC_SIXLOWPAN_CTX    /* included optionally for NIB testing */
    for (uint8_t id = 0; id < GNRC_SIXLOWPAN_CTX_SIZE; id++) {
        if (gnrc_sixlowpan_ctx_lookup_id(id) != NULL) {
            bf_set(abr->ctxs, id);
        }
    }
#endif  /* MODULE_GNRC_SIXLOWPAN_CTX */
    _nib_release();
    return 0;
}

void gnrc_ipv6_nib_abr_del(const ipv6_addr_t *addr)
{
    _nib_acquire();
    _nib_abr_remove(addr);
    _nib_release();
}
#endif  /* CONFIG_GNRC_IPV6_NIB_6LBR */

bool gnrc_ipv6_nib_abr_iter(void **state, gnrc_ipv6_nib_abr_t *entry)
{
    _nib_abr_entry_t *abr = *state;

    _nib_acquire();
    while ((abr = _nib_abr_iter(abr)) != NULL) {
        if (!ipv6_addr_is_unspecified(&abr->addr)) {
            memcpy(&entry->addr, &abr->addr, sizeof(entry->addr));
            entry->version = abr->version;
            entry->valid_until = abr->valid_until;
            break;
        }
    }
    _nib_release();
    *state = abr;
    return (*state != NULL);
}

void gnrc_ipv6_nib_abr_print(gnrc_ipv6_nib_abr_t *abr)
{
    char addr_str[IPV6_ADDR_MAX_STR_LEN];
    uint32_t now = evtimer_now_min();

    printf("%s v%" PRIu32 " expires %" PRIu32 "min\n",
           ipv6_addr_to_str(addr_str, &abr->addr, sizeof(addr_str)),
           abr->version,
           (abr->valid_until != 0) ? (abr->valid_until - now) :
                                     SIXLOWPAN_ND_OPT_ABR_LTIME_DEFAULT);
}
#else
typedef int dont_be_pedantic;
#endif  /* CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C */

/** @} */
