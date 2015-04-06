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

#include <string.h>

#include "kernel_types.h"
#include "net/ng_ipv6/addr.h"
#include "net/ng_ipv6/nc.h"
#include "net/ng_netif.h"
#include "thread.h"

/* maximum length of L2 address */
#define MAX_L2_ADDR_LEN (8U)

static void _print_nc_state(ng_ipv6_nc_t *entry)
{
    switch (entry->flags & NG_IPV6_NC_STATE_MASK) {
        case NG_IPV6_NC_STATE_UNMANAGED:
            printf("UNMANAGED");
            break;

        case NG_IPV6_NC_STATE_UNREACHABLE:
            printf("UNREACHABLE");
            break;

        case NG_IPV6_NC_STATE_INCOMPLETE:
            printf("INCOMPLETE");
            break;

        case NG_IPV6_NC_STATE_STALE:
            printf("STALE");
            break;

        case NG_IPV6_NC_STATE_DELAY:
            printf("DELAY");
            break;

        case NG_IPV6_NC_STATE_PROBE:
            printf("PROBE");
            break;

        case NG_IPV6_NC_STATE_REACHABLE:
            printf("REACHABLE");
            break;

        default:
            printf("UNKNOWN");
            break;
    }
}

static void _print_nc_type(ng_ipv6_nc_t *entry)
{
    switch (entry->flags & NG_IPV6_NC_TYPE_MASK) {
        case NG_IPV6_NC_TYPE_GC:
            printf("GC");
            break;

        case NG_IPV6_NC_TYPE_TENTATIVE:
            printf("T");
            break;

        case NG_IPV6_NC_TYPE_REGISTERED:
            printf("R");
            break;

        default:
            printf("-");
            break;
    }
}

static bool _is_iface(kernel_pid_t iface)
{
#ifdef MODULE_NG_NETIF
    size_t numof;
    kernel_pid_t *ifs = ng_netif_get(&numof);

    for (size_t i = 0; i < numof; i++) {
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
    char ipv6_str[NG_IPV6_ADDR_MAX_STR_LEN];
    char l2addr_str[3 * MAX_L2_ADDR_LEN];

    puts("IPv6 address                    if  L2 address                state      type");
    puts("-----------------------------------------------------------------------------");

    for (ng_ipv6_nc_t *entry = ng_ipv6_nc_get_next(NULL);
         entry != NULL;
         entry = ng_ipv6_nc_get_next(entry)) {
        printf("%-30s  %2" PRIkernel_pid "  %-24s  ",
               ng_ipv6_addr_to_str(ipv6_str, &entry->ipv6_addr, sizeof(ipv6_str)),
               entry->iface,
               ng_netif_addr_to_str(l2addr_str, sizeof(l2addr_str),
                                    entry->l2_addr, entry->l2_addr_len));
        _print_nc_state(entry);
        _print_nc_type(entry);
        puts("");
    }

    return 0;
}

static int _ipv6_nc_add(kernel_pid_t iface, char *ipv6_addr_str,
                        char *l2_addr_str)
{
    ng_ipv6_addr_t ipv6_addr;
    uint8_t l2_addr[MAX_L2_ADDR_LEN];
    size_t l2_addr_len;

    if (ng_ipv6_addr_from_str(&ipv6_addr, ipv6_addr_str) == NULL) {
        puts("error: unable to parse IPv6 address.");
        return 1;
    }

    if ((l2_addr_len = ng_netif_addr_from_str(l2_addr, sizeof(l2_addr),
                       l2_addr_str)) == 0) {
        puts("error: unable to parse link-layer address.");
        return 1;
    }

    if (ng_ipv6_nc_add(iface, &ipv6_addr, l2_addr, l2_addr_len, 0) < 0) {
        puts("error: unable to add address to neighbor cache.");
        return 1;
    }

    printf("success: added IPv6 address %s to neighbor cache\n", ipv6_addr_str);
    return 0;
}

static int _ipv6_nc_del(char *ipv6_addr_str)
{
    ng_ipv6_addr_t ipv6_addr;

    if (ng_ipv6_addr_from_str(&ipv6_addr, ipv6_addr_str) == NULL) {
        puts("error: unable to parse IPv6 address.");
        return 1;
    }

    ng_ipv6_nc_remove(KERNEL_PID_UNDEF, &ipv6_addr);
    printf("success: removed %s from neighbor cache\n", ipv6_addr_str);

    return 0;
}

int _ipv6_nc_manage(int argc, char **argv)
{
    if ((argc == 1) || (strcmp("list", argv[1]) == 0)) {
        return _ipv6_nc_list();
    }

    if (argc > 2) {
        if ((argc > 4) && (strcmp("add", argv[1]) == 0)) {
            kernel_pid_t iface = (kernel_pid_t)atoi(argv[2]);

            if (!_is_iface(iface)) {
                puts("error: invalid interface given.");
                return 1;
            }

            return _ipv6_nc_add(iface, argv[3], argv[4]);
        }

        if (strcmp("del", argv[1]) == 0) {
            return _ipv6_nc_del(argv[2]);
        }
    }

    printf("usage: %s [list]\n"
           "   or: %s add <iface pid> <ipv6_addr> <l2_addr>\n"
           "   or: %s del <ipv6_addr>\n", argv[0], argv[0], argv[0]);
    return 1;
}

int _ipv6_nc_routers(int argc, char **argv)
{
    kernel_pid_t iface = KERNEL_PID_UNDEF;
    char ipv6_str[NG_IPV6_ADDR_MAX_STR_LEN];

    if (argc > 1) {
        iface = atoi(argv[1]);

        if (!_is_iface(iface)) {
            printf("usage: %s [<iface pid>]\n", argv[0]);
            return 1;
        }
    }

    puts("if  Router                          state      type");
    puts("---------------------------------------------------");

    for (ng_ipv6_nc_t *entry = ng_ipv6_nc_get_next_router(NULL);
         entry != NULL;
         entry = ng_ipv6_nc_get_next_router(entry)) {
        if ((iface != KERNEL_PID_UNDEF) && (iface != entry->iface)) {
            continue;
        }

        printf("%2" PRIkernel_pid "  %-30s  ", entry->iface,
               ng_ipv6_addr_to_str(ipv6_str, &entry->ipv6_addr, sizeof(ipv6_str)));
        _print_nc_state(entry);
        _print_nc_type(entry);
        puts("");
    }

    return 0;
}

/**
 * @}
 */
