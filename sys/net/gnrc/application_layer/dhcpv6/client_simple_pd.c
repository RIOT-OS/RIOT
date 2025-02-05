/*
 * Copyright (C) 2018-20 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */

#include "event.h"
#include "log.h"
#include "net/dhcpv6/client.h"
#include "net/ipv6/addr.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ipv6/nib/ft.h"
#include "net/gnrc/netif/internal.h"

#include "net/gnrc/dhcpv6/client/simple_pd.h"

/**
 * @brief   Find upstream network interface
 *
 * Either the one network interface configured at compile-time with @ref
 * CONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM is picked or the first network
 * interface that is not a 6LoWPAN interfaces if
 * `CONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM` is 0.
 *
 * @return  The upstream network interface.
 */
static gnrc_netif_t *_find_upstream_netif(void)
{
    gnrc_netif_t *netif = NULL;

    if (CONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM) {
        return gnrc_netif_get_by_pid(CONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM);
    }

    if (CONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM_TYPE != NETDEV_ANY) {
        return gnrc_netif_get_by_type(CONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM_TYPE,
                                      CONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM_IDX);
    }

    while ((netif = gnrc_netif_iter(netif))) {
        if (!gnrc_netif_is_6lo(netif)) {
            LOG_WARNING("DHCPv6: Selecting interface %d as upstream\n",
                        netif->pid);
            return netif;
        }
    }
    return NULL;
}

/**
 * @brief   Configure upstream netif to be in line with configuration script
 *
 * Set route and link-local address in accordance to
 * `dist/tools/ethos/setup_network.sh`.
 *
 * @note    This might not be necessary with a properly set-up DHCPv6 server
 *          (automatically configures a route for the delegated prefix) and
 *          upstream router (sends periodic router advertisements).
 *
 * @param[in] upstream_netif    The upstream netif  The upstream netif
 */
static void _configure_upstream_netif(gnrc_netif_t *upstream_netif)
{
    if (IS_ACTIVE(CONFIG_GNRC_DHCPV6_CLIENT_6LBR_STATIC_ROUTE)) {
        ipv6_addr_t addr = {
                .u8 = { 0xfe, 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }
            };

        /* set default route to host machine (as set-up in setup_network.sh) */
        gnrc_ipv6_nib_ft_add(NULL, 0, &addr, upstream_netif->pid, 0);
        /* set additional link-local address to provide a well-known next hop
         * for static route configuration on the host machine */
        addr.u8[15] = 2;
        gnrc_netif_ipv6_addr_add(upstream_netif, &addr, 64, 0);
    }
}

/**
 * @brief   Configures all 6LoWPAN interfaces to request a 64-bit prefix
 */
static void _configure_dhcpv6_client(void)
{
    gnrc_netif_t *netif = NULL;
    gnrc_netif_t *upstream = _find_upstream_netif();

    if (upstream == NULL) {
        LOG_ERROR("DHCPv6: No upstream interface found!\n");
        return;
    }

    if (IS_ACTIVE(MODULE_DHCPV6_CLIENT_IA_NA)) {
        upstream->ipv6.aac_mode |= GNRC_NETIF_AAC_DHCP;
    }

    while ((netif = gnrc_netif_iter(netif))) {
        if (IS_USED(MODULE_GNRC_DHCPV6_CLIENT_6LBR)
            && !gnrc_netif_is_6lo(netif)) {
            continue;
        }
        if (netif != upstream) {
            dhcpv6_client_req_ia_pd(netif->pid, 64U);
        }
    }
}

/**
 * @brief   The DHCPv6 client thread
 */
static void _dhcpv6_client_simple_pd_init_hook(void)
{
    /* configure client to request prefix delegation for WPAN interfaces */
    _configure_dhcpv6_client();
    /* set client configuration mode to stateful */
    dhcpv6_client_set_conf_mode(DHCPV6_CLIENT_CONF_MODE_STATEFUL);
}

void gnrc_dhcpv6_client_simple_pd_init(void)
{
    gnrc_netif_t *upstream_netif = _find_upstream_netif();

    if (upstream_netif == NULL) {
        LOG_ERROR("DHCPv6: No upstream interface found!\n");
        return ;
    }
    _configure_upstream_netif(upstream_netif);

    dhcpv6_client_set_init_hook(_dhcpv6_client_simple_pd_init_hook, upstream_netif->pid);
}

/** @} */
