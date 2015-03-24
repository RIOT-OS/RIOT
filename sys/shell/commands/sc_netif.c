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
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "thread.h"
#include "net/ng_netif.h"
#include "net/ng_netapi.h"
#include "net/ng_netconf.h"
#include "net/ng_pkt.h"
#include "net/ng_pktbuf.h"
#include "net/ng_netif/hdr.h"

/**
 * @brief   The maximal expected link layer address length in byte
 */
#define MAX_ADDR_LEN            (8U)

/* utility functions */
static bool _is_number(char *str)
{
    for (; *str; str++) {
        if (*str < '0' || *str > '9') {
            return false;
        }
    }

    return true;
}

static bool _is_iface(kernel_pid_t dev)
{
    size_t numof;
    kernel_pid_t *ifs = ng_netif_get(&numof);

    for (size_t i = 0; i < numof; i++) {
        if (ifs[i] == dev) {
            return true;
        }
    }

    return false;
}

static void _set_usage(char *cmd_name)
{
    printf("usage: %s <if_id> set <key> <value>\n", cmd_name);
    puts("      Sets an hardware specific specific value\n"
         "      <key> may be one of the following\n"
         "       * \"addr\" - sets (short) address\n"
         "       * \"addr_long\" - sets long address\n"
         "       * \"addr_short\" - alias for \"addr\"\n"
         "       * \"channel\" - sets the frequency channel\n"
         "       * \"chan\" - alias for \"channel\""
         "       * \"nid\" - sets the network identifier (or the PAN ID)\n"
         "       * \"pan\" - alias for \"nid\"\n"
         "       * \"pan_id\" - alias for \"nid\"\n"
         "       * \"power\" - TX power in dBm\n"
         "       * \"src_len\" - sets the source address length in byte\n"
         "       * \"state\" - set the device state\n");
}

