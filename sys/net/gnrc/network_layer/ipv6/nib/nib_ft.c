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
#include <errno.h>
#include <stdio.h>

#include "_nib-internal.h"

#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ipv6/nib/ft.h"

int gnrc_ipv6_nib_ft_get(const ipv6_addr_t *dst, gnrc_pktsnip_t *pkt,
                         gnrc_ipv6_nib_ft_t *fte)
{
    assert(dst && fte);
    _nib_acquire();
    int res = _nib_get_route(dst, pkt, fte);
    _nib_release();
    return res;
}

int gnrc_ipv6_nib_ft_add(const ipv6_addr_t *dst, unsigned dst_len,
                         const ipv6_addr_t *next_hop, unsigned iface,
                         uint16_t ltime)
{
    int res = 0;
    bool is_default_route = (!dst || (dst_len == 0) ||
                             ipv6_addr_is_unspecified(dst));

    if ((iface == 0) || (is_default_route && !next_hop)) {
        return -EINVAL;
    }
    _nib_acquire();
    if (is_default_route) {
        _nib_dr_entry_t *ptr;
        if (!(ptr = _nib_drl_add(next_hop, iface))) {
            res = -ENOMEM;
        }
        else {
            _prime_def_router = ptr;
            if (ltime > 0) {
                _evtimer_add(ptr, GNRC_IPV6_NIB_RTR_TIMEOUT,
                             &ptr->rtr_timeout, ltime * MS_PER_SEC);
            }
        }
    }
    else {
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER)
        dst_len = (dst_len > 128) ? 128 : dst_len;
        _nib_offl_entry_t *ptr;
        if (!(ptr = _nib_ft_add(next_hop, iface, dst, dst_len))) {
            res = -ENOMEM;
        }
        else if (ltime > 0) {
            _evtimer_add(ptr, GNRC_IPV6_NIB_ROUTE_TIMEOUT,
                         &ptr->route_timeout, ltime * MS_PER_SEC);
        }
#else /* CONFIG_GNRC_IPV6_NIB_ROUTER */
        res = -ENOTSUP;
#endif
    }
    _nib_release();
    return res;
}

void gnrc_ipv6_nib_ft_del(const ipv6_addr_t *dst, unsigned dst_len)
{
    _nib_acquire();
    if (!dst || (dst_len == 0) || ipv6_addr_is_unspecified(dst)) {
        _nib_dr_entry_t *entry = _nib_drl_get_dr();
        if (entry) {
            _nib_drl_remove(entry);
        }
    }
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER)
    else {
        _nib_offl_entry_t *entry = NULL;
        while ((entry = _nib_offl_iter(entry))) {
            if ((entry->pfx_len == dst_len) &&
                (ipv6_addr_match_prefix(&entry->pfx, dst) >= dst_len)) {
                _nib_ft_remove(entry);
                break;
            }
        }
    }
#endif
    _nib_release();
}

bool gnrc_ipv6_nib_ft_iter(const ipv6_addr_t *next_hop, unsigned iface,
                           void **state, gnrc_ipv6_nib_ft_t *fte)
{
    assert(state && fte);

    if (!*state || _nib_offl_is_entry(*state)) {
        _nib_offl_entry_t *offl = *state;

        while ((offl = _nib_offl_iter(offl))) {
            assert(offl->mode != 0);
            if (offl->next_hop &&
                ((iface == 0) || (iface == _nib_onl_get_if(offl->next_hop))) &&
                (!next_hop || ipv6_addr_equal(&offl->next_hop->ipv6, next_hop))) {
                _nib_ft_get(offl, fte);
                *state = offl;
                return true;
            }
        }
        *state = NULL;
    }
    _nib_dr_entry_t *entry = *state;
    while ((entry = _nib_drl_iter(entry))) {
        if (entry->next_hop &&
            ((iface == 0) || (iface == _nib_onl_get_if(entry->next_hop))) &&
            (!next_hop || ipv6_addr_equal(&entry->next_hop->ipv6, next_hop))) {
            _nib_drl_ft_get(entry, fte);
            break;
        }
    }
    *state = entry;
    return (*state != NULL);
}

void gnrc_ipv6_nib_ft_print(const gnrc_ipv6_nib_ft_t *fte)
{
    char addr_str[IPV6_ADDR_MAX_STR_LEN];

    if ((fte->dst_len == 0) || ipv6_addr_is_unspecified(&fte->dst)) {
        printf("default%s ", (fte->primary ? "*" : ""));
    }
    else {
        printf("%s/%u ", ipv6_addr_to_str(addr_str, &fte->dst, sizeof(addr_str)),
               fte->dst_len);
    }
    if (!ipv6_addr_is_unspecified(&fte->next_hop)) {
        printf("via %s ", ipv6_addr_to_str(addr_str, &fte->next_hop,
                                           sizeof(addr_str)));
    }
    printf("dev #%u\n", fte->iface);
}

/** @} */
