/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     test_utils_netdev_eth_minimal
 * @{
 *
 * @file
 * @brief       Shell commands for netdev Eth minimal test utility module
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "net/ethernet/hdr.h"
#include "net/ethertype.h"
#include "net/l2util.h"
#include "byteorder.h"
#include "shell.h"
#include "iolist.h"
#include "assert.h"
#include "netdev_eth_minimal_internal.h"
#include "init_dev.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int ifconfig_list(int idx)
{
    int res;
    netdev_t *dev = _devices[idx].dev;
    uint8_t addr[ETHERNET_ADDR_LEN];
    char addr_str[ETHERNET_ADDR_LEN * 3];

    printf("Iface %3d  HWaddr: ", idx);
    res = dev->driver->get(dev, NETOPT_ADDRESS, addr, sizeof(addr));
    assert(res > 0);
    l2util_addr_to_str(addr, ETHERNET_ADDR_LEN, addr_str);
    printf("%s\n", addr_str);

    return 0;
}

int cmd_ifconfig(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    for (unsigned int i = 0; i < NETDEV_ETH_MINIMAL_NUMOF; i++) {
        ifconfig_list(i);
    }
    return 0;
}

static int _print_txtsnd_usage(char *cmd) {
    printf("usage: %s <iface> <addr> <text>\n", cmd);
    return 1;
}

static int cmd_txtsnd(int argc, char **argv)
{
    ethernet_hdr_t header;
    int res;

    if (4 != argc) {
        return _print_txtsnd_usage(argv[0]);
    }

    int iface = atoi(argv[1]);
    if (iface < 0 || (unsigned)iface >= NETDEV_ETH_MINIMAL_NUMOF) {
        printf("unknown interface %d\n", iface);
        return _print_txtsnd_usage(argv[0]);
    }

    /* build Ethernet header */
    res = l2util_addr_from_str(argv[2], header.dst);
    if (!res) {
        puts("Could not parse address");
        return _print_txtsnd_usage(argv[0]);
    }

    netdev_t *dev = _devices[iface].dev;
    if (!dev) {
        return _print_txtsnd_usage(argv[0]);
    }

    dev->driver->get(dev, NETOPT_ADDRESS, header.src, ETHERNET_ADDR_LEN);
    header.type = byteorder_htons(ETHERTYPE_UNKNOWN);

    /* prepare iolists to send */
    iolist_t io_data = { 0 };
    io_data.iol_base = argv[3];
    io_data.iol_len = strlen(argv[3]);

    iolist_t io_header = { 0 };
    io_header.iol_base = &header;
    io_header.iol_len = sizeof(ethernet_hdr_t);
    io_header.iol_next = &io_data;

    /* send */
    res = dev->driver->send(dev, &io_header);
    if (res < 0) {
        puts("txtsnd: Could not send");
        return 1;
    }

    puts("Successfully sent");
    return 0;
}

SHELL_COMMAND(txtsnd, "Send an Ethernet frame", cmd_txtsnd);
SHELL_COMMAND(ifconfig, "List interfaces", cmd_ifconfig);
