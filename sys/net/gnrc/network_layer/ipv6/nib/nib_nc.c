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
#include <stdbool.h>
#include <stdio.h>

#include "net/gnrc/ipv6.h"
#include "net/gnrc/netif.h"

#include "net/gnrc/ipv6/nib/nc.h"

#include "_nib-internal.h"
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)
#  include "_nib-6ln.h"
#endif /* CONFIG_GNRC_IPV6_NIB_6LN */

int gnrc_ipv6_nib_nc_set(const ipv6_addr_t *ipv6, unsigned iface,
                         const uint8_t *l2addr, size_t l2addr_len)
{
    _nib_onl_entry_t *node;

    assert(ipv6 != NULL);
    assert(l2addr_len <= CONFIG_GNRC_IPV6_NIB_L2ADDR_MAX_LEN);
    assert((iface > KERNEL_PID_UNDEF) && (iface <= KERNEL_PID_LAST));
    _nib_acquire();
    node = _nib_nc_add(ipv6, iface, GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED);
    if (node == NULL) {
        _nib_release();
        return -ENOMEM;
    }
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ARSM)
    if ((l2addr != NULL) && (l2addr_len > 0)) {
        memcpy(node->l2addr, l2addr, l2addr_len);
    }
    node->l2addr_len = l2addr_len;
#else
    (void)l2addr;
    (void)l2addr_len;
    if (!ipv6_addr_is_link_local(ipv6)) {
        return -EINVAL;
    }
#endif
    node->info &= ~(GNRC_IPV6_NIB_NC_INFO_AR_STATE_MASK |
                    GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK);
    node->info |= (GNRC_IPV6_NIB_NC_INFO_AR_STATE_MANUAL |
                   GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED);
    _nib_release();
    return 0;
}

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN)
int gnrc_ipv6_nib_nc_set_6ln(unsigned iface, const uint8_t *l2addr, size_t l2addr_len)
{
    int res;
    ipv6_addr_t ipv6addr;

    gnrc_netif_t *netif = gnrc_netif_get_by_pid(iface);

    if (netif == NULL) {
        return -ENOENT;
    }
    /* Build link-local IPv6 address based on L2-address. */
    res = _build_ll_ipv6_from_addr(netif, l2addr, l2addr_len, &ipv6addr);
    if (res < 0) {
        return res;
    }
    /* Add as unmanaged entry to neighbor cache. */
    res = gnrc_ipv6_nib_nc_set(&ipv6addr, netif->pid, l2addr, l2addr_len);
    if (res < 0) {
        return res;
    }

    return 0;
}
#endif

void gnrc_ipv6_nib_nc_del(const ipv6_addr_t *ipv6, unsigned iface)
{
    _nib_onl_entry_t *node = NULL;

    _nib_acquire();
    while ((node = _nib_onl_iter(node)) != NULL) {
        if ((_nib_onl_get_if(node) == iface) &&
            ipv6_addr_equal(ipv6, &node->ipv6)) {
            _nib_nc_remove(node);
            break;
        }
    }
    _nib_release();
}

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ARSM)
bool gnrc_ipv6_nib_nc_del_l2(unsigned iface, const uint8_t *l2addr, size_t l2addr_len)
{
    _nib_onl_entry_t *node = NULL;
    bool res = false;

    _nib_acquire();

    /* Find and remove entry with matching l2 address.*/
    while ((node = _nib_onl_iter(node)) != NULL) {
        if ((_nib_onl_get_if(node) == iface) &&
            l2util_addr_equal(l2addr, l2addr_len, node->l2addr, node->l2addr_len)) {
            _nib_nc_remove(node);
            res = true;
            break;
        }
    }
    _nib_release();

    return res;
}
#endif /* CONFIG_GNRC_IPV6_NIB_ARSM */

void gnrc_ipv6_nib_nc_mark_reachable(const ipv6_addr_t *ipv6)
{
    _nib_onl_entry_t *node = NULL;

    _nib_acquire();
    while ((node = _nib_onl_iter(node)) != NULL) {
        if ((node->mode & _NC) && ipv6_addr_equal(ipv6, &node->ipv6)) {
            /* only set reachable if not unmanaged */
            if ((node->info & GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK)) {
                _nib_nc_set_reachable(node);
            }
            break;
        }
    }
    _nib_release();
}

bool gnrc_ipv6_nib_nc_iter(unsigned iface, void **state,
                           gnrc_ipv6_nib_nc_t *entry)
{
    _nib_onl_entry_t *node = *state;

    _nib_acquire();
    while ((node = _nib_onl_iter(node)) != NULL) {
        if ((node->mode & _NC) &&
            ((iface == 0) || (_nib_onl_get_if(node) == iface))) {
            _nib_nc_get(node, entry);
            break;
        }
    }
    *state = node;
    _nib_release();
    return (*state != NULL);
}

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ARSM)
static const char *_nud_str[] = {
    [GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED]     = "-",
    [GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE]   = "UNREACHABLE",
    [GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE]    = "INCOMPLETE",
    [GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE]         = "STALE",
    [GNRC_IPV6_NIB_NC_INFO_NUD_STATE_DELAY]         = "DELAY",
    [GNRC_IPV6_NIB_NC_INFO_NUD_STATE_PROBE]         = "PROBE",
    [GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE]     = "REACHABLE",
};
#endif

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LR)
#define _AR_STR_IDX(state)      ((state) >> GNRC_IPV6_NIB_NC_INFO_AR_STATE_POS)

static const char *_ar_str[] = {
    [_AR_STR_IDX(GNRC_IPV6_NIB_NC_INFO_AR_STATE_GC)]            = "GC",
    [_AR_STR_IDX(GNRC_IPV6_NIB_NC_INFO_AR_STATE_TENTATIVE)]     = "TENTATIVE",
    [_AR_STR_IDX(GNRC_IPV6_NIB_NC_INFO_AR_STATE_REGISTERED)]    = "REGISTERED",
    [_AR_STR_IDX(GNRC_IPV6_NIB_NC_INFO_AR_STATE_MANUAL)]        = "MANUAL",
};
#endif

void gnrc_ipv6_nib_nc_print(gnrc_ipv6_nib_nc_t *entry)
{
    char addr_str[(IPV6_ADDR_MAX_STR_LEN > CONFIG_GNRC_IPV6_NIB_L2ADDR_MAX_LEN) ?
                   IPV6_ADDR_MAX_STR_LEN : CONFIG_GNRC_IPV6_NIB_L2ADDR_MAX_LEN];

    printf("%s ", ipv6_addr_to_str(addr_str, &entry->ipv6, sizeof(addr_str)));
    if (gnrc_ipv6_nib_nc_get_iface(entry) != KERNEL_PID_UNDEF) {
        printf("dev #%u ", gnrc_ipv6_nib_nc_get_iface(entry));
    }
    printf("lladdr %s ", gnrc_netif_addr_to_str(entry->l2addr,
                                                entry->l2addr_len,
                                                addr_str));
    if (gnrc_ipv6_nib_nc_is_router(entry)) {
        printf("router");
    }
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ARSM)
    printf(" %s", _nud_str[gnrc_ipv6_nib_nc_get_nud_state(entry)]);
#endif
#if CONFIG_GNRC_IPV6_NIB_6LR
    printf(" %s",_ar_str[_AR_STR_IDX(gnrc_ipv6_nib_nc_get_ar_state(entry))]);
#endif
    puts("");
}

/** @} */
