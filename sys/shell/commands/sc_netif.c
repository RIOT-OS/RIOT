/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Shell commands for interacting with network devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "thread.h"
#include "net/ng_netif.h"
#include "net/ng_netapi.h"
#include "net/ng_netconf.h"
#include "net/ng_pktbuf.h"
#include "net/ng_netif/hdr.h"

/**
 * @brief   The maximal expected link layer address length in byte
 */
#define MAX_ADDR_LEN            (16U)


/* utility functions */
uint8_t _parse_addr(char *str, uint8_t *addr)
{
    char *tok = strtok(str, ":");
    uint8_t res = 0;

    while (tok != NULL) {
        if (res >= MAX_ADDR_LEN) {
            return 0;
        }
        uint16_t tmp = (uint16_t)strtol(tok, NULL, 16);
        if (tmp <= 0xff) {
            addr[res++] = (uint8_t)tmp;
        }
        else {
            return 0;
        }
        tok = strtok(NULL, ":");
    }
    return res;
}

void _print_addr(uint8_t *addr, uint8_t addr_len)
{
    for (uint8_t i = 0; i < addr_len; i++) {
        printf("%02x", addr[i]);
        if (i != (addr_len - 1)) {
            printf(":");
        }
    }
}

/* shell commands */
void _netif_list(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    kernel_pid_t *ifs;
    size_t numof;

    /* read available interfaces */
    ifs = ng_netif_get(&numof);

    /* print information about each interface */
    for (size_t i = 0; i < numof; i++) {
        /* TODO: print more information about the devices */
        printf("%3i - \n", ifs[i]);
    }
}

void _netif_send(int argc, char **argv)
{
    kernel_pid_t dev;
    uint8_t addr[MAX_ADDR_LEN];
    uint8_t addr_len;
    ng_pktsnip_t *pkt;
    ng_netif_hdr_t *nethdr;


    if (argc < 4) {
        printf("usage: %s IF ADDR DATA\n", argv[0]);
        return;
    }
    /* parse interface */
    dev = (kernel_pid_t)atoi(argv[1]);
    if (thread_get(dev) == NULL) {
        puts("error: invalid interface given");
        return;
    }
    /* parse address */
    addr_len = _parse_addr(argv[2], addr);
    if (addr_len == 0) {
        puts("error: invalid address given");
        return;
    }
    /* put packet together */
    pkt = ng_pktbuf_add(NULL, argv[3], strlen(argv[3]), NG_NETTYPE_UNDEF);
    pkt = ng_pktbuf_add(pkt, NULL, sizeof(ng_netif_hdr_t) + addr_len,
                     NG_NETTYPE_UNDEF);
    nethdr = (ng_netif_hdr_t *)pkt->data;
    ng_netif_hdr_init(nethdr, 0, addr_len);
    ng_netif_hdr_set_dst_addr(nethdr, addr, addr_len);
    /* and send it */
    ng_netapi_send(dev, pkt);
}

void _netif_addr(int argc, char **argv)
{
    kernel_pid_t dev;
    uint8_t addr[MAX_ADDR_LEN];
    size_t addr_len;
    int res;

    if (argc < 2) {
        printf("usage: %s IF [ADDR]\n", argv[0]);
        return;
    }
    /* parse interface */
    dev = (kernel_pid_t)atoi(argv[1]);
    if (thread_get(dev) == NULL) {
        puts("error: invalid interface given");
        return;
    }
    /* if address was given, parse and set it */
    if (argc >= 3) {
        addr_len = _parse_addr(argv[2], addr);
        if (addr_len == 0) {
            puts("error: unable to parse address");
        }
        else {
            res = ng_netapi_set(dev, NETCONF_OPT_ADDRESS, 0,
                                     addr, addr_len);
            if (res > 0) {
                printf("success: address of interface %i set to ", dev);
                _print_addr(addr, addr_len);
                puts("");
            }
            else {
                puts("error: could not set address\n");
            }
        }
    }
    /* otherwise read it */
    else {
        res = ng_netapi_get(dev, NETCONF_OPT_ADDRESS, 0, addr, MAX_ADDR_LEN);
        if (res > 0) {
            addr_len = (size_t)res;
            printf("address of interface %i is ", dev);
            _print_addr(addr, addr_len);
            puts("");
        }
        else {
            puts("error: unable to read address");
        }
    }
}

void _netif_chan(int argc, char **argv)
{
    kernel_pid_t dev;
    uint16_t chan;
    int res;

    if (argc < 2) {
        printf("usage: %s IF [CHAN]\n", argv[0]);
        return;
    }
    /* parse interface */
    dev = (kernel_pid_t)atoi(argv[1]);
    if (thread_get(dev) == NULL) {
        puts("error: invalid interface given");
        return;
    }
    /* if channel was given, parse and set it */
    if (argc >= 3) {
        chan = (uint16_t)atoi(argv[2]);
        res = ng_netapi_set(dev, NETCONF_OPT_CHANNEL, 0, &chan, 2);
        if (res > 0) {
            printf("success: channel of interface %i set to %u\n", dev, chan);
        }
        else {
            puts("error: could not set channel\n");
        }
    }
    /* otherwise read it */
    else {
        res = ng_netapi_get(dev, NETCONF_OPT_CHANNEL, 0, &chan, 2);
        if (res > 0) {
            printf("channel of interface %i is %u\n", dev, chan);
        }
        else {
            puts("error: unable to read channel");
        }
    }
}

void _netif_pan(int argc, char **argv)
{
    kernel_pid_t dev;
    uint16_t pan;
    int res;

    if (argc < 2) {
        printf("usage: %s IF [CHAN]\n", argv[0]);
        return;
    }
    /* parse interface */
    dev = (kernel_pid_t)atoi(argv[1]);
    if (thread_get(dev) == NULL) {
        puts("error: invalid interface given");
        return;
    }
    /* if PAN was given, parse and set it */
    if (argc >= 3) {
        pan = (uint16_t)atoi(argv[2]);
        res = ng_netapi_set(dev, NETCONF_OPT_NID, 0, &pan, 2);
        if (res > 0) {
            printf("success: PAN of interface %i set to %u\n", dev, pan);
        }
        else {
            puts("error: could not set PAN\n");
        }
    }
    /* otherwise read it */
    else {
        res = ng_netapi_get(dev, NETCONF_OPT_NID, 0, &pan, 2);
        if (res > 0) {
            printf("PAN of interface %i is %u\n", dev, pan);
        }
        else {
            puts("error: unable to read PAN");
        }
    }

}
