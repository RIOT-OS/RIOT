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
 * @brief       Shell command for interacting with lwIP network interfaces
 *
 * @author      Erik Ekman <eekman@google.com>
 *
 * @}
 */

#include <stdio.h>
#include "lwip/netif.h"
#include "lwip/netifapi.h"
#include "lwip/prot/dhcp.h"
#include "net/netdev.h"
#include "net/netopt.h"

#ifdef MODULE_LWIP_IPV6
static void _netif_list_ipv6(struct netif *netif, int addr_index) {
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
    printf("\n");
}
#endif

static void _netif_list(struct netif *netif) {
    int i;
    char name[8];
    struct netdev *dev = netif->state;
    netif_get_name((netif_t *)netif, name);
    printf("Iface %s HWaddr: ", name);
    for (i = 0; i < netif->hwaddr_len; i++) {
        printf("%02x", netif->hwaddr[i]);
        if ((i+1) < netif->hwaddr_len) {
            printf(":");
        }
    }
    printf("  Link: %s  State: %s  %s\n",
        netif_is_link_up(netif) ? "up  " : "down",
        netif_is_up(netif) ? "up  " : "down",
        netif == netif_default ? "Default route" : "");
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
    if (netif_is_flag_set(netif, NETIF_FLAG_ETHERNET)) {
        printf(" dhcp: ");
        if (dhcp_supplied_address(netif)) {
            printf("bound");
        } else {
            struct dhcp *dhcp = netif_dhcp_data(netif);
            if (dhcp && dhcp->state > DHCP_STATE_OFF) {
                printf("active");
            } else {
                printf("off");
            }
        }
    }
    printf("\n");
#endif

#ifdef MODULE_LWIP_IPV6
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        if (netif_ip6_addr_state(netif, i) != IP6_ADDR_INVALID) {
            _netif_list_ipv6(netif, i);
        }
    }
#endif
}

static void _usage(const char *cmd) {
    printf("usage: %s [iface]\n", cmd);
    puts("      List all or a specific network interface");
    printf("usage: %s <iface> {up|down}\n", cmd);
    puts("      Enable or disable an interface (independent of link)");
    printf("usage: %s <iface> default\n", cmd);
    puts("      Set interface as default for routing");
}

int _lwip_netif_config(int argc, char **argv)
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
    } else if (strcmp(argv[1], "help") == 0) {
        _usage(argv[0]);
        return 0;
    } else {
        LOCK_TCPIP_CORE();
        struct netif *netif = netif_find(argv[1]);
        UNLOCK_TCPIP_CORE();
        if (!netif) {
            printf("Interface '%s' not found.\n", argv[1]);
            _usage(argv[0]);
            return 1;
        }
        if (argc == 2) {
            _netif_list(netif);
            return 0;
        } else if (argc == 3) {
            if (strcmp(argv[2], "up") == 0) {
                netifapi_netif_set_up(netif);
                return 0;
            } else if (strcmp(argv[2], "down") == 0) {
                netifapi_netif_set_down(netif);
                return 0;
            } else if (strcmp(argv[2], "default") == 0) {
                netifapi_netif_set_default(netif);
                return 0;
            }
        }
    }
    _usage(argv[0]);
    return 1;
}
