/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     sys_shell_commands.h
 * @{
 *
 * @file
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdio.h>
#include <string.h>

#include "kernel_types.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/nc.h"
#include "net/gnrc/netif.h"
#include "thread.h"

/* maximum length of L2 address */
#define MAX_L2_ADDR_LEN (8U)

static void _print_nc_state(gnrc_ipv6_nc_t *entry)
{
    switch (entry->flags & GNRC_IPV6_NC_STATE_MASK) {
        case GNRC_IPV6_NC_STATE_UNMANAGED:
            printf("UNMANAGED   ");
            break;

        case GNRC_IPV6_NC_STATE_UNREACHABLE:
            printf("UNREACHABLE ");
            break;

        case GNRC_IPV6_NC_STATE_INCOMPLETE:
            printf("INCOMPLETE  ");
            break;

        case GNRC_IPV6_NC_STATE_STALE:
            printf("STALE       ");
            break;

        case GNRC_IPV6_NC_STATE_DELAY:
            printf("DELAY       ");
            break;

        case GNRC_IPV6_NC_STATE_PROBE:
            printf("PROBE       ");
            break;

        case GNRC_IPV6_NC_STATE_REACHABLE:
            printf("REACHABLE   ");
            break;

        default:
            printf("UNKNOWN     ");
            break;
    }
}

static void _print_nc_type(gnrc_ipv6_nc_t *entry)
{
    switch (entry->flags & GNRC_IPV6_NC_TYPE_MASK) {
        case GNRC_IPV6_NC_TYPE_GC:
            printf("GC");
            break;

        case GNRC_IPV6_NC_TYPE_TENTATIVE:
            printf("TENT");
            break;

        case GNRC_IPV6_NC_TYPE_REGISTERED:
            printf("REG");
            break;

        default:
            printf("-");
            break;
    }
}

static bool _is_iface(kernel_pid_t iface)
{
#ifdef MODULE_GNRC_NETIF
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    size_t numof = gnrc_netif_get(ifs);

    for (size_t i = 0; i < numof && i < GNRC_NETIF_NUMOF; i++) {
        if (ifs[i] == iface) {
            return true;
        }
    }

    return false;
#else
    return (thread_get(iface) != NULL);
#endif
}

static int _ipv6_nc_list(void)
{
    char ipv6_str[IPV6_ADDR_MAX_STR_LEN];
    char l2addr_str[3 * MAX_L2_ADDR_LEN];

    puts("IPv6 address                    if  L2 address                state       type");
    puts("------------------------------------------------------------------------------");

    for (gnrc_ipv6_nc_t *entry = gnrc_ipv6_nc_get_next(NULL);
         entry != NULL;
         entry = gnrc_ipv6_nc_get_next(entry)) {
        printf("%-30s  %2" PRIkernel_pid "  %-24s  ",
               ipv6_addr_to_str(ipv6_str, &entry->ipv6_addr, sizeof(ipv6_str)),
               entry->iface,
               gnrc_netif_addr_to_str(l2addr_str, sizeof(l2addr_str),
                                      entry->l2_addr, entry->l2_addr_len));
        _print_nc_state(entry);
        _print_nc_type(entry);
        puts("");
    }

    return 0;
}

static inline kernel_pid_t _get_iface(void)
{
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    size_t ifnum = gnrc_netif_get(ifs);
    return (ifnum > 1) ? KERNEL_PID_UNDEF : ifs[0];
}

static int _ipv6_nc_add(int argc, char **argv)
{
    char *ipv6_addr_str;
    char *l2_addr_str = NULL;
    kernel_pid_t iface = KERNEL_PID_UNDEF;
    ipv6_addr_t ipv6_addr;
    uint8_t l2_addr[MAX_L2_ADDR_LEN];
    size_t l2_addr_len = 0;

    switch (argc) {
        case 1:
            iface = _get_iface();
            ipv6_addr_str = argv[0];    /* store for output */
            l2_addr_len = 0;
            break;
        case 2:
            ipv6_addr_str = argv[0];    /* store for output */
            if (ipv6_addr_from_str(&ipv6_addr, ipv6_addr_str) == NULL) {
                iface = atoi(argv[0]);
                ipv6_addr_str = argv[1];
                l2_addr_len = 0;
            }
            else {
                iface = _get_iface();
                l2_addr_str = argv[1];
            }
            break;
        default:
            iface = atoi(argv[0]);
            ipv6_addr_str = argv[1];
            l2_addr_str = argv[2];
            break;
    }

    if (!_is_iface(iface)) {
        puts("error: invalid interface given.");
        return 1;
    }

    if (ipv6_addr_from_str(&ipv6_addr, ipv6_addr_str) == NULL) {
        puts("error: unable to parse IPv6 address.");
        return 1;
    }

    if ((l2_addr_str != NULL) && (l2_addr_len = gnrc_netif_addr_from_str(l2_addr, sizeof(l2_addr),
                                                                         l2_addr_str)) == 0) {
        puts("error: unable to parse link-layer address.");
        return 1;
    }

    if (gnrc_ipv6_nc_add(iface, &ipv6_addr, l2_addr, l2_addr_len, 0) == NULL) {
        puts("error: unable to add address to neighbor cache.");
        return 1;
    }

    printf("success: added IPv6 address %s to neighbor cache\n", ipv6_addr_str);
    return 0;
}

static int _ipv6_nc_del(char *ipv6_addr_str)
{
    ipv6_addr_t ipv6_addr;

    if (ipv6_addr_from_str(&ipv6_addr, ipv6_addr_str) == NULL) {
        puts("error: unable to parse IPv6 address.");
        return 1;
    }

    gnrc_ipv6_nc_remove(KERNEL_PID_UNDEF, &ipv6_addr);
    printf("success: removed %s from neighbor cache\n", ipv6_addr_str);

    return 0;
}

static int _ipv6_nc_reset(void)
{
    gnrc_ipv6_nc_t *tmp = NULL;

    for (gnrc_ipv6_nc_t *entry = gnrc_ipv6_nc_get_next(NULL);
         entry != NULL;
         tmp = entry, entry = gnrc_ipv6_nc_get_next(entry)) {
        if (tmp) {
            gnrc_ipv6_nc_remove(tmp->iface, &tmp->ipv6_addr);
        }
    }

    /* remove last entry */
    if (tmp) {
        gnrc_ipv6_nc_remove(tmp->iface, &tmp->ipv6_addr);
    }

    printf("success: reset neighbor cache\n");

    return 0;
}

int _ipv6_nc_manage(int argc, char **argv)
{
    if ((argc == 1) || (strcmp("list", argv[1]) == 0)) {
        return _ipv6_nc_list();
    }

    if (argc > 1) {
        if (strcmp("add", argv[1]) == 0) {
            return _ipv6_nc_add(argc - 2, &argv[2]);
        }
        if (strcmp("del", argv[1]) == 0) {
            return _ipv6_nc_del(argv[2]);
        }
        if (strcmp("reset", argv[1]) == 0) {
            return _ipv6_nc_reset();
        }
    }

    printf("usage: %s [list]\n"
           "   or: %s add [<iface pid>] <ipv6_addr> <l2_addr>\n"
           "      * <iface pid> is optional if only one interface exists.\n"
           "   or: %s del <ipv6_addr>\n"
           "   or: %s reset\n", argv[0], argv[0], argv[0], argv[0]);
    return 1;
}

int _ipv6_nc_routers(int argc, char **argv)
{
    kernel_pid_t iface = KERNEL_PID_UNDEF;
    char ipv6_str[IPV6_ADDR_MAX_STR_LEN];

    if (argc > 1) {
        iface = atoi(argv[1]);

        if (!_is_iface(iface)) {
            printf("usage: %s [<iface pid>]\n", argv[0]);
            return 1;
        }
    }

    puts("if  Router                          state      type");
    puts("---------------------------------------------------");

    for (gnrc_ipv6_nc_t *entry = gnrc_ipv6_nc_get_next_router(NULL);
         entry != NULL;
         entry = gnrc_ipv6_nc_get_next_router(entry)) {
        if ((iface != KERNEL_PID_UNDEF) && (iface != entry->iface)) {
            continue;
        }

        printf("%2" PRIkernel_pid "  %-30s  ", entry->iface,
               ipv6_addr_to_str(ipv6_str, &entry->ipv6_addr, sizeof(ipv6_str)));
        _print_nc_state(entry);
        _print_nc_type(entry);
        puts("");
    }

    return 0;
}

/**
 * @}
 */
