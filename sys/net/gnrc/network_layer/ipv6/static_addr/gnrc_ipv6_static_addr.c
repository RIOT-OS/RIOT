/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/rpl.h"
#include "net/ipv6/addr.h"
#include "net/netdev.h"
#include "net/netopt.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   ID of the upstream interface, set to 0 to attempt auto-select
 */
#ifndef CONFIG_GNRC_IPV6_STATIC_ADDR_UPSTREAM
#define CONFIG_GNRC_IPV6_STATIC_ADDR_UPSTREAM 0
#endif

/**
 * @brief   ID of the downstream interface, set to 0 to attempt auto-select
 */
#ifndef CONFIG_GNRC_IPV6_STATIC_ADDR_DOWNSTREAM
#define CONFIG_GNRC_IPV6_STATIC_ADDR_DOWNSTREAM 0
#endif

static void _config_upstream(gnrc_netif_t *upstream)
{
    ipv6_addr_t addr;

    const char *static_addr =
#ifdef CONFIG_GNRC_IPV6_STATIC_ADDR
        CONFIG_GNRC_IPV6_STATIC_ADDR;
#else
        NULL;
#endif

    const char *static_route =
#ifdef CONFIG_GNRC_IPV6_STATIC_DEFAULT_ROUTER
        CONFIG_GNRC_IPV6_STATIC_DEFAULT_ROUTER;
#else
        NULL;
#endif

    DEBUG("gnrc_ipv6_static_addr: interface %u selected as upstream\n", upstream->pid);

    /* configure static address */
    if (static_addr != NULL &&
        ipv6_addr_from_str(&addr, static_addr) != NULL) {
        gnrc_netif_ipv6_addr_add_internal(upstream, &addr, 128,
                                          GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID);
    }

    /* configure static route */
    if (static_route != NULL &&
        ipv6_addr_from_str(&addr, static_route) != NULL) {
        gnrc_ipv6_nib_ft_add(&addr, 0, &addr, upstream->pid, 0);
    }
}

static void _config_downstream(gnrc_netif_t *downstream)
{
    const char *static_prefix =
#ifdef CONFIG_GNRC_IPV6_STATIC_PREFIX
        CONFIG_GNRC_IPV6_STATIC_PREFIX;
#else
        NULL;
#endif

    DEBUG("gnrc_ipv6_static_addr: interface %u selected as downstream\n", downstream->pid);

    if (static_prefix == NULL) {
        return;
    }

    ipv6_addr_t prefix;
    int len = ipv6_prefix_from_str(&prefix, static_prefix);
    if (len <= 0) {
        return;
    }

    /* configure subnet on downstream interface */
    int idx = gnrc_netif_ipv6_add_prefix(downstream, &prefix, len,
                                         UINT32_MAX, UINT32_MAX);
    if (idx < 0) {
        DEBUG("gnrc_ipv6_static_addr: adding prefix to %u failed\n", downstream->pid);
        return;
    }

    /* start advertising subnet */
    gnrc_ipv6_nib_change_rtr_adv_iface(downstream, true);

    /* configure RPL root if applicable */
    if (IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LBR)) {
        gnrc_rpl_configure_root(downstream, &downstream->ipv6.addrs[idx]);
    }
}

void auto_init_gnrc_ipv6_static_addr(void)
{
    gnrc_netif_t *upstream = NULL;
    gnrc_netif_t *downstream = NULL;

    if (gnrc_netif_highlander() || gnrc_netif_numof() == 1) {
        upstream = gnrc_netif_iter(NULL);

        if (IS_ACTIVE(CONFIG_GNRC_IPV6_STATIC_ADDR_UPSTREAM)) {
            assert(upstream->pid == CONFIG_GNRC_IPV6_STATIC_ADDR_UPSTREAM);
        }
    } else {

        if (IS_ACTIVE(CONFIG_GNRC_IPV6_STATIC_ADDR_UPSTREAM)) {
            upstream = gnrc_netif_get_by_pid(CONFIG_GNRC_IPV6_STATIC_ADDR_UPSTREAM);
        }

        if (IS_ACTIVE(CONFIG_GNRC_IPV6_STATIC_ADDR_DOWNSTREAM)) {
            downstream = gnrc_netif_get_by_pid(CONFIG_GNRC_IPV6_STATIC_ADDR_DOWNSTREAM);
        }

        gnrc_netif_t *netif = NULL;
        while ((netif = gnrc_netif_iter(netif))) {
            bool is_wired = gnrc_netapi_get(netif->pid, NETOPT_IS_WIRED, 0, NULL, 0) == 1;

            if (!upstream && is_wired) {
                upstream = netif;
            } else if (!downstream && !is_wired) {
                downstream = netif;
            }
        }
    }

    if (upstream) {
        _config_upstream(upstream);
    }

    if (downstream) {
        _config_downstream(downstream);
    }
}

/** @} */