static size_t _parse_hwaddr(char *str, uint8_t *addr)
{
    char *tok = strtok(str, ":");
    size_t res = 0;

    while (tok != NULL) {
        if (res >= MAX_ADDR_LEN) {
            return 0;
        }

        unsigned int tmp = strtoul(tok, NULL, 16);

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

static void _print_hwaddr(uint8_t *addr, uint8_t addr_len)
{
    for (uint8_t i = 0; i < addr_len; i++) {
        printf("%02x", addr[i]);

        if (i != (addr_len - 1)) {
            printf(":");
        }
    }
}

static void _print_netconf(ng_netconf_opt_t opt)
{
    switch (opt) {
        case NETCONF_OPT_ADDRESS:
            printf("(short) address");
            break;

        case NETCONF_OPT_ADDRESS_LONG:
            printf("long address");
            break;

        case NETCONF_OPT_SRC_LEN:
            printf("source address length");
            break;

        case NETCONF_OPT_CHANNEL:
            printf("channel");
            break;

        case NETCONF_OPT_NID:
            printf("network identifier");
            break;

        case NETCONF_OPT_TX_POWER:
            printf("TX power [in dBm]");
            break;

        default:
            /* we don't serve these options here */
            break;
    }
}

static void _print_netconf_state(ng_netconf_state_t state)
{
    switch (state) {
        case NETCONF_STATE_OFF:
            printf("OFF");
            break;
        case NETCONF_STATE_SLEEP:
            printf("SLEEP");
            break;
        case NETCONF_STATE_IDLE:
            printf("IDLE");
            break;
        case NETCONF_STATE_RX:
            printf("RX");
            break;
        case NETCONF_STATE_TX:
            printf("TX");
            break;
        case NETCONF_STATE_RESET:
            printf("RESET");
            break;
        default:
            /* nothing to do then */
            break;
    }
}

void _netif_list(kernel_pid_t dev)
{
    uint8_t hwaddr[MAX_ADDR_LEN];
    uint16_t u16;
    int16_t i16;
    int res;
    ng_netconf_state_t state;

    printf("Iface %2d  ", dev);

    res = ng_netapi_get(dev, NETCONF_OPT_ADDRESS, 0, hwaddr, sizeof(hwaddr));

    if (res >= 0) {
        printf(" HWaddr: ");
        _print_hwaddr(hwaddr, (uint8_t)res);
        printf(" ");
    }

    res = ng_netapi_get(dev, NETCONF_OPT_CHANNEL, 0, &u16, sizeof(u16));

    if (res >= 0) {
        printf(" Channel: %" PRIu16 " ", u16);
    }

    res = ng_netapi_get(dev, NETCONF_OPT_NID, 0, &u16, sizeof(u16));

    if (res >= 0) {
        printf(" NID: 0x%" PRIx16 " ", u16);
    }

    res = ng_netapi_get(dev, NETCONF_OPT_TX_POWER, 0, &i16, sizeof(i16));

    if (res >= 0) {
        printf(" TX-Power: %" PRIi16 "dBm ", i16);
    }

    res = ng_netapi_get(dev, NETCONF_OPT_STATE, 0, &state, sizeof(state));

    if (res >= 0) {
        printf(" State: ");
        _print_netconf_state(state);
    }

    printf("\n            ");

    res = ng_netapi_get(dev, NETCONF_OPT_ADDRESS_LONG, 0, hwaddr, sizeof(hwaddr));

    if (res >= 0) {
        printf("Long HWaddr: ");
        _print_hwaddr(hwaddr, (uint8_t)res);
        printf("\n            ");
    }

    res = ng_netapi_get(dev, NETCONF_OPT_SRC_LEN, 0, &u16, sizeof(u16));

    if (res >= 0) {
        printf("Source address length: %" PRIu16 "\n            ", u16);
    }

    /* TODO: list IPv6 info */

    puts("");
}

static void _netif_set_u16(kernel_pid_t dev, ng_netconf_opt_t opt,
                           char *u16_str)
{
    unsigned int res;
    bool hex = false;

    if (_is_number(u16_str)) {
        if ((res = strtoul(u16_str, NULL, 10)) == ULONG_MAX) {
            puts("error: unable to parse value.\n"
                 "Must be a 16-bit unsigned integer (dec or hex)\n");
            return;
        }
    }
    else {
        if ((res = strtoul(u16_str, NULL, 16)) == ULONG_MAX) {
            puts("error: unable to parse value.\n"
                 "Must be a 16-bit unsigned integer (dec or hex)\n");
            return;
        }

        hex = true;
    }

    if (res > 0xffff) {
        puts("error: unable to parse value.\n"
             "Must be a 16-bit unsigned integer (dec or hex)\n");
        return;
    }

    if (ng_netapi_set(dev, opt, 0, (uint16_t *)&res, sizeof(uint16_t)) < 0) {
        printf("error: unable to set ");
        _print_netconf(opt);
        puts("");
        return;
    }

    printf("success: set ");
    _print_netconf(opt);
    printf(" on interface %" PRIkernel_pid " to ", dev);

    if (hex) {
        printf("0x%04x\n", res);
    }
    else {
        printf("%u\n", res);
    }
}

static void _netif_set_i16(kernel_pid_t dev, ng_netconf_opt_t opt,
                           char *i16_str)
{
    int16_t val = (int16_t)atoi(i16_str);

    if (ng_netapi_set(dev, opt, 0, (int16_t *)&val, sizeof(int16_t)) < 0) {
        printf("error: unable to set ");
        _print_netconf(opt);
        puts("");
        return;
    }

    printf("success: set ");
    _print_netconf(opt);
    printf(" on interface %" PRIkernel_pid " to %i\n", dev, val);
}

static void _netif_set_addr(kernel_pid_t dev, ng_netconf_opt_t opt,
                            char *addr_str)
{
    uint8_t addr[MAX_ADDR_LEN];
    size_t addr_len = _parse_hwaddr(addr_str, addr);

    if (addr_len == 0) {
        puts("error: unable to parse address.\n"
             "Must be of format [0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*\n"
             "(hex pairs delimited by colons)");
        return;
    }

    if (ng_netapi_set(dev, opt, 0, addr, addr_len) < 0) {
        printf("error: unable to set ");
        _print_netconf(opt);
        puts("");
        return;
    }

    printf("success: set ");
    _print_netconf(opt);
    printf(" on interface %" PRIkernel_pid " to ", dev);
    _print_hwaddr(addr, addr_len);
    puts("");
}

static void _netif_set_state(kernel_pid_t dev, char *state_str)
{
    ng_netconf_state_t state;
    if ((strcmp("off", state_str) == 0) || (strcmp("OFF", state_str) == 0)) {
        state = NETCONF_STATE_OFF;
    }
    else if ((strcmp("sleep", state_str) == 0) ||
             (strcmp("SLEEP", state_str) == 0)) {
        state = NETCONF_STATE_SLEEP;
    }
    else if ((strcmp("idle", state_str) == 0) ||
             (strcmp("IDLE", state_str) == 0)) {
        state = NETCONF_STATE_IDLE;
    }
    else if ((strcmp("reset", state_str) == 0) ||
             (strcmp("RESET", state_str) == 0)) {
        state = NETCONF_STATE_RESET;
    }
    else {
        puts("usage: ifconfig <if_id> set state [off|sleep|idle|reset]");
        return;
    }
    if (ng_netapi_set(dev, NETCONF_OPT_STATE, 0,
                      &state, sizeof(ng_netconf_state_t)) < 0) {
        printf("error: unable to set state to ");
        _print_netconf_state(state);
        puts("");
        return;
    }
    printf("success: set state of interface %" PRIkernel_pid " to ", dev);
    _print_netconf_state(state);
    puts("");
}

static void _netif_set(char *cmd_name, kernel_pid_t dev, char *key, char *value)
{
    if ((strcmp("addr", key) == 0) || (strcmp("addr_short", key) == 0)) {
        _netif_set_addr(dev, NETCONF_OPT_ADDRESS, value);
    }
    else if (strcmp("addr_long", key) == 0) {
        _netif_set_addr(dev, NETCONF_OPT_ADDRESS_LONG, value);
    }
    else if ((strcmp("channel", key) == 0) || (strcmp("chan", key) == 0)) {
        _netif_set_u16(dev, NETCONF_OPT_CHANNEL, value);
    }
    else if ((strcmp("nid", key) == 0) || (strcmp("pan", key) == 0) ||
             (strcmp("pan_id", key) == 0)) {
        _netif_set_u16(dev, NETCONF_OPT_NID, value);
    }
    else if (strcmp("power", key) == 0) {
        _netif_set_i16(dev, NETCONF_OPT_TX_POWER, value);
    }
    else if (strcmp("src_len", key) == 0) {
        _netif_set_u16(dev, NETCONF_OPT_SRC_LEN, value);
    }
    else if (strcmp("state", key) == 0) {
        _netif_set_state(dev, value);
    }
    else {
        _set_usage(cmd_name);
    }
}

/* shell commands */
void _netif_send(int argc, char **argv)
{
    kernel_pid_t dev;
    uint8_t addr[MAX_ADDR_LEN];
    size_t addr_len;
    ng_pktsnip_t *pkt;
    ng_netif_hdr_t *nethdr;


    if (argc < 4) {
        printf("usage: %s <if> <addr> <data>\n", argv[0]);
        return;
    }

    /* parse interface */
    dev = (kernel_pid_t)atoi(argv[1]);

    if (!_is_iface(dev)) {
        puts("error: invalid interface given");
        return;
    }

    /* parse address */
    addr_len = _parse_hwaddr(argv[2], addr);

    if (addr_len == 0) {
        puts("error: invalid address given");
        return;
    }

    /* put packet together */
    pkt = ng_pktbuf_add(NULL, argv[3], strlen(argv[3]), NG_NETTYPE_UNDEF);
    pkt = ng_pktbuf_add(pkt, NULL, sizeof(ng_netif_hdr_t) + addr_len,
                        NG_NETTYPE_NETIF);
    nethdr = (ng_netif_hdr_t *)pkt->data;
    ng_netif_hdr_init(nethdr, 0, addr_len);
    ng_netif_hdr_set_dst_addr(nethdr, addr, addr_len);
    /* and send it */
    ng_netapi_send(dev, pkt);
}

void _netif_config(int argc, char **argv)
{
    if (argc < 2) {
        size_t numof;
        kernel_pid_t *ifs = ng_netif_get(&numof);

        for (size_t i = 0; i < numof; i++) {
            _netif_list(ifs[i]);
            return;
        }
    }
    else if (_is_number(argv[1])) {
        kernel_pid_t dev = (kernel_pid_t)atoi(argv[1]);

        if (_is_iface(dev)) {
            if (argc < 3) {
                _netif_list(dev);
                return;
            }
            else if (strcmp(argv[2], "set") == 0) {
                if (argc < 5) {
                    _set_usage(argv[0]);
                    return;
                }

                _netif_set(argv[0], dev, argv[3], argv[4]);
                return;
            }

            /* TODO implement add for IP addresses */
        }
        else {
            puts("error: invalid interface given");
        }
    }

    printf("usage: %s [<if_id> set <key> <value>]]\n", argv[0]);
}
