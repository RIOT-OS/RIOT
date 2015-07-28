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

#include "log.h"
#include "net/ng_ipv6.h"
#include "net/ng_ipv6/addr.h"
#include "net/ng_ipv6/nc.h"
#include "net/ng_ipv6/netif.h"
#include "net/ng_ndp.h"
#include "net/ng_pktbuf.h"
#include "thread.h"
#include "timex.h"
#include "vtimer.h"

#if (LOG_LEVEL > LOG_LEVEL_KERNEL)
/* For PRIu8 etc. */
#include <inttypes.h>
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
    ng_ipv6_nc_t *free_entry = NULL;

    if (ipv6_addr == NULL) {
        LOG_ERROR("ipv6_nc: address was NULL\n");
        return NULL;
    }

    if ((l2_addr_len > NG_IPV6_NC_L2_ADDR_MAX) || ng_ipv6_addr_is_unspecified(ipv6_addr)) {
        LOG_ERROR("ipv6_nc: invalid parameters\n");
        return NULL;
    }

    for (int i = 0; i < NG_IPV6_NC_SIZE; i++) {
        if (ng_ipv6_addr_equal(&(ncache[i].ipv6_addr), ipv6_addr)) {
            LOG_WARNING("ipv6_nc: Address %s already registered.\n",
                  ng_ipv6_addr_to_str(addr_str, ipv6_addr, sizeof(addr_str)));

            if ((l2_addr != NULL) && (l2_addr_len > 0)) {
                LOG_DEBUG("ipv6_nc: Update to L2 address %s",
                      ng_netif_addr_to_str(addr_str, sizeof(addr_str),
                                           l2_addr, l2_addr_len));

                memcpy(&(ncache[i].l2_addr), l2_addr, l2_addr_len);
                ncache[i].l2_addr_len = l2_addr_len;
                ncache[i].flags = flags;
                LOG_DEBUG(" with flags = 0x%0x\n", flags);

            }
            return &ncache[i];
        }

        if (ng_ipv6_addr_is_unspecified(&(ncache[i].ipv6_addr)) && !free_entry) {
            /* found the first free entry */
            free_entry = &ncache[i];
        }
    }

    if (!free_entry) {
        /* reached end of NC without finding updateable or free entry */
        LOG_ERROR("ipv6_nc: neighbor cache full.\n");
        return NULL;
    }

    /* Otherwise, fill free entry with your fresh information */
    free_entry->iface = iface;

    free_entry->pkts = NULL;
    memcpy(&(free_entry->ipv6_addr), ipv6_addr, sizeof(ng_ipv6_addr_t));
    LOG_DEBUG("ipv6_nc: Register %s for interface %" PRIkernel_pid,
          ng_ipv6_addr_to_str(addr_str, ipv6_addr, sizeof(addr_str)),
          iface);

    if ((l2_addr != NULL) && (l2_addr_len > 0)) {
        LOG_DEBUG(" to L2 address %s",
              ng_netif_addr_to_str(addr_str, sizeof(addr_str),
                                   l2_addr, l2_addr_len));
        memcpy(&(free_entry->l2_addr), l2_addr, l2_addr_len);
        free_entry->l2_addr_len = l2_addr_len;
    }

    free_entry->flags = flags;

    LOG_DEBUG(" with flags = 0x%0x\n", flags);

    if (ng_ipv6_nc_get_state(free_entry) == NG_IPV6_NC_STATE_INCOMPLETE) {
        LOG_DEBUG("ipv6_nc: Set remaining probes to %" PRIu8 "\n", NG_NDP_MAX_MC_NBR_SOL_NUMOF);
        free_entry->probes_remaining = NG_NDP_MAX_MC_NBR_SOL_NUMOF;
    }

    return free_entry;
}

void ng_ipv6_nc_remove(kernel_pid_t iface, const ng_ipv6_addr_t *ipv6_addr)
{
    ng_ipv6_nc_t *entry = ng_ipv6_nc_get(iface, ipv6_addr);

    if (entry != NULL) {
        LOG_DEBUG("ipv6_nc: Remove %s for interface %" PRIkernel_pid "\n",
              ng_ipv6_addr_to_str(addr_str, ipv6_addr, sizeof(addr_str)),
              iface);

        while (entry->pkts != NULL) {
#ifdef MODULE_NG_PKTBUF
            ng_pktbuf_release(entry->pkts->pkt);
#endif
            entry->pkts->pkt = NULL;
            ng_pktqueue_remove_head(&entry->pkts);
        }

        ng_ipv6_addr_set_unspecified(&(entry->ipv6_addr));
        entry->iface = KERNEL_PID_UNDEF;
        entry->flags = 0;
    }
}

ng_ipv6_nc_t *ng_ipv6_nc_get(kernel_pid_t iface, const ng_ipv6_addr_t *ipv6_addr)
{
    if (ipv6_addr == NULL) {
        LOG_ERROR("ipv6_nc: address was NULL\n");
        return NULL;
    }

    for (int i = 0; i < NG_IPV6_NC_SIZE; i++) {
        if (((iface == KERNEL_PID_UNDEF) || (iface == ncache[i].iface)) &&
            ng_ipv6_addr_equal(&(ncache[i].ipv6_addr), ipv6_addr)) {
            LOG_DEBUG("ipv6_nc: Found entry for %s on interface %" PRIkernel_pid
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
        LOG_WARNING("ipv6_nc: No entry found for %s\n",
              ng_ipv6_addr_to_str(addr_str, ipv6_addr, sizeof(addr_str)));
        return NULL;
    }

    if (ng_ipv6_nc_get_state(entry) != NG_IPV6_NC_STATE_INCOMPLETE) {
#if defined(MODULE_NG_IPV6_NETIF) && defined(MODULE_VTIMER) && defined(MODULE_NG_IPV6)
        ng_ipv6_netif_t *iface = ng_ipv6_netif_get(entry->iface);
        timex_t t = iface->reach_time;

        vtimer_remove(&entry->nbr_sol_timer);
        vtimer_set_msg(&entry->nbr_sol_timer, t, ng_ipv6_pid,
                       NG_NDP_MSG_NC_STATE_TIMEOUT, entry);
#endif

        LOG_DEBUG("ipv6_nc: Marking entry %s as reachable\n",
              ng_ipv6_addr_to_str(addr_str, ipv6_addr, sizeof(addr_str)));
        entry->flags &= ~(NG_IPV6_NC_STATE_MASK >> NG_IPV6_NC_STATE_POS);
        entry->flags |= (NG_IPV6_NC_STATE_REACHABLE >> NG_IPV6_NC_STATE_POS);
    }

    return entry;
}

/** @} */
