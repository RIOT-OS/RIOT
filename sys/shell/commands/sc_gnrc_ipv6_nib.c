/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdio.h>

#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/netif.h"
#include "net/ipv6/addr.h"

static void _usage(char **argv);
static int _nib_neigh(int argc, char **argv);

int _gnrc_ipv6_nib(int argc, char **argv)
{
    int res = 1;

    if ((argc < 2) || (strcmp(argv[1], "help") == 0)) {
        _usage(argv);
        res = 0;
    }
    else if (strcmp(argv[1], "neigh") == 0) {
        res = _nib_neigh(argc, argv);
    }
    else {
        _usage(argv);
    }
    return res;
}

static void _usage(char **argv)
{
    printf("usage: %s {neigh|help} ...\n", argv[0]);
}

static void _usage_nib_neigh(char **argv)
{
    printf("usage: %s %s [show|add|del|help]\n", argv[0], argv[1]);
    printf("       %s %s add <iface> <ipv6 addr> [<l2 addr>]\n", argv[0], argv[1]);
    printf("       %s %s del <ipv6 addr>\n", argv[0], argv[1]);
    printf("       %s %s show <ipv6 addr>\n", argv[0], argv[1]);
}

static int _nib_neigh(int argc, char **argv)
{
    if ((argc == 2) || (strcmp(argv[2], "show") == 0)) {
        gnrc_ipv6_nib_nc_t entry;
        void *state = NULL;
        unsigned iface = 0U;

        if (argc > 3) {
            iface = atoi(argv[3]);
        }
        while (gnrc_ipv6_nib_nc_iter(iface, &state, &entry)) {
            gnrc_ipv6_nib_nc_print(&entry);
        }
    }
    else if ((argc > 2) && (strcmp(argv[2], "help") == 0)) {
        _usage_nib_neigh(argv);
    }
    else if ((argc > 4) && (strcmp(argv[2], "add") == 0)) {
        ipv6_addr_t ipv6_addr;
        uint8_t l2addr[GNRC_IPV6_NIB_L2ADDR_MAX_LEN];
        size_t l2addr_len = 0;
        unsigned iface = atoi(argv[3]);

        if (ipv6_addr_from_str(&ipv6_addr, argv[4]) == NULL) {
            _usage_nib_neigh(argv);
            return 1;
        }
        if ((argc > 5) && /* TODO also check if interface supports link-layers or not */
            (l2addr_len = gnrc_netif_addr_from_str(l2addr, sizeof(l2addr),
                                                   argv[5])) == 0) {
            _usage_nib_neigh(argv);
            return 1;
        }
        gnrc_ipv6_nib_nc_set(&ipv6_addr, iface, l2addr, l2addr_len);
    }
    else if ((argc > 3) && (strcmp(argv[2], "del") == 0)) {
        ipv6_addr_t ipv6_addr;

        if (ipv6_addr_from_str(&ipv6_addr, argv[4]) == NULL) {
            _usage_nib_neigh(argv);
            return 1;
        }
        gnrc_ipv6_nib_nc_del(&ipv6_addr);
    }
    else {
        _usage_nib_neigh(argv);
        return 1;
    }
    return 0;
}

/** @} */
