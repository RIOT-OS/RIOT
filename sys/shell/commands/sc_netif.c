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

static void _flag_usage(char *cmd_name)
{
    printf("usage: %s <if_id> [-]{promisc|autoack|preload}", cmd_name);
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

static void _netif_list(kernel_pid_t dev)
{
    uint8_t hwaddr[MAX_ADDR_LEN];
    uint16_t u16;
    int16_t i16;
    int res;
    ng_netconf_state_t state;
    ng_netconf_enable_t enable;
    bool linebreak = false;

    printf("Iface %2d  ", dev);

    res = ng_netapi_get(dev, NETCONF_OPT_ADDRESS, 0, hwaddr, sizeof(hwaddr));

    if (res >= 0) {
        char hwaddr_str[res * 3];
        printf(" HWaddr: ");
        printf("%s", ng_netif_addr_to_str(hwaddr_str, sizeof(hwaddr_str),
                                          hwaddr, res));
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

    printf("\n           ");

    res = ng_netapi_get(dev, NETCONF_OPT_ADDRESS_LONG, 0, hwaddr, sizeof(hwaddr));

    if (res >= 0) {
        char hwaddr_str[res * 3];
        printf("Long HWaddr: ");
        printf("%s", ng_netif_addr_to_str(hwaddr_str, sizeof(hwaddr_str),
                                          hwaddr, res));
        printf("\n           ");
    }

    res = ng_netapi_get(dev, NETCONF_OPT_PROMISCUOUSMODE, 0, &enable, sizeof(enable));

    if ((res >= 0) && (enable == NETCONF_ENABLE)) {
        printf("PROMISC  ");
        linebreak = true;
    }

    res = ng_netapi_get(dev, NETCONF_OPT_AUTOACK, 0, &enable, sizeof(enable));

    if ((res >= 0) && (enable == NETCONF_ENABLE)) {
        printf("AUTOACK  ");
        linebreak = true;
    }

    res = ng_netapi_get(dev, NETCONF_OPT_PRELOADING, 0, &enable, sizeof(enable));

    if ((res >= 0) && (enable == NETCONF_ENABLE)) {
        printf("PRELOAD  ");
        linebreak = true;
    }

    if (linebreak) {
        printf("\n           ");
    }

    res = ng_netapi_get(dev, NETCONF_OPT_SRC_LEN, 0, &u16, sizeof(u16));

    if (res >= 0) {
        printf("Source address length: %" PRIu16 "\n            ", u16);
    }

    /* TODO: list IPv6 info */

    puts("");
}

static int _netif_set_u16(kernel_pid_t dev, ng_netconf_opt_t opt,
                          char *u16_str)
{
    unsigned int res;
    bool hex = false;

    if (_is_number(u16_str)) {
        if ((res = strtoul(u16_str, NULL, 10)) == ULONG_MAX) {
            puts("error: unable to parse value.\n"
                 "Must be a 16-bit unsigned integer (dec or hex)\n");
            return 1;
        }
    }
    else {
        if ((res = strtoul(u16_str, NULL, 16)) == ULONG_MAX) {
            puts("error: unable to parse value.\n"
                 "Must be a 16-bit unsigned integer (dec or hex)\n");
            return 1;
        }

        hex = true;
    }

    if (res > 0xffff) {
        puts("error: unable to parse value.\n"
             "Must be a 16-bit unsigned integer (dec or hex)\n");
        return 1;
    }

    if (ng_netapi_set(dev, opt, 0, (uint16_t *)&res, sizeof(uint16_t)) < 0) {
        printf("error: unable to set ");
        _print_netconf(opt);
        puts("");
        return 1;
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

    return 0;
}

static int _netif_set_i16(kernel_pid_t dev, ng_netconf_opt_t opt,
                          char *i16_str)
{
    int16_t val = (int16_t)atoi(i16_str);

    if (ng_netapi_set(dev, opt, 0, (int16_t *)&val, sizeof(int16_t)) < 0) {
        printf("error: unable to set ");
        _print_netconf(opt);
        puts("");
        return 1;
    }

    printf("success: set ");
    _print_netconf(opt);
    printf(" on interface %" PRIkernel_pid " to %i\n", dev, val);

    return 0;
}

static int _netif_set_flag(kernel_pid_t dev, ng_netconf_opt_t opt,
                           ng_netconf_enable_t set)
{
    if (ng_netapi_set(dev, opt, 0, &set, sizeof(ng_netconf_enable_t)) < 0) {
        puts("error: unable to set option");
        return 1;
    }
    printf("success: %sset option\n", (set) ? "" : "un");
    return 0;
}

static int _netif_set_addr(kernel_pid_t dev, ng_netconf_opt_t opt,
                           char *addr_str)
{
    uint8_t addr[MAX_ADDR_LEN];
    size_t addr_len = ng_netif_addr_from_str(addr, sizeof(addr), addr_str);

    if (addr_len == 0) {
        puts("error: unable to parse address.\n"
             "Must be of format [0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*\n"
             "(hex pairs delimited by colons)");
        return 1;
    }

    if (ng_netapi_set(dev, opt, 0, addr, addr_len) < 0) {
        printf("error: unable to set ");
        _print_netconf(opt);
        puts("");
        return 1;
    }

    printf("success: set ");
    _print_netconf(opt);
    printf(" on interface %" PRIkernel_pid " to %s\n", dev, addr_str);

    return 0;
}

static int _netif_set_state(kernel_pid_t dev, char *state_str)
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
        return 1;
    }
    if (ng_netapi_set(dev, NETCONF_OPT_STATE, 0,
                      &state, sizeof(ng_netconf_state_t)) < 0) {
        printf("error: unable to set state to ");
        _print_netconf_state(state);
        puts("");
        return 1;
    }
    printf("success: set state of interface %" PRIkernel_pid " to ", dev);
    _print_netconf_state(state);
    puts("");

    return 0;
}

static int _netif_set(char *cmd_name, kernel_pid_t dev, char *key, char *value)
{
    if ((strcmp("addr", key) == 0) || (strcmp("addr_short", key) == 0)) {
        return _netif_set_addr(dev, NETCONF_OPT_ADDRESS, value);
    }
    else if (strcmp("addr_long", key) == 0) {
        return _netif_set_addr(dev, NETCONF_OPT_ADDRESS_LONG, value);
    }
    else if ((strcmp("channel", key) == 0) || (strcmp("chan", key) == 0)) {
        return _netif_set_u16(dev, NETCONF_OPT_CHANNEL, value);
    }
    else if ((strcmp("nid", key) == 0) || (strcmp("pan", key) == 0) ||
             (strcmp("pan_id", key) == 0)) {
        return _netif_set_u16(dev, NETCONF_OPT_NID, value);
    }
    else if (strcmp("power", key) == 0) {
        return _netif_set_i16(dev, NETCONF_OPT_TX_POWER, value);
    }
    else if (strcmp("src_len", key) == 0) {
        return _netif_set_u16(dev, NETCONF_OPT_SRC_LEN, value);
    }
    else if (strcmp("state", key) == 0) {
        return _netif_set_state(dev, value);
    }

    _set_usage(cmd_name);
    return 1;
}

static int _netif_flag(char *cmd, kernel_pid_t dev, char *flag)
{
    ng_netconf_enable_t set = NETCONF_ENABLE;

    if (flag[0] == '-') {
        set = NETCONF_DISABLE;
        flag++;
    }

    if (strcmp(flag, "promisc") == 0) {
        return _netif_set_flag(dev, NETCONF_OPT_PROMISCUOUSMODE, set);
    }
    else if (strcmp(flag, "preload") == 0) {
        return _netif_set_flag(dev, NETCONF_OPT_PRELOADING, set);
    }
    else if (strcmp(flag, "autoack") == 0) {
        return _netif_set_flag(dev, NETCONF_OPT_AUTOACK, set);
    }

    _flag_usage(cmd);
    return 1;
}

/* shell commands */
int _netif_send(int argc, char **argv)
{
    kernel_pid_t dev;
    uint8_t addr[MAX_ADDR_LEN];
    size_t addr_len;
    ng_pktsnip_t *pkt;
    ng_netif_hdr_t *nethdr;
    uint8_t flags = 0x00;

    if (argc < 4) {
        printf("usage: %s <if> [<addr>|bcast] <data>\n", argv[0]);
        return 1;
    }

    /* parse interface */
    dev = (kernel_pid_t)atoi(argv[1]);

    if (!_is_iface(dev)) {
        puts("error: invalid interface given");
        return 1;
    }

    /* parse address */
    addr_len = ng_netif_addr_from_str(addr, sizeof(addr), argv[2]);

    if (addr_len == 0) {
        if (strcmp(argv[2], "bcast") == 0) {
            flags |= NG_NETIF_HDR_FLAGS_BROADCAST;
        }
        else {
            puts("error: invalid address given");
            return 1;
        }
    }

    /* put packet together */
    pkt = ng_pktbuf_add(NULL, argv[3], strlen(argv[3]), NG_NETTYPE_UNDEF);
    pkt = ng_pktbuf_add(pkt, NULL, sizeof(ng_netif_hdr_t) + addr_len,
                        NG_NETTYPE_NETIF);
    nethdr = (ng_netif_hdr_t *)pkt->data;
    ng_netif_hdr_init(nethdr, 0, addr_len);
    ng_netif_hdr_set_dst_addr(nethdr, addr, addr_len);
    nethdr->flags = flags;
    /* and send it */
    ng_netapi_send(dev, pkt);

    return 0;
}

int _netif_config(int argc, char **argv)
{
    if (argc < 2) {
        size_t numof;
        kernel_pid_t *ifs = ng_netif_get(&numof);

        for (size_t i = 0; i < numof; i++) {
            _netif_list(ifs[i]);
        }

        return 0;
    }
    else if (_is_number(argv[1])) {
        kernel_pid_t dev = (kernel_pid_t)atoi(argv[1]);

        if (_is_iface(dev)) {
            if (argc < 3) {
                _netif_list(dev);
                return 0;
            }
            else if (strcmp(argv[2], "set") == 0) {
                if (argc < 5) {
                    _set_usage(argv[0]);
                    return 1;
                }

                return _netif_set(argv[0], dev, argv[3], argv[4]);
            }

            /* TODO implement add for IP addresses */

            else {
                return _netif_flag(argv[0], dev, argv[2]);
            }
        }
        else {
            puts("error: invalid interface given");
            return 1;
        }
    }

    printf("usage: %s <if_id>\n", argv[0]);
    _set_usage(argv[0]);
    _flag_usage(argv[0]);
    return 1;
}
