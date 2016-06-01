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
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "thread.h"
#include "net/netstats.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/netif.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netapi.h"
#include "net/netopt.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/sixlowpan/netif.h"
#include "net/netdev2/ieee802154.h"

/**
 * @brief   The maximal expected link layer address length in byte
 */
#define MAX_ADDR_LEN            (8U)

/**
 * @brief   The default IPv6 prefix length if not specified.
 */
#define SC_iwpan_IPV6_DEFAULT_PREFIX_LEN     (64)

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
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    size_t numof = gnrc_netif_get(ifs);

    for (size_t i = 0; i < numof && i < GNRC_NETIF_NUMOF; i++) {
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
         "       * \"chan\" - alias for \"channel\"\n"
         "       * \"csma_retries\" - set max. number of channel access attempts\n"
         "       * \"cca_threshold\" - set ED threshold during CCA in dBm\n"
         "       * \"cca_mode\" - set CCA mode\n"
         "       * \"nid\" - sets the network identifier (or the PAN ID)\n"
         "       * \"page\" - set the channel page (IEEE 802.15.4)\n"
         "       * \"pan\" - alias for \"nid\"\n"
         "       * \"pan_id\" - alias for \"nid\"\n"
         "       * \"power\" - TX power in dBm\n"
         "       * \"retrans\" - max. number of retransmissions\n"
         "       * \"src_len\" - sets the source address length in byte\n"
         "       * \"state\" - set the device state\n"
         "       * \"encrypt\" - set the encryption on-off\n"
         "       * \"key\" - set the encryption key in hexadecimal format\n");
}

static void _flag_usage(char *cmd_name)
{
    printf("usage: %s <if_id> [-]{promisc|autoack|csma|autocca|preload|iphc|rtr_adv}\n", cmd_name);
}

static void _print_netopt(netopt_t opt)
{
    switch (opt) {
        case NETOPT_ADDRESS:
            printf("(short) address");
            break;

        case NETOPT_ADDRESS_LONG:
            printf("long address");
            break;

        case NETOPT_SRC_LEN:
            printf("source address length");
            break;

        case NETOPT_CHANNEL:
            printf("channel");
            break;

        case NETOPT_CHANNEL_PAGE:
            printf("page");
            break;

        case NETOPT_NID:
            printf("network identifier");
            break;

        case NETOPT_TX_POWER:
            printf("TX power [in dBm]");
            break;

        case NETOPT_RETRANS:
            printf("max. retransmissions");
            break;

        case NETOPT_CSMA_RETRIES:
            printf("CSMA retries");
            break;

        case NETOPT_CCA_THRESHOLD:
            printf("CCA threshold [in dBm]");
            break;

        case NETOPT_CCA_MODE:
            printf("CCA mode");
            break;

        case NETOPT_ENCRYPTION:
            printf("encryption");
            break;

        case NETOPT_ENCRYPTION_KEY:
            printf("encryption key");
            break;

        default:
            /* we don't serve these options here */
            break;
    }
}

static void _print_netopt_state(netopt_state_t state)
{
    switch (state) {
        case NETOPT_STATE_OFF:
            printf("OFF");
            break;
        case NETOPT_STATE_SLEEP:
            printf("SLEEP");
            break;
        case NETOPT_STATE_IDLE:
            printf("IDLE");
            break;
        case NETOPT_STATE_RX:
            printf("RX");
            break;
        case NETOPT_STATE_TX:
            printf("TX");
            break;
        case NETOPT_STATE_RESET:
            printf("RESET");
            break;
        default:
            /* nothing to do then */
            break;
    }
}

