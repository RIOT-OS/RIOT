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

#include "shell_commands.h"

static void _usage(char **argv);
static int _nib_neigh(int argc, char **argv);
static int _nib_prefix(int argc, char **argv);
static int _nib_route(int argc, char **argv);

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
    else if (strcmp(argv[1], "prefix") == 0) {
        res = _nib_prefix(argc, argv);
    }
    else if (strcmp(argv[1], "route") == 0) {
        res = _nib_route(argc, argv);
    }
    else {
        _usage(argv);
    }
    return res;
}

static void _usage(char **argv)
{
    printf("usage: %s {neigh|prefix|route|help} ...\n", argv[0]);
}

static void _usage_nib_neigh(char **argv)
{
    printf("usage: %s %s [show|add|del|help]\n", argv[0], argv[1]);
    printf("       %s %s add <iface> <ipv6 addr> [<l2 addr>]\n", argv[0], argv[1]);
    printf("       %s %s del <iface> <ipv6 addr>\n", argv[0], argv[1]);
    printf("       %s %s show [iface]\n", argv[0], argv[1]);
}

static void _usage_nib_prefix(char **argv)
{
    printf("usage: %s %s [show|add|del|help]\n", argv[0], argv[1]);
    printf("       %s %s add <iface> <prefix>[/<prefix_len>] [<valid in sec>] [<pref in sec>]\n",
           argv[0], argv[1]);
    printf("       %s %s del <iface> <prefix>[/<prefix_len>]\n", argv[0], argv[1]);
    printf("       %s %s show [iface]\n", argv[0], argv[1]);
}

static void _usage_nib_route(char **argv)
{
    printf("usage: %s %s [show|add|del|help]\n", argv[0], argv[1]);
    printf("       %s %s add <iface> <prefix>[/<prefix_len>] <next_hop> [<ltime in sec>]\n",
           argv[0], argv[1]);
    printf("       %s %s del <iface> <prefix>[/<prefix_len>]\n", argv[0], argv[1]);
    printf("       %s %s show [iface]\n", argv[0], argv[1]);
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
            (l2addr_len = gnrc_netif_addr_from_str(argv[5], l2addr)) == 0) {
            _usage_nib_neigh(argv);
            return 1;
        }
        gnrc_ipv6_nib_nc_set(&ipv6_addr, iface, l2addr, l2addr_len);
    }
    else if ((argc > 3) && (strcmp(argv[2], "del") == 0)) {
        ipv6_addr_t ipv6_addr;
        unsigned iface = atoi(argv[3]);

        if (ipv6_addr_from_str(&ipv6_addr, argv[4]) == NULL) {
            _usage_nib_neigh(argv);
            return 1;
        }
        gnrc_ipv6_nib_nc_del(&ipv6_addr, iface);
    }
    else {
        _usage_nib_neigh(argv);
        return 1;
    }
    return 0;
}

static int _nib_prefix(int argc, char **argv)
{
    if ((argc == 2) || (strcmp(argv[2], "show") == 0)) {
        gnrc_ipv6_nib_pl_t entry;
        void *state = NULL;
        unsigned iface = 0U;

        if (argc > 3) {
            iface = atoi(argv[3]);
        }
        while (gnrc_ipv6_nib_pl_iter(iface, &state, &entry)) {
            gnrc_ipv6_nib_pl_print(&entry);
        }
    }
    else if ((argc > 2) && (strcmp(argv[2], "help") == 0)) {
        _usage_nib_prefix(argv);
    }
    else if ((argc > 4) && (strcmp(argv[2], "add") == 0)) {
        ipv6_addr_t pfx;
        unsigned iface = atoi(argv[3]);
        unsigned pfx_len = ipv6_addr_split_prefix(argv[4]);
        uint32_t valid_ltime = UINT32_MAX, pref_ltime = UINT32_MAX;

        if (ipv6_addr_from_str(&pfx, argv[4]) == NULL) {
            _usage_nib_prefix(argv);
            return 1;
        }
        if (argc > 5) {
            uint32_t ltime_ms = atoi(argv[5]);
            valid_ltime = (ltime_ms > UINT32_MAX / MS_PER_SEC) ?
                          UINT32_MAX - 1 :
                          ltime_ms * MS_PER_SEC;
        }
        if (argc > 6) {
            uint32_t ltime_ms = atoi(argv[6]);
            pref_ltime = (ltime_ms > UINT32_MAX / MS_PER_SEC) ?
                         UINT32_MAX - 1 :
                         ltime_ms * MS_PER_SEC;
        }
        gnrc_ipv6_nib_pl_set(iface, &pfx, pfx_len, valid_ltime, pref_ltime);
    }
    else if ((argc > 4) && (strcmp(argv[2], "del") == 0)) {
        ipv6_addr_t pfx;
        unsigned iface = atoi(argv[3]);
        unsigned pfx_len = ipv6_addr_split_prefix(argv[4]);

        if (ipv6_addr_from_str(&pfx, argv[4]) == NULL) {
            _usage_nib_prefix(argv);
            return 1;
        }
        gnrc_ipv6_nib_pl_del(iface, &pfx, pfx_len);
    }
    else {
        _usage_nib_prefix(argv);
        return 1;
    }
    return 0;
}

static int _nib_route(int argc, char **argv)
{
    if ((argc == 2) || (strcmp(argv[2], "show") == 0)) {
        gnrc_ipv6_nib_ft_t entry;
        void *state = NULL;
        unsigned iface = 0U;

        if (argc > 3) {
            iface = atoi(argv[3]);
        }
        while (gnrc_ipv6_nib_ft_iter(NULL, iface, &state, &entry)) {
            gnrc_ipv6_nib_ft_print(&entry);
        }
    }
    else if ((argc > 2) && (strcmp(argv[2], "help") == 0)) {
        _usage_nib_route(argv);
    }
    else if ((argc > 5) && (strcmp(argv[2], "add") == 0)) {
        ipv6_addr_t pfx = IPV6_ADDR_UNSPECIFIED, next_hop;
        unsigned iface = atoi(argv[3]);
        unsigned pfx_len = ipv6_addr_split_prefix(argv[4]);
        uint16_t ltime = 0;

        if (ipv6_addr_from_str(&pfx, argv[4]) == NULL) {
            /* check if string equals "default"
             * => keep pfx as unspecified address == default route */
            if (strcmp(argv[4], "default") != 0) {
                _usage_nib_route(argv);
                return 1;
            }
        }
        if (ipv6_addr_from_str(&next_hop, argv[5]) == NULL) {
            _usage_nib_route(argv);
            return 1;
        }
        if (argc > 6) {
            ltime = (uint16_t)atoi(argv[6]);
        }
        gnrc_ipv6_nib_ft_add(&pfx, pfx_len, &next_hop, iface, ltime);
    }
    else if ((argc > 4) && (strcmp(argv[2], "del") == 0)) {
        ipv6_addr_t pfx;
        unsigned pfx_len = ipv6_addr_split_prefix(argv[4]);

        if (ipv6_addr_from_str(&pfx, argv[4]) == NULL) {
            _usage_nib_route(argv);
            return 1;
        }
        gnrc_ipv6_nib_ft_del(&pfx, pfx_len);
    }
    else {
        _usage_nib_route(argv);
        return 1;
    }
    return 0;
}

REGISTER_SHELL_COMMAND("nib", "Configure neighbor information base", _gnrc_ipv6_nib);

/** @} */
