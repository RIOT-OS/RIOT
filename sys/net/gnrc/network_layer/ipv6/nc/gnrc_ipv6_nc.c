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

#include "net/gnrc/ipv6.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/nc.h"
#include "net/gnrc/ipv6/netif.h"
#include "net/gnrc/ndp.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/sixlowpan/nd.h"
#include "thread.h"
#include "xtimer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu8 etc. */
#include <inttypes.h>

static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

static gnrc_ipv6_nc_t ncache[GNRC_IPV6_NC_SIZE];

void gnrc_ipv6_nc_init(void)
{
    memset(ncache, 0, sizeof(ncache));
}

gnrc_ipv6_nc_t *_find_free_entry(void)
{
    for (int i = 0; i < GNRC_IPV6_NC_SIZE; i++) {
        if (ipv6_addr_is_unspecified(&(ncache[i].ipv6_addr))) {
            return ncache + i;
        }
    }

    return NULL;
}

gnrc_ipv6_nc_t *gnrc_ipv6_nc_add(kernel_pid_t iface, const ipv6_addr_t *ipv6_addr,
                                 const void *l2_addr, size_t l2_addr_len, uint8_t flags)
{
    gnrc_ipv6_nc_t *free_entry = NULL;

    if (ipv6_addr == NULL) {
        DEBUG("ipv6_nc: address was NULL\n");
        return NULL;
    }

    if ((l2_addr_len > GNRC_IPV6_NC_L2_ADDR_MAX) || ipv6_addr_is_unspecified(ipv6_addr)) {
        DEBUG("ipv6_nc: invalid parameters\n");
        return NULL;
    }

    for (int i = 0; i < GNRC_IPV6_NC_SIZE; i++) {
        if (ipv6_addr_equal(&(ncache[i].ipv6_addr), ipv6_addr)) {
            DEBUG("ipv6_nc: Address %s already registered.\n",
                  ipv6_addr_to_str(addr_str, ipv6_addr, sizeof(addr_str)));

            if ((l2_addr != NULL) && (l2_addr_len > 0)) {
                DEBUG("ipv6_nc: Update to L2 address %s",
                      gnrc_netif_addr_to_str(addr_str, sizeof(addr_str),
                                             l2_addr, l2_addr_len));

                memcpy(&(ncache[i].l2_addr), l2_addr, l2_addr_len);
                ncache[i].l2_addr_len = l2_addr_len;
                ncache[i].flags = flags;
                DEBUG(" with flags = 0x%0x\n", flags);

            }
            return &ncache[i];
        }

        if (ipv6_addr_is_unspecified(&(ncache[i].ipv6_addr)) && !free_entry) {
            /* found the first free entry */
            free_entry = &ncache[i];
        }
    }

    if (!free_entry) {
        /* reached end of NC without finding updateable or free entry */
        DEBUG("ipv6_nc: neighbor cache full.\n");
        return NULL;
    }

    /* Otherwise, fill free entry with your fresh information */
    free_entry->iface = iface;

#ifdef MODULE_GNRC_NDP_NODE
    free_entry->pkts = NULL;
#endif
    memcpy(&(free_entry->ipv6_addr), ipv6_addr, sizeof(ipv6_addr_t));
    DEBUG("ipv6_nc: Register %s for interface %" PRIkernel_pid,
          ipv6_addr_to_str(addr_str, ipv6_addr, sizeof(addr_str)),
          iface);

    if ((l2_addr != NULL) && (l2_addr_len > 0)) {
        DEBUG(" to L2 address %s",
              gnrc_netif_addr_to_str(addr_str, sizeof(addr_str),
                                     l2_addr, l2_addr_len));
        memcpy(&(free_entry->l2_addr), l2_addr, l2_addr_len);
        free_entry->l2_addr_len = l2_addr_len;
    }

    free_entry->flags = flags;

    DEBUG(" with flags = 0x%0x\n", flags);

    if (gnrc_ipv6_nc_get_state(free_entry) == GNRC_IPV6_NC_STATE_INCOMPLETE) {
        DEBUG("ipv6_nc: Set remaining probes to %" PRIu8 "\n", (uint8_t) GNRC_NDP_MAX_MC_NBR_SOL_NUMOF);
        free_entry->probes_remaining = GNRC_NDP_MAX_MC_NBR_SOL_NUMOF;
    }

#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
    free_entry->type_timeout_msg.type = GNRC_SIXLOWPAN_ND_MSG_AR_TIMEOUT;
    free_entry->type_timeout_msg.content.ptr = (char *) free_entry;
#endif

    free_entry->rtr_timeout_msg.type = GNRC_NDP_MSG_RTR_TIMEOUT;
    free_entry->rtr_timeout_msg.content.ptr = (char *) free_entry;

#if defined(MODULE_GNRC_NDP_ROUTER) || defined(MODULE_GNRC_SIXLOWPAN_ND_BORDER_ROUTER)
    free_entry->rtr_adv_msg.type = GNRC_NDP_MSG_RTR_ADV_DELAY;
    free_entry->rtr_adv_msg.content.ptr = (char *) free_entry;
#endif

    free_entry->nbr_sol_msg.content.ptr = (char *) free_entry;

    return free_entry;
}