static void _iwpan_list(kernel_pid_t dev)
{
    uint8_t hwaddr[MAX_ADDR_LEN];
    uint16_t u16;
    int16_t i16;
    uint8_t u8;
    int res;
    netopt_state_t state;
    netopt_enable_t enable = NETOPT_DISABLE;
    bool linebreak = false;

    printf("Iface %2d  ", dev);

    res = gnrc_netapi_get(dev, NETOPT_ADDRESS, 0, hwaddr, sizeof(hwaddr));

    if (res >= 0) {
        char hwaddr_str[res * 3];
        printf(" HWaddr: ");
        printf("%s", gnrc_netif_addr_to_str(hwaddr_str, sizeof(hwaddr_str),
                                            hwaddr, res));
        printf(" ");
    }

    res = gnrc_netapi_get(dev, NETOPT_CHANNEL, 0, &u16, sizeof(u16));

    if (res >= 0) {
        printf(" Channel: %" PRIu16 " ", u16);
    }

    res = gnrc_netapi_get(dev, NETOPT_CHANNEL_PAGE, 0, &u16, sizeof(u16));

    if (res >= 0) {
        printf(" Page: %" PRIu16 " ", u16);
    }

    res = gnrc_netapi_get(dev, NETOPT_NID, 0, &u16, sizeof(u16));

    if (res >= 0) {
        printf(" NID: 0x%" PRIx16, u16);
    }

    printf("\n           ");

    res = gnrc_netapi_get(dev, NETOPT_ADDRESS_LONG, 0, hwaddr, sizeof(hwaddr));

    if (res >= 0) {
        char hwaddr_str[res * 3];
        printf("Long HWaddr: ");
        printf("%s ", gnrc_netif_addr_to_str(hwaddr_str, sizeof(hwaddr_str),
                                             hwaddr, res));
        linebreak = true;
    }

    if (linebreak) {
        printf("\n          ");
    }

    res = gnrc_netapi_get(dev, NETOPT_TX_POWER, 0, &i16, sizeof(i16));

    if (res >= 0) {
        printf(" TX-Power: %" PRIi16 "dBm ", i16);
    }

    res = gnrc_netapi_get(dev, NETOPT_STATE, 0, &state, sizeof(state));

    if (res >= 0) {
        printf(" State: ");
        _print_netopt_state(state);
        printf(" ");
    }

    res = gnrc_netapi_get(dev, NETOPT_RETRANS, 0, &u8, sizeof(u8));

    if (res >= 0) {
        printf(" max. Retrans.: %u ", (unsigned)u8);
    }

    res = gnrc_netapi_get(dev, NETOPT_CSMA_RETRIES, 0, &u8, sizeof(u8));

    if (res >= 0) {
        res = gnrc_netapi_get(dev, NETOPT_CSMA, 0, &enable, sizeof(enable));
        if ((res >= 0) && (enable == NETOPT_ENABLE)) {
            printf(" CSMA Retries: %u ", (unsigned)u8);
        }
    }

    printf("\n           ");

    res = gnrc_netapi_get(dev, NETOPT_PROMISCUOUSMODE, 0, &enable, sizeof(enable));

    if ((res >= 0) && (enable == NETOPT_ENABLE)) {
        printf("PROMISC  ");
        linebreak = true;
    }

    res = gnrc_netapi_get(dev, NETOPT_AUTOACK, 0, &enable, sizeof(enable));

    if ((res >= 0) && (enable == NETOPT_ENABLE)) {
        printf("AUTOACK  ");
        linebreak = true;
    }

    res = gnrc_netapi_get(dev, NETOPT_PRELOADING, 0, &enable, sizeof(enable));

    if ((res >= 0) && (enable == NETOPT_ENABLE)) {
        printf("PRELOAD  ");
        linebreak = true;
    }

    res = gnrc_netapi_get(dev, NETOPT_RAWMODE, 0, &enable, sizeof(enable));

    if ((res >= 0) && (enable == NETOPT_ENABLE)) {
        printf("RAWMODE  ");
        linebreak = true;
    }

    res = gnrc_netapi_get(dev, NETOPT_CSMA, 0, &enable, sizeof(enable));

    if ((res >= 0) && (enable == NETOPT_ENABLE)) {
        printf("CSMA  ");
        linebreak = true;
    }

    res = gnrc_netapi_get(dev, NETOPT_AUTOCCA, 0, &enable, sizeof(enable));

    if ((res >= 0) && (enable == NETOPT_ENABLE)) {
        printf("AUTOCCA  ");
        linebreak = true;
    }

    res = gnrc_netapi_get(dev, NETOPT_CCA_MODE, 0, &u8, sizeof(u8));

    if (res >= 0) {
        printf("CCA-MODE: ");
        switch (u8) {
            case NETDEV2_IEEE802154_CCA_MODE_1:
                printf("ED");
                break;
            case NETDEV2_IEEE802154_CCA_MODE_2:
                printf("CS");
                break;
            case NETDEV2_IEEE802154_CCA_MODE_3:
                printf("ED+CS");
                break;
            case NETDEV2_IEEE802154_CCA_MODE_4:
                printf("ALOHA");
                break;
            case NETDEV2_IEEE802154_CCA_MODE_5:
                printf("UWB PS on SHR");
                break;
            case NETDEV2_IEEE802154_CCA_MODE_6:
                printf("UWB PS");
                break;
            default:
                printf("unknown");
                break;
        }
        linebreak = true;
    }

    if (linebreak) {
        printf("\n           ");
    }

    res = gnrc_netapi_get(dev, NETOPT_SRC_LEN, 0, &u16, sizeof(u16));

    if (res >= 0) {
        printf("Source address length: %" PRIu16 "\n           ", u16);
    }

    puts("");
}

