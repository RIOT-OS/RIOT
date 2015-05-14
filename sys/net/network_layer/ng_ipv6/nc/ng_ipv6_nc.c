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

#include <errno.h>
#include <string.h>

#include "net/ng_ipv6/addr.h"
#include "net/ng_ipv6/nc.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
static char addr_str[NG_IPV6_ADDR_MAX_STR_LEN];
#endif

static ng_ipv6_nc_t ncache[NG_IPV6_NC_SIZE];

void ng_ipv6_nc_init(void)
{
    memset(ncache, 0, sizeof(ncache));
}

ng_ipv6_nc_t *_find_free_entry(void)
{
    for (int i = 0; i < NG_IPV6_NC_SIZE; i++) {
        if (ng_ipv6_addr_is_unspecified(&(ncache[i].ipv6_addr))) {
            return ncache + i;
        }
    }

    return NULL;
}

ng_ipv6_nc_t *ng_ipv6_nc_add(kernel_pid_t iface, const ng_ipv6_addr_t *ipv6_addr,
                             const void *l2_addr, size_t l2_addr_len, uint8_t flags)
{
    if (ipv6_addr == NULL) {
        DEBUG("ipv6_nc: address was NULL\n");
        return NULL;
    }

    if ((l2_addr_len > NG_IPV6_NC_L2_ADDR_MAX) || ng_ipv6_addr_is_unspecified(ipv6_addr)) {
        DEBUG("ipv6_nc: invalid parameters\n");
        return NULL;
    }

    for (int i = 0; i < NG_IPV6_NC_SIZE; i++) {
        if (ng_ipv6_addr_equal(&(ncache[i].ipv6_addr), ipv6_addr)) {
            DEBUG("ipv6_nc: Address %s already registered.\n",
                  ng_ipv6_addr_to_str(addr_str, ipv6_addr, sizeof(addr_str)));

            if ((l2_addr != NULL) && (l2_addr_len > 0)) {
                DEBUG("ipv6_nc: Update to L2 address %s",
                      ng_netif_addr_to_str(addr_str, sizeof(addr_str),
                                           l2_addr, l2_addr_len));

                memcpy(&(ncache[i].l2_addr), l2_addr, l2_addr_len);
                ncache[i].l2_addr_len = l2_addr_len;
                ncache[i].flags = flags;
                DEBUG(" with flags = 0x%0x\n", flags);

            }
            return &ncache[i];
        }

        if (ng_ipv6_addr_is_unspecified(&(ncache[i].ipv6_addr))) {
            ncache[i].iface = iface;

            ng_pktqueue_init(&(ncache[i].pkts));
            memcpy(&(ncache[i].ipv6_addr), ipv6_addr, sizeof(ng_ipv6_addr_t));
            DEBUG("ipv6_nc: Register %s for interface %" PRIkernel_pid,
                  ng_ipv6_addr_to_str(addr_str, ipv6_addr, sizeof(addr_str)),
                  iface);

            if ((l2_addr != NULL) && (l2_addr_len > 0)) {
                DEBUG(" to L2 address %s",
                      ng_netif_addr_to_str(addr_str, sizeof(addr_str),
                                           l2_addr, l2_addr_len));
                memcpy(&(ncache[i].l2_addr), l2_addr, l2_addr_len);
                ncache[i].l2_addr_len = l2_addr_len;
            }

            ncache[i].flags = flags;

            DEBUG(" with flags = 0x%0x\n", flags);

            return &ncache[i];
        }
    }

    DEBUG("ipv6_nc: neighbor cache full.\n");

    return NULL;
}

void ng_ipv6_nc_remove(kernel_pid_t iface, const ng_ipv6_addr_t *ipv6_addr)
{
    ng_ipv6_nc_t *entry = ng_ipv6_nc_get(iface, ipv6_addr);

    if (entry != NULL) {
        DEBUG("ipv6_nc: Remove %s for interface %" PRIkernel_pid "\n",
              ng_ipv6_addr_to_str(addr_str, ipv6_addr, sizeof(addr_str)),
              iface);

        ng_ipv6_addr_set_unspecified(&(entry->ipv6_addr));
        entry->iface = KERNEL_PID_UNDEF;
        entry->flags = 0;
    }
}

ng_ipv6_nc_t *ng_ipv6_nc_get(kernel_pid_t iface, const ng_ipv6_addr_t *ipv6_addr)
{
    if (ipv6_addr == NULL) {
        DEBUG("ipv6_nc: address was NULL\n");
        return NULL;
    }

    for (int i = 0; i < NG_IPV6_NC_SIZE; i++) {
        if (((iface == KERNEL_PID_UNDEF) || (iface == ncache[i].iface)) &&
            ng_ipv6_addr_equal(&(ncache[i].ipv6_addr), ipv6_addr)) {
            DEBUG("ipv6_nc: Found entry for %s on interface %" PRIkernel_pid
                  " (0 = all interfaces) [%p]\n",
                  ng_ipv6_addr_to_str(addr_str, ipv6_addr, sizeof(addr_str)),
                  iface, (void *)(ncache + i));

            return ncache + i;
        }
    }

    return NULL;
}

ng_ipv6_nc_t *ng_ipv6_nc_get_next(ng_ipv6_nc_t *prev)
{
    if (prev == NULL) {
        prev = ncache;
    }
    else {
        prev++;     /* get next entry */
    }

    while (prev < (ncache + NG_IPV6_NC_SIZE)) { /* while not reached end */
        if (!ng_ipv6_addr_is_unspecified(&(prev->ipv6_addr))) {
            return prev;
        }

        prev++;
    }

    return NULL;
}

ng_ipv6_nc_t *ng_ipv6_nc_get_next_router(ng_ipv6_nc_t *prev)
{
    for (ng_ipv6_nc_t *router = ng_ipv6_nc_get_next(prev); router != NULL;
         router = ng_ipv6_nc_get_next(router)) {
        if (router->flags & NG_IPV6_NC_IS_ROUTER) {
            return router;
        }
    }

    return NULL;
}

ng_ipv6_nc_t *ng_ipv6_nc_still_reachable(const ng_ipv6_addr_t *ipv6_addr)
{
    ng_ipv6_nc_t *entry = ng_ipv6_nc_get(KERNEL_PID_UNDEF, ipv6_addr);

    if (entry == NULL) {
        DEBUG("ipv6_nc: No entry found for %s\n",
              ng_ipv6_addr_to_str(addr_str, ipv6_addr, sizeof(addr_str)));
        return NULL;
    }

    if (((entry->flags & NG_IPV6_NC_STATE_MASK) >> NG_IPV6_NC_STATE_POS) !=
        NG_IPV6_NC_STATE_INCOMPLETE) {
        DEBUG("ipv6_nc: Marking entry %s as reachable\n",
              ng_ipv6_addr_to_str(addr_str, ipv6_addr, sizeof(addr_str)));
        entry->flags &= ~(NG_IPV6_NC_STATE_MASK >> NG_IPV6_NC_STATE_POS);
        entry->flags |= (NG_IPV6_NC_STATE_REACHABLE >> NG_IPV6_NC_STATE_POS);
    }

    return entry;
}

/** @} */