void gnrc_ipv6_nc_remove(kernel_pid_t iface, const ipv6_addr_t *ipv6_addr)
{
    gnrc_ipv6_nc_t *entry = gnrc_ipv6_nc_get(iface, ipv6_addr);

    if (entry != NULL) {
        DEBUG("ipv6_nc: Remove %s for interface %" PRIkernel_pid "\n",
              ipv6_addr_to_str(addr_str, ipv6_addr, sizeof(addr_str)),
              iface);

#ifdef MODULE_GNRC_NDP_NODE
        while (entry->pkts != NULL) {
            gnrc_pktbuf_release(entry->pkts->pkt);
            entry->pkts->pkt = NULL;
            gnrc_pktqueue_remove_head(&entry->pkts);
        }
#endif
#ifdef MODULE_GNRC_SIXLOWPAN_ND_ROUTER
        xtimer_remove(&entry->type_timeout);
#endif
#if defined(MODULE_GNRC_NDP_ROUTER) || defined(MODULE_GNRC_SIXLOWPAN_ND_BORDER_ROUTER)
        xtimer_remove(&entry->rtr_adv_timer);
#endif

        ipv6_addr_set_unspecified(&(entry->ipv6_addr));
        entry->iface = KERNEL_PID_UNDEF;
        entry->flags = 0;
    }
}

gnrc_ipv6_nc_t *gnrc_ipv6_nc_get(kernel_pid_t iface, const ipv6_addr_t *ipv6_addr)
{
    if ((ipv6_addr == NULL) || (ipv6_addr_is_unspecified(ipv6_addr))) {
        DEBUG("ipv6_nc: address was NULL or ::\n");
        return NULL;
    }

    for (int i = 0; i < GNRC_IPV6_NC_SIZE; i++) {
        if (((ncache[i].iface == KERNEL_PID_UNDEF) || (iface == KERNEL_PID_UNDEF) ||
             (iface == ncache[i].iface)) && ipv6_addr_equal(&(ncache[i].ipv6_addr), ipv6_addr)) {
            DEBUG("ipv6_nc: Found entry for %s on interface %" PRIkernel_pid
                  " (0 = all interfaces) [%p]\n",
                  ipv6_addr_to_str(addr_str, ipv6_addr, sizeof(addr_str)),
                  iface, (void *)(ncache + i));

            return ncache + i;
        }
    }

    return NULL;
}

gnrc_ipv6_nc_t *gnrc_ipv6_nc_get_next(gnrc_ipv6_nc_t *prev)
{
    if (prev == NULL) {
        prev = ncache;
    }
    else {
        prev++;     /* get next entry */
    }

    while (prev < (ncache + GNRC_IPV6_NC_SIZE)) { /* while not reached end */
        if (!ipv6_addr_is_unspecified(&(prev->ipv6_addr))) {
            return prev;
        }

        prev++;
    }

    return NULL;
}

gnrc_ipv6_nc_t *gnrc_ipv6_nc_get_next_router(gnrc_ipv6_nc_t *prev)
{
    for (gnrc_ipv6_nc_t *router = gnrc_ipv6_nc_get_next(prev); router != NULL;
         router = gnrc_ipv6_nc_get_next(router)) {
        if (router->flags & GNRC_IPV6_NC_IS_ROUTER) {
            return router;
        }
    }

    return NULL;
}

gnrc_ipv6_nc_t *gnrc_ipv6_nc_still_reachable(const ipv6_addr_t *ipv6_addr)
{
    gnrc_ipv6_nc_t *entry = gnrc_ipv6_nc_get(KERNEL_PID_UNDEF, ipv6_addr);

    if (entry == NULL) {
        DEBUG("ipv6_nc: No entry found for %s\n",
              ipv6_addr_to_str(addr_str, ipv6_addr, sizeof(addr_str)));
        return NULL;
    }

    if ((gnrc_ipv6_nc_get_state(entry) != GNRC_IPV6_NC_STATE_INCOMPLETE) &&
        (gnrc_ipv6_nc_get_state(entry) != GNRC_IPV6_NC_STATE_UNMANAGED)) {
#if defined(MODULE_GNRC_IPV6_NETIF) && defined(MODULE_VTIMER) && defined(MODULE_GNRC_IPV6)
        gnrc_ipv6_netif_t *iface = gnrc_ipv6_netif_get(entry->iface);

        gnrc_ndp_internal_reset_nbr_sol_timer(entry, iface->reach_time,
                                              GNRC_NDP_MSG_NC_STATE_TIMEOUT, gnrc_ipv6_pid);
#endif

        DEBUG("ipv6_nc: Marking entry %s as reachable\n",
              ipv6_addr_to_str(addr_str, ipv6_addr, sizeof(addr_str)));
        entry->flags &= ~(GNRC_IPV6_NC_STATE_MASK >> GNRC_IPV6_NC_STATE_POS);
        entry->flags |= (GNRC_IPV6_NC_STATE_REACHABLE >> GNRC_IPV6_NC_STATE_POS);
    }

    return entry;
}

/** @} */