static int _iwpan_set_u16(kernel_pid_t dev, netopt_t opt, char *u16_str)
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

    if (gnrc_netapi_set(dev, opt, 0, (uint16_t *)&res, sizeof(uint16_t)) < 0) {
        printf("error: unable to set ");
        _print_netopt(opt);
        puts("");
        return 1;
    }

    printf("success: set ");
    _print_netopt(opt);
    printf(" on interface %" PRIkernel_pid " to ", dev);

    if (hex) {
        printf("0x%04x\n", res);
    }
    else {
        printf("%u\n", res);
    }

    return 0;
}

static int _iwpan_set_i16(kernel_pid_t dev, netopt_t opt, char *i16_str)
{
    int16_t val = (int16_t)atoi(i16_str);

    if (gnrc_netapi_set(dev, opt, 0, (int16_t *)&val, sizeof(int16_t)) < 0) {
        printf("error: unable to set ");
        _print_netopt(opt);
        puts("");
        return 1;
    }

    printf("success: set ");
    _print_netopt(opt);
    printf(" on interface %" PRIkernel_pid " to %i\n", dev, val);

    return 0;
}

static int _iwpan_set_u8(kernel_pid_t dev, netopt_t opt, char *u8_str)
{
    uint8_t val = (uint8_t)atoi(u8_str);

    if (gnrc_netapi_set(dev, opt, 0, (uint8_t *)&val, sizeof(uint8_t)) < 0) {
        printf("error: unable to set ");
        _print_netopt(opt);
        puts("");
        return 1;
    }

    printf("success: set ");
    _print_netopt(opt);
    printf(" on interface %" PRIkernel_pid " to %i\n", dev, val);

    return 0;
}

static int _iwpan_set_flag(kernel_pid_t dev, netopt_t opt,
                           netopt_enable_t set)
{
    if (gnrc_netapi_set(dev, opt, 0, &set, sizeof(netopt_enable_t)) < 0) {
        puts("error: unable to set option");
        return 1;
    }
    printf("success: %sset option\n", (set) ? "" : "un");
    return 0;
}

static int _iwpan_set_addr(kernel_pid_t dev, netopt_t opt, char *addr_str)
{
    uint8_t addr[MAX_ADDR_LEN];
    size_t addr_len = gnrc_netif_addr_from_str(addr, sizeof(addr), addr_str);

    if (addr_len == 0) {
        puts("error: unable to parse address.\n"
             "Must be of format [0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*\n"
             "(hex pairs delimited by colons)");
        return 1;
    }

    if (gnrc_netapi_set(dev, opt, 0, addr, addr_len) < 0) {
        printf("error: unable to set ");
        _print_netopt(opt);
        puts("");
        return 1;
    }

    printf("success: set ");
    _print_netopt(opt);
    printf(" on interface %" PRIkernel_pid " to %s\n", dev, addr_str);

    return 0;
}

