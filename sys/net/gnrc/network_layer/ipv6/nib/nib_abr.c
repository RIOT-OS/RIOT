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

#include <stdio.h>

#include "net/gnrc/ipv6/nib/abr.h"

#include "_nib-6ln.h"
#include "_nib-internal.h"

#if GNRC_IPV6_NIB_CONF_MULTIHOP_P6C
#if GNRC_IPV6_NIB_CONF_6LBR
int gnrc_ipv6_nib_abr_add(const ipv6_addr_t *addr)
{
    _nib_abr_entry_t *abr;
    _nib_offl_entry_t *offl = NULL;

    mutex_lock(&_nib_mutex);
    if ((abr = _nib_abr_add(addr)) == NULL) {
        mutex_unlock(&_nib_mutex);
        return -ENOMEM;
    }
    abr->valid_until = 0U;
    while ((offl = _nib_offl_iter(offl))) {
        if (offl->mode & _PL) {
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
    mutex_unlock(&_nib_mutex);
    return 0;
}

void gnrc_ipv6_nib_abr_del(const ipv6_addr_t *addr)
{
    mutex_lock(&_nib_mutex);
    _nib_abr_remove(addr);
    mutex_unlock(&_nib_mutex);
}
#endif  /* GNRC_IPV6_NIB_CONF_6LBR */

bool gnrc_ipv6_nib_abr_iter(void **state, gnrc_ipv6_nib_abr_t *entry)
{
    _nib_abr_entry_t *abr = *state;

    mutex_lock(&_nib_mutex);
    while ((abr = _nib_abr_iter(abr)) != NULL) {
        if (!ipv6_addr_is_unspecified(&abr->addr)) {
            memcpy(&entry->addr, &abr->addr, sizeof(entry->addr));
            entry->version = abr->version;
            entry->valid_until = abr->valid_until;
            break;
        }
    }
    mutex_unlock(&_nib_mutex);
    *state = abr;
    return (*state != NULL);
}

void gnrc_ipv6_nib_abr_print(gnrc_ipv6_nib_abr_t *abr)
{
    char addr_str[IPV6_ADDR_MAX_STR_LEN];
    uint32_t now = _now_min();

    printf("%s v%" PRIu32 " expires %" PRIu32 "min\n",
           ipv6_addr_to_str(addr_str, &abr->addr, sizeof(addr_str)),
           abr->version,
           (abr->valid_until != 0) ? (abr->valid_until - now) :
                                     SIXLOWPAN_ND_OPT_ABR_LTIME_DEFAULT);
}
#else
typedef int dont_be_pedantic;
#endif  /* GNRC_IPV6_NIB_CONF_MULTIHOP_P6C */

/** @} */
