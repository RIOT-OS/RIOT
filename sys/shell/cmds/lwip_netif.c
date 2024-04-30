/*
 * Copyright (C) 2021 Google LLC
 *               2024 Krzysztof Cabaj <kcabaj@gmail.com>
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
 *              and configuration of IPv4 address
 *
 * @author      Erik Ekman <eekman@google.com>
 *              Krzysztof Cabaj <kcabaj@gmail.com>
 *
 * @}
 */

#include <kernel_defines.h>
#include <stdio.h>

#include "lwip/netif/compat.h"
#include "net/netdev.h"
#include "net/netopt.h"
#include "shell.h"

#include "arch/sys_arch.h"
#include <arpa/inet.h>

#include "net/netif.h"

#ifdef MODULE_LWIP_IPV6
static void _netif_list_ipv6(struct netif *netif, int addr_index, uint8_t state)
{
    printf("        inet6 addr: ");
    ip_addr_debug_print(LWIP_DBG_ON, netif_ip_addr6(netif, addr_index));
    printf(" scope: ");
    if (ip6_addr_isglobal(netif_ip6_addr(netif, addr_index))) {
        printf("global");
    }
    else if (ip6_addr_islinklocal(netif_ip6_addr(netif, addr_index))) {
        printf("link");
    }
    else if (ip6_addr_issitelocal(netif_ip6_addr(netif, addr_index))) {
        printf("site");
    }
    else {
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
        if ((i + 1) < netif->hwaddr_len) {
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

#ifdef MODULE_LWIP_IPV4
static void _usage_add4(char *cmd)
{
    printf("usage: %s add <interface> <IPv4>/<prefix>\n", cmd);
    printf("usage: %s add <interface> <IPv4>/<prefix> gw <IPv4>\n", cmd);
}

static void _lwip_prefix_to_subnet(int prefix, ip4_addr_t *subnet)
{
    uint32_t value = 0;
    uint32_t tmp = 0x80000000;

    for (int i = 0; i < prefix; i++) {
        value += tmp;
        tmp = tmp >> 1;
    }
    subnet->addr = htonl(value);
}

static int _lwip_netif_add4(int argc, char **argv)
{
    struct netif *iface;
    char *ip_ptr, *prefix_ptr = NULL;
    ip4_addr_t ip, subnet, gw;
    int prefix;

    if (argc != 4 && argc != 6) {
        printf("error: invalid number of parameters\n");
        _usage_add4(argv[0]);
        return 1;
    }

    sys_lock_tcpip_core();
    iface = netif_find(argv[2]);

    if (iface == NULL) {
        printf("error: invalid interface name (names are case sensitive)\n");
        sys_unlock_tcpip_core();
        return 1;
    }

    ip_ptr = argv[3];
    while ((*ip_ptr) != 0) {
        if ((*ip_ptr) == '/') {
            *ip_ptr = 0;
            prefix_ptr = ip_ptr + 1;
        }

        ip_ptr++;
    }
    ip_ptr = argv[3];

    if (prefix_ptr == NULL) {
        printf("error: invalid IPv4 prefix notation\n");
        _usage_add4(argv[0]);
        sys_unlock_tcpip_core();
        return 1;
    }

    if (inet_pton(AF_INET, ip_ptr, &ip.addr) != 1) {
        printf("error:invalid IPv4 address\n");
        sys_unlock_tcpip_core();
        return 1;
    }

    prefix = atoi(prefix_ptr);

    if (prefix < 0 || prefix > 32) {
        printf("error:invalid prefix, should be in range <0, 32>\n");
        sys_unlock_tcpip_core();
        return 1;
    }

    _lwip_prefix_to_subnet(prefix, &subnet);

    if (argc == 4) {
        netif_set_addr(iface, &ip, &subnet, NULL);
    }
    else {
        if (strcmp("gw", argv[4]) != 0) {
            printf("error: invalid subcommand \"%s\"\n", argv[4]);
            _usage_add4(argv[0]);
            sys_unlock_tcpip_core();
            return 1;
        }

        if (inet_pton(AF_INET, argv[5], &gw.addr) != 1) {
            printf("error: invalid gateway address\n");
            sys_unlock_tcpip_core();
            return 1;
        }
        netif_set_addr(iface, &ip, &subnet, &gw);
    }

    sys_unlock_tcpip_core();
    return 0;
}
#endif

#ifdef MODULE_LWIP_IPV6
static void _usage_add6(char *cmd)
{
    printf("usage: %s add for LWIP IPv6 currently not implemented\n", cmd);
}

static int _lwip_netif_add6(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("error: LWIP IPv6 configuration currently not implemented\n");

    return 0;
}
#endif

static void _lwip_netif_help(char *cmd)
{
    printf("usage: %s\n", cmd);
    printf("usage: %s help\n", cmd);
#ifdef MODULE_LWIP_IPV4
    _usage_add4(cmd);
#endif
#ifdef MODULE_LWIP_IPV6
    _usage_add6(cmd);
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
    else {
        if (strcmp("help", argv[1]) == 0) {
            _lwip_netif_help(argv[0]);
        }
#if defined(MODULE_LWIP_IPV4) || defined(MODULE_LWIP_IPV6)
        else if (strcmp("add", argv[1]) == 0) {
            if (argc != 4 && argc != 6) {
                printf("error: invalid number of parameters\n");
#ifdef MODULE_LWIP_IPV4
                _usage_add4(argv[0]);
#endif
#ifdef MODULE_LWIP_IPV6
                _usage_add6(argv[0]);
#endif
                return 0;
            }

            char *prefix_ptr = strchr(argv[3], '/');

            if (prefix_ptr == NULL) {
                printf("error: provide IP address with prefix\n");
                return 0;
            }

            *prefix_ptr = 0;

#ifdef MODULE_LWIP_IPV4
            ipv4_addr_t ip4;

            if (ipv4_addr_from_buf(&ip4, argv[3], strlen(argv[3])) != NULL) {
                *prefix_ptr = '/';
                _lwip_netif_add4(argc, argv);
                return 1;
            }
#endif
#ifdef MODULE_LWIP_IPV6
            ipv6_addr_t ip6;
            if (ipv6_addr_from_buf(&ip6, argv[3], strlen(argv[3])) != NULL) {
                *prefix_ptr = '/';
                _lwip_netif_add6(argc, argv);
                return 1;
            }
#endif
            printf("error: use proper IPv4 or IPv6 address\n");
        }
#endif
        else {
            printf("error: invalid subcommand - use help\n");
        }
    }
    return 1;
}

SHELL_COMMAND(ifconfig, "List network interfaces", _lwip_netif_config);