static int _iwpan_set_state(kernel_pid_t dev, char *state_str)
{
    netopt_state_t state;

    if ((strcmp("off", state_str) == 0) || (strcmp("OFF", state_str) == 0)) {
        state = NETOPT_STATE_OFF;
    }
    else if ((strcmp("sleep", state_str) == 0) ||
             (strcmp("SLEEP", state_str) == 0)) {
        state = NETOPT_STATE_SLEEP;
    }
    else if ((strcmp("idle", state_str) == 0) ||
             (strcmp("IDLE", state_str) == 0)) {
        state = NETOPT_STATE_IDLE;
    }
    else if ((strcmp("reset", state_str) == 0) ||
             (strcmp("RESET", state_str) == 0)) {
        state = NETOPT_STATE_RESET;
    }
    else {
        puts("usage: ifconfig <if_id> set state [off|sleep|idle|reset]");
        return 1;
    }
    if (gnrc_netapi_set(dev, NETOPT_STATE, 0,
                        &state, sizeof(netopt_state_t)) < 0) {
        printf("error: unable to set state to ");
        _print_netopt_state(state);
        puts("");
        return 1;
    }
    printf("success: set state of interface %" PRIkernel_pid " to ", dev);
    _print_netopt_state(state);
    puts("");

    return 0;
}

static int _iwpan_set_encrypt(kernel_pid_t dev, netopt_t opt, char *encrypt_str)
{
    netopt_enable_t set;
    size_t size = 1;
    if ((strcmp("on", encrypt_str) == 0) || (strcmp("ON", encrypt_str) == 0)) {
        set = NETOPT_ENABLE;
    }
    else if ((strcmp("off", encrypt_str) == 0) || (strcmp("OFF", encrypt_str) == 0)) {
        set = NETOPT_DISABLE;
    }
    else {
        puts("usage: ifconfig <if_id> set encryption [on|off]");
        return 1;
    }

    if (gnrc_netapi_set(dev, opt, 0, &set, size) < 0) {
        printf("error: unable to set ");
        _print_netopt(opt);
        puts("");
        return 1;
    }

    printf("success: set ");
    _print_netopt(opt);
    printf(" on interface %" PRIkernel_pid " to %s\n", dev, encrypt_str);

    return 0;
}

static int _hex_to_int(char c) {
    if ('0' <= c && c <= '9') {
        return c - '0';
    }
    else if ('a' <= c && c <= 'f') {
        return c - 'a';
    }
    else if ('A' <= c && c <= 'F') {
        return c - 'A';
    }
    else {
        return -1;
    }
}

static int _iwpan_set_encrypt_key(kernel_pid_t dev, netopt_t opt, char *key_str)
{
    size_t str_len = strlen(key_str);
    size_t key_len = str_len / 2;
    uint8_t key[key_len];

    if (str_len == 14U) {
        printf("\nNotice: setting 56 bit key.");
    }
    else if (str_len == 16U) {
        printf("\nNotice: setting 64 bit key.");
    }
    else if (str_len == 32U) {
        printf("\nNotice: setting 128 bit key.");
    }
    else if (str_len == 48U) {
        printf("\nNotice: setting 192 bit key.");
    }
    else if (str_len == 64U) {
        printf("\nNotice: setting 256 bit key.");
    }
    else if (str_len == 128U) {
        printf("\nNotice: setting 512 bit key.");
    }
    else {
        printf("error: invalid key size.\n");
        return 1;
    }
    /* Convert any char from ASCII table in hex format */
    for (size_t i = 0; i < str_len; i += 2) {
        int i1 = _hex_to_int(key_str[i]);
        int i2 = _hex_to_int(key_str[i + 1]);

        if (i1 == -1 || i2 == -1) {
            puts("error: unable to parse key");
            return 1;
        }

        key[i / 2] = (uint8_t)((i1 << 4) + i2);
    }

    if (gnrc_netapi_set(dev, opt, 0, key, key_len) < 0) {
        printf("error: unable to set ");
        _print_netopt(opt);
        puts("");
        return 1;
    }

    printf("success: set ");
    _print_netopt(opt);
    printf(" on interface %" PRIkernel_pid " to \n", dev);
    for (size_t i = 0; i < key_len; i++) {
        /* print the hex value of the key */
        printf("%02x", key[i]);
    }
    puts("");
    return 0;
}

