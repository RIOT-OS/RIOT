/*
 * Copyright (C) 2021 Google LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell command for printing lwIP network interface status
 *
 * @author      Erik Ekman <eekman@google.com>
 *
 * @}
 */

#include <kernel_defines.h>
#include <stdio.h>

#include "lwip/netif/compat.h"
#include "net/netdev.h"
#include "net/netopt.h"
#include "shell.h"

#ifdef MODULE_LWIP_IPV6
static void _netif_list_ipv6(struct netif *netif, int addr_index, uint8_t state)
{
    printf("        inet6 addr: ");
    ip_addr_debug_print(LWIP_DBG_ON, netif_ip_addr6(netif, addr_index));
    printf(" scope: ");
    if (ip6_addr_isglobal(netif_ip6_addr(netif, addr_index))) {
        printf("global");
    } else if (ip6_addr_islinklocal(netif_ip6_addr(netif, addr_index))) {
        printf("link");
    } else if (ip6_addr_issitelocal(netif_ip6_addr(netif, addr_index))) {
        printf("site");
    } else {
        printf("unknown");
    }
    printf(" state:");
    if (ip6_addr_istentative(state)) {
        printf(" tentative (%u probes send)",
                (unsigned)(state & IP6_ADDR_TENTATIVE_COUNT_MASK));
    }
    if (ip6_addr_isvalid(state)) {
        printf(" valid");
    }
    if (ip6_addr_ispreferred(state)) {
        printf(" preferred");
    }
    if (ip6_addr_isduplicated(state)) {
        printf(" duplicated");
    }
    printf("\n");
}
#endif

static void _netif_list(struct netif *netif)
{
    int i;
    char name[CONFIG_NETIF_NAMELENMAX];
    struct netdev *dev = netif->state;
    lwip_netif_t *compat = container_of(netif, lwip_netif_t, lwip_netif);
    netif_get_name(&compat->common_netif, name);
    printf("Iface %s HWaddr: ", name);
    for (i = 0; i < netif->hwaddr_len; i++) {
        printf("%02x", netif->hwaddr[i]);
        if ((i+1) < netif->hwaddr_len) {
            printf(":");
        }
    }
    printf(" Link: %s State: %s\n",
        netif_is_link_up(netif) ? "up" : "down",
        netif_is_up(netif) ? "up" : "down");
    printf("        Link type: %s\n",
        (dev->driver->get(dev, NETOPT_IS_WIRED, &i, sizeof(i)) > 0) ?
            "wired" : "wireless");
#ifdef MODULE_LWIP_IPV4
    printf("        inet addr: ");
    ip_addr_debug_print(LWIP_DBG_ON, netif_ip_addr4(netif));
    printf(" mask: ");
    ip_addr_debug_print(LWIP_DBG_ON, netif_ip_netmask4(netif));
    printf(" gw: ");
    ip_addr_debug_print(LWIP_DBG_ON, netif_ip_gw4(netif));
    printf("\n");
#endif

#ifdef MODULE_LWIP_IPV6
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        uint8_t state = netif_ip6_addr_state(netif, i);
        /* Note: !ip_addr_isinvalid() also matches tentative addresses,
         * ip_addr_isvalid() would filter them out. We want both valid and
         * tentative addresses to aid debugging when an address gets stuck in
         * tentative state. _netif_list_ipv6() prints the state (e.g. valid or
         * tentative), so users will not confuse tentative addresses with
         * valid ones. */
        if (!ip6_addr_isinvalid(state)) {
            _netif_list_ipv6(netif, i, state);
        }
    }
#endif
}

static int _lwip_netif_config(int argc, char **argv)
{
    if (argc < 2) {
        /* List in interface order, which is normally reverse of list order */
        struct netif *netif;
        int netifs = 0;
        int listed = 0;
        u8_t i;
        NETIF_FOREACH(netif) netifs++;
        for (i = 0; listed < netifs; i++) {
            NETIF_FOREACH(netif) {
                if (i == netif->num) {
                    _netif_list(netif);
                    listed++;
                }
            }
        }
        return 0;
    }
    printf("%s takes no arguments.\n", argv[0]);
    return 1;
}

SHELL_COMMAND(ifconfig, "List network interfaces", _lwip_netif_config);
