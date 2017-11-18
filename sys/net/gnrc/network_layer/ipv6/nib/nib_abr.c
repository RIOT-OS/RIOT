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

#include "net/gnrc/ipv6/nib/abr.h"

#include "_nib-internal.h"

#if GNRC_IPV6_NIB_CONF_6LBR && GNRC_IPV6_NIB_CONF_MULTIHOP_P6C
int gnrc_ipv6_nib_abr_add(const ipv6_addr_t *addr)
{
    _nib_abr_entry_t *abr;
    _nib_offl_entry_t *offl = NULL;

    mutex_lock(&_nib_mutex);
    if ((abr = _nib_abr_add(addr)) == NULL) {
        mutex_unlock(&_nib_mutex);
        return -ENOMEM;
    }
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
#endif
    mutex_unlock(&_nib_mutex);
    return 0;
}

void gnrc_ipv6_nib_abr_del(const ipv6_addr_t *addr)
{
    mutex_lock(&_nib_mutex);
    _nib_abr_remove(addr);
    mutex_unlock(&_nib_mutex);
}
#else
typedef int dont_be_pedantic;
#endif

/** @} */