static int _iwpan_set(char *cmd_name, kernel_pid_t dev, char *key, char *value)
{
    if ((strcmp("addr", key) == 0) || (strcmp("addr_short", key) == 0)) {
        return _iwpan_set_addr(dev, NETOPT_ADDRESS, value);
    }
    else if (strcmp("addr_long", key) == 0) {
        return _iwpan_set_addr(dev, NETOPT_ADDRESS_LONG, value);
    }
    else if ((strcmp("channel", key) == 0) || (strcmp("chan", key) == 0)) {
        return _iwpan_set_u16(dev, NETOPT_CHANNEL, value);
    }
    else if (strcmp("page", key) == 0) {
        return _iwpan_set_u16(dev, NETOPT_CHANNEL_PAGE, value);
    }
    else if ((strcmp("nid", key) == 0) || (strcmp("pan", key) == 0) ||
             (strcmp("pan_id", key) == 0)) {
        return _iwpan_set_u16(dev, NETOPT_NID, value);
    }
    else if (strcmp("power", key) == 0) {
        return _iwpan_set_i16(dev, NETOPT_TX_POWER, value);
    }
    else if (strcmp("src_len", key) == 0) {
        return _iwpan_set_u16(dev, NETOPT_SRC_LEN, value);
    }
    else if (strcmp("state", key) == 0) {
        return _iwpan_set_state(dev, value);
    }
    else if (strcmp("retrans", key) == 0) {
        return _iwpan_set_u8(dev, NETOPT_RETRANS, value);
    }
    else if (strcmp("csma_retries", key) == 0) {
        return _iwpan_set_u8(dev, NETOPT_CSMA_RETRIES, value);
    }
    else if (strcmp("cca_threshold", key) == 0) {
        return _iwpan_set_u8(dev, NETOPT_CCA_THRESHOLD, value);
    }
    else if (strcmp("cca_mode", key) == 0) {
        return _iwpan_set_u8(dev, NETOPT_CCA_MODE, value);
    }
    else if (strcmp("encrypt", key) == 0) {
        return _iwpan_set_encrypt(dev, NETOPT_ENCRYPTION, value);
    }
    else if (strcmp("key", key) == 0) {
        return _iwpan_set_encrypt_key(dev, NETOPT_ENCRYPTION_KEY, value);
    }

    _set_usage(cmd_name);
    return 1;
}

static int _iwpan_flag(char *cmd, kernel_pid_t dev, char *flag)
{
    netopt_enable_t set = NETOPT_ENABLE;

    if (flag[0] == '-') {
        set = NETOPT_DISABLE;
        flag++;
    }

    if (strcmp(flag, "promisc") == 0) {
        return _iwpan_set_flag(dev, NETOPT_PROMISCUOUSMODE, set);
    }
    else if (strcmp(flag, "preload") == 0) {
        return _iwpan_set_flag(dev, NETOPT_PRELOADING, set);
    }
    else if (strcmp(flag, "autoack") == 0) {
        return _iwpan_set_flag(dev, NETOPT_AUTOACK, set);
    }
    else if (strcmp(flag, "ackreq") == 0) {
        return _iwpan_set_flag(dev, NETOPT_AUTOACK, set);
    }
    else if (strcmp(flag, "raw") == 0) {
        return _iwpan_set_flag(dev, NETOPT_RAWMODE, set);
    }
    else if (strcmp(flag, "csma") == 0) {
        return _iwpan_set_flag(dev, NETOPT_CSMA, set);
    }
    else if (strcmp(flag, "autocca") == 0) {
        return _iwpan_set_flag(dev, NETOPT_AUTOCCA, set);
    }

    _flag_usage(cmd);
    return 1;
}

int _iwpan_config(int argc, char **argv)
{
    if (argc < 2) {
        kernel_pid_t ifs[GNRC_NETIF_NUMOF];
        size_t numof = gnrc_netif_get(ifs);

        for (size_t i = 0; i < numof && i < GNRC_NETIF_NUMOF; i++) {
            _iwpan_list(ifs[i]);
        }

        return 0;
    }
    else if (_is_number(argv[1])) {
        kernel_pid_t dev = (kernel_pid_t)atoi(argv[1]);

        if (_is_iface(dev)) {
            if (argc < 3) {
                _iwpan_list(dev);
                return 0;
            }
            else if (strcmp(argv[2], "set") == 0) {
                if (argc < 5) {
                    _set_usage(argv[0]);
                    return 1;
                }

                return _iwpan_set(argv[0], dev, argv[3], argv[4]);
            }
            else {
                return _iwpan_flag(argv[0], dev, argv[2]);
            }
        }
        else {
            puts("error: invalid interface given");
            return 1;
        }
    }

    printf("usage: %s [<if_id>]\n", argv[0]);
    _set_usage(argv[0]);
    _flag_usage(argv[0]);
    return 1;
}
