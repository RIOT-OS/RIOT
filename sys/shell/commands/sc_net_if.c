/*
 * Shell commands for network interfaces
 *
 * Copyright (C) 2013 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup shell_commands
 * @{
 * @file    sc_net_if.c
 * @brief   provides shell commands to configure network interfaces
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @}
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "inet_pton.h"
#include "inet_ntop.h"
#include "net_help.h"
#include "net_if.h"
#include "transceiver.h"

#ifndef MODULE_SIXLOWPAN
#define ADDR_REGISTERED_MAX (6)
#define ADDRS_LEN_MAX       (16)

static uint8_t addr_registered = 0;
static uint8_t addrs[ADDR_REGISTERED_MAX][ADDRS_LEN_MAX];
#else
#include "ipv6.h"
#endif

void _net_if_ifconfig_add(int if_id, int argc, char **argv);
void _net_if_ifconfig_add_ipv6(int if_id, int argc, char **argv);
void _net_if_ifconfig_set(int if_id, char *key, char *value);
void _net_if_ifconfig_set_srcaddrmode(int if_id, char *mode);
void _net_if_ifconfig_set_eui64(int if_id, char *addr);
void _net_if_ifconfig_set_hwaddr(int if_id, char *addr);
void _net_if_ifconfig_set_pan_id(int if_id, char *pan_id);
void _net_if_ifconfig_set_channel(int if_id, char *channel);
void _net_if_ifconfig_create(char *transceivers_str);
int _net_if_ifconfig_ipv6_addr_convert(net_if_addr_t *addr, void *addr_data,
                                       char *type, char *addr_data_str,
                                       char *addr_data_len);
void _net_if_ifconfig_list(int if_id);

int is_number(char *str)
{
    for (; *str; str++) {
        if (!isdigit((int)*str)) {
            return 0;
        }
    }

    return 1;
}

static inline void _eui64_to_str(char *eui64_str, net_if_eui64_t *eui64)
{
    sprintf(eui64_str, "%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x",
            eui64->uint8[0], eui64->uint8[1], eui64->uint8[2], eui64->uint8[3],
            eui64->uint8[4], eui64->uint8[5], eui64->uint8[6], eui64->uint8[7]);
}

char *addr_data_to_str(char *addr_str, const uint8_t *addr, uint8_t addr_len)
{
    int i;

    for (i = 0; i < addr_len / 8; i++) {
        sprintf(addr_str, "%02x", addr[i]);
    }

    uint8_t r = addr_len % 8;

    if (r) {
        uint8_t mask = 0x00;

        while (r) {
            mask |= 0x1 << (8 - (r--));
        }

        sprintf(addr_str, "%02x", addr[addr_len / 8] & mask);
    }

    return addr_str;
}

void add_usage(void)
{
    puts("Usage: ifconfig <if_id> add ipv6 [multicast|anycast] <addr>");
}

void set_usage(void)
{
    printf("Usage: ifconfig <if_id> set <key> <value>\n"
           "      Sets an transceiver specific value\n"
           "      <key> may be one of the following\n"
           "       * \"srcaddrmode\" - sets the source address mode for IEEE\n"
           "                         802.15.4 transeivers (valid values: \"short\" or \"long\"),\n"
           "       * \"sam\" - alias for \"srcaddrmode\"\n"
           "       * \"eui64\" - sets the EUI-64 if supported by transceivers,\n"
           "       * \"hwaddr\" - sets the 16-bit address (or just the address\n"
           "                    for e.g cc1100) of the transceivers\n"
           "       * \"pan_id\" - sets the PAN ID of the transceiver\n"
           "       * \"pan\" - alias for \"pan_id\"\n"
           "       * \"channel\" - sets the frequency channel of the transceiver\n"
           "       * \"chan\" - alias for \"channel\"\n");
}

void create_usage(void)
{
    puts("Usage: ifconfig create <transceiver_1>[,<transceiver_2>,...]\n"
         "       <transceiver_n> may be one of the following values:\n"
#ifdef MODULE_AT86RF231
         "        * at86rv231\n"
#endif
#ifdef MODULE_CC1020
         "        * cc1020\n"
#endif
#if MODULE_CC110X_LEGACY_CSMA || MODULE_CC110X_LEGACY
         "        * cc1100\n"
#endif
#ifdef MODULE_CC2420
         "        * cc2420\n"
#endif
#ifdef MODULE_MC1322X
         "        * mc1322x\n"
#endif
#ifdef MODULE_NATIVENET
         "        * native\n"
#endif
        );
}

void _net_if_ifconfig(int argc, char **argv)
{
    if (argc < 2) {
        int if_id = -1;

        while ((if_id = net_if_iter_interfaces(if_id)) >= 0) {
            _net_if_ifconfig_list(if_id);
        }

        return;
    }
    else if (strcmp(argv[1], "create") == 0) {
        _net_if_ifconfig_create(argv[2]);
        return;
    }
    else if (is_number(argv[1])) {
        int if_id = atoi(argv[1]);

        if (argc < 3) {
            _net_if_ifconfig_list(if_id);
            return;
        }
        else if (strcmp(argv[2], "add") == 0) {
            if (argc < 5) {
                add_usage();
                return;
            }

            _net_if_ifconfig_add(if_id, argc, argv);
            return;
        }
        else if (strcmp(argv[2], "set") == 0) {
            if (argc < 5) {
                set_usage();
                return;
            }

            _net_if_ifconfig_set(if_id, argv[3], argv[4]);
            return;
        }
    }

    create_usage();
    printf("or:    ifconfig [<if_id> [add <protocol> <addr>|set <key> <value>]]\n");
}

void _net_if_ifconfig_set_srcaddrmode(int if_id, char *mode)
{
    if (mode == NULL) {
        set_usage();
        return;
    }
    else if (strcmp(mode, "short") == 0) {
        net_if_set_src_address_mode(if_id, NET_IF_TRANS_ADDR_M_SHORT);
    }
    else if (strcmp(mode, "long") == 0) {
        net_if_set_src_address_mode(if_id, NET_IF_TRANS_ADDR_M_LONG);
    }
    else {
        set_usage();
        return;
    }
}

void _net_if_ifconfig_set_eui64(int if_id, char *eui64_str)
{
    net_if_eui64_t eui64;

    if (eui64_str == NULL) {
        set_usage();
        return;
    }

    net_if_hex_to_eui64(&eui64, eui64_str);
    net_if_set_eui64(if_id, &eui64);
}

void _net_if_ifconfig_set_hwaddr(int if_id, char *addr_str)
{
    int addr;

    if (addr_str == NULL) {
        set_usage();
        return;
    }

    if (is_number(addr_str)) {
        if ((addr = atoi(addr_str)) > 0xffff) {
            set_usage();
            return;
        }
    }
    else {
        if ((addr = strtoul(addr_str, NULL, 16)) > 0xffff) {
            set_usage();
            return;
        }
    }

    net_if_set_hardware_address(if_id, (uint16_t)addr);
}

void _net_if_ifconfig_set_pan_id(int if_id, char *pan_str)
{
    int pan_id;

    if (pan_str == NULL) {
        set_usage();
        return;
    }

    if (is_number(pan_str)) {
        if ((pan_id = atoi(pan_str)) > 0xffff) {
            set_usage();
            return;
        }
    }
    else {
        if ((pan_id = strtoul(pan_str, NULL, 16)) > 0xffff) {
            set_usage();
            return;
        }
    }

    net_if_set_pan_id(if_id, (uint16_t) pan_id);
}

void _net_if_ifconfig_set_channel(int if_id, char *chan_str)
{
    int channel;

    if (chan_str == NULL) {
        set_usage();
        return;
    }

    if (is_number(chan_str)) {
        if ((channel = atoi(chan_str)) > 0xffff) {
            set_usage();
            return;
        }
    }
    else {
        if ((channel = strtoul(chan_str, NULL, 16)) > 0xffff) {
            set_usage();
            return;
        }
    }

    net_if_set_channel(if_id, (uint16_t) channel);
}

void _net_if_ifconfig_set(int if_id, char *key, char *value)
{
    if (strcmp(key, "sam") == 0 || strcmp(key, "srcaddrmode") == 0) {
        _net_if_ifconfig_set_srcaddrmode(if_id, value);
    }
    else if (strcmp(key, "eui64") == 0) {
        _net_if_ifconfig_set_eui64(if_id, value);
    }
    else if (strcmp(key, "hwaddr") == 0) {
        _net_if_ifconfig_set_hwaddr(if_id, value);
    }
    else if (strcmp(key, "pan") == 0 || strcmp(key, "pan_id") == 0) {
        _net_if_ifconfig_set_pan_id(if_id, value);
    }
    else if (strcmp(key, "chan") == 0 || strcmp(key, "channel") == 0) {
        _net_if_ifconfig_set_channel(if_id, value);
    }
    else {
        set_usage();
        return;
    }
}

void _net_if_ifconfig_add_ipv6(int if_id, int argc, char **argv)
{
    char *type;
    char *addr_str;
    char *addr_data_str;
    char *addr_data_len;
    net_if_addr_t addr;

    if (argc > 5) {
        if (strcmp(argv[4], "multicast") == 0 || strcmp(argv[4], "anycast") == 0 || strcmp(argv[4], "unicast") == 0) {
            type = argv[4];
            addr_str = argv[5];
        }
        else {
            add_usage();
            return;
        }
    }
    else {
        addr_str = argv[4];
        type = NULL;
    }

#ifdef MODULE_SIXLOWPAN
    ipv6_addr_t ipv6_addr;
    void *addr_data = &ipv6_addr;
#else
    void *addr_data = (void *)&addrs[addr_registered][0];
#endif

    addr_data_str = strtok(addr_str, "/");
    addr_data_len = strtok(NULL, "/");

    if (!_net_if_ifconfig_ipv6_addr_convert(&addr, addr_data, type,
                                            addr_data_str, addr_data_len)) {
        add_usage();
        return;
    }

#ifdef MODULE_SIXLOWPAN

    if (addr.addr_protocol & NET_IF_L3P_IPV6_PREFIX) {
        if (ndp_add_prefix_info(if_id, &ipv6_addr, addr.addr_len,
                                NDP_OPT_PI_VLIFETIME_INFINITE,
                                NDP_OPT_PI_PLIFETIME_INFINITE, 1,
                                ICMPV6_NDP_OPT_PI_FLAG_AUTONOM) != SIXLOWERROR_SUCCESS) {
            add_usage();
            return;
        }
    }
    else if (addr.addr_protocol & NET_IF_L3P_IPV6_ADDR) {
        uint8_t is_anycast = 0;

        if (addr.addr_protocol & NET_IF_L3P_IPV6_ANYCAST) {
            is_anycast = 1;
        }

        if (!ipv6_net_if_add_addr(if_id, &ipv6_addr, NDP_ADDR_STATE_PREFERRED,
                                  0, 0, is_anycast)) {
            add_usage();
            return;
        }
    }
    else {
        add_usage();
        return;
    }

#else

    if (net_if_add_address(if_id, &addr) < 0) {
        add_usage();
        return;
    }

    addr_registered++;
#endif
}

void _net_if_ifconfig_add(int if_id, int argc, char **argv)
{
    if (strcmp(argv[3], "ipv6") == 0) {
        _net_if_ifconfig_add_ipv6(if_id, argc, argv);
    }
    else {
        add_usage();
    }
}

void _net_if_ifconfig_create(char *transceivers_str)
{
    char *transceiver_str;
    transceiver_type_t transceivers = TRANSCEIVER_NONE;
    int iface;

    transceiver_str = strtok(transceivers_str, ",");

    while (transceiver_str) {
        if (strcasecmp(transceiver_str, "at86rv231") == 0) {
            transceivers |= TRANSCEIVER_AT86RF231;
        }
        else if (strcasecmp(transceiver_str, "cc1020") == 0) {
            transceivers |= TRANSCEIVER_CC1020;
        }
        else if (strcasecmp(transceiver_str, "cc1100") == 0) {
            transceivers |= TRANSCEIVER_CC1100;
        }
        else if (strcasecmp(transceiver_str, "cc2420") == 0) {
            transceivers |= TRANSCEIVER_CC2420;
        }
        else if (strcasecmp(transceiver_str, "mc1322x") == 0) {
            transceivers |= TRANSCEIVER_MC1322X;
        }
        else if (strcasecmp(transceiver_str, "native") == 0) {
            transceivers |= TRANSCEIVER_NATIVE;
        }
        else {
            create_usage();
            return;
        }

        transceiver_str = strtok(NULL, ",");
    }

    if (!transceivers) {
        create_usage();
        return;
    }

    iface = net_if_init_interface(NET_IF_L3P_RAW, transceivers);

    if (iface < 0) {
        puts("Maximum number of allowed interfaces reached.\n");
    }
    else {
        printf("Initialized interface %d\n", iface);
    }
}

static inline int _is_multicast(uint8_t *addr)
{
#ifdef MODULE_SIXLOWPAN
    return ipv6_addr_is_multicast((ipv6_addr_t *) addr);
#else
    return *addr == 0xff;
#endif
}

static inline int _is_link_local(uint8_t *addr)
{
#ifdef MODULE_SIXLOWPAN
    return ipv6_addr_is_link_local((ipv6_addr_t *) addr);
#else
    return (addr[0] == 0xfe && addr[1] == 0x80) ||
           (_is_multicast(addr) && (addr[1] & 0x0f) == 2);
#endif
}

int _set_protocol_from_type(char *type, net_if_addr_t *addr)
{
    if (type != NULL) {
        if ((strcmp(type, "multicast") == 0) &&
            _is_multicast((uint8_t *)addr->addr_data)) {
            addr->addr_protocol |= NET_IF_L3P_IPV6_MULTICAST;
            return 1;
        }
        else if ((strcmp(type, "anycast") == 0) &&
                 addr->addr_protocol & NET_IF_L3P_IPV6_PREFIX) {
            addr->addr_protocol |= NET_IF_L3P_IPV6_ANYCAST;
            return 1;
        }

        return 0;
    }
    else if (_is_multicast((uint8_t *)addr->addr_data)) {
        addr->addr_protocol |= NET_IF_L3P_IPV6_MULTICAST;
        return 1;
    }
    else {
        addr->addr_protocol |= NET_IF_L3P_IPV6_UNICAST;
        return 1;
    }
}

int _net_if_ifconfig_ipv6_addr_convert(net_if_addr_t *addr, void *addr_data,
                                       char *type, char *addr_data_str,
                                       char *addr_data_len)
{
    if (addr_data_len && !is_number(addr_data_len)) {
        return 0;
    }

    addr->addr_data = addr_data;

    if (!inet_pton(AF_INET6, addr_data_str, addr->addr_data)) {
        return 0;
    }

    else if (addr_data_len == NULL) {
        addr->addr_len = 128;
        addr->addr_protocol = 0;

        if (!_set_protocol_from_type(type, addr)) {
            return 0;
        }
    }
    else {
        addr->addr_len = atoi(addr_data_len);
        addr->addr_protocol = NET_IF_L3P_IPV6_PREFIX;

        if (addr->addr_len > 128 || !_set_protocol_from_type(type, addr)) {
            return 0;
        }
    }

    return 1;
}

void _net_if_ifconfig_list(int if_id)
{
    net_if_t *iface = net_if_get_interface(if_id);
    transceiver_type_t transceivers;
    uint16_t hw_address;
    int32_t channel;
    int32_t pan_id;
    net_if_eui64_t eui64;
    char eui64_str[24];
    net_if_addr_t *addr_ptr = NULL;

    if (!iface) {
        return;
    }

    transceivers = iface->transceivers;
    hw_address = net_if_get_hardware_address(if_id);
    channel = net_if_get_channel(if_id);
    pan_id = net_if_get_pan_id(if_id);
    net_if_get_eui64(&eui64, if_id, 0);
    _eui64_to_str(eui64_str, &eui64);

    printf("Iface %3d   HWaddr: 0x%04x", if_id,
           hw_address);

    if (channel < 0) {
        printf(" Channel: not set");
    }
    else {
        printf(" Channel: %d", (uint16_t) channel);
    }

    if (pan_id < 0) {
        printf(" PAN ID: not set");
    }
    else {
        printf(" PAN ID: 0x%04x", (uint16_t)pan_id);
    }

    printf("\n");

    printf("            EUI-64: %s\n", eui64_str);

    switch (net_if_get_src_address_mode(if_id)) {
        case NET_IF_TRANS_ADDR_M_SHORT:
            puts("            Source address mode: short");
            break;

        case NET_IF_TRANS_ADDR_M_LONG:
            puts("            Source address mode: long");
            break;

        default:
            puts("            Source address mode: unknown");
            break;
    }

    puts("            Transceivers:");

    if (transceivers & TRANSCEIVER_AT86RF231) {
        puts("             * at86rf231");
    }

    if (transceivers & TRANSCEIVER_CC1020) {
        puts("             * cc1020");
    }

    if (transceivers & TRANSCEIVER_CC1100) {
        puts("             * cc1100");
    }

    if (transceivers & TRANSCEIVER_CC2420) {
        puts("             * cc2420");
    }

    if (transceivers & TRANSCEIVER_MC1322X) {
        puts("             * mc1322x");
    }

    if (transceivers & TRANSCEIVER_NATIVE) {
        puts("             * native");
    }

    while (net_if_iter_addresses(if_id, &addr_ptr)) {
        if (addr_ptr->addr_protocol == NET_IF_L3P_RAW) {
            char addr_str[addr_ptr->addr_len / 4 + 3];

            printf("            Raw L3 addr: 0x");
            printf("%s", addr_data_to_str(addr_str, addr_ptr->addr_data,
                                          addr_ptr->addr_len));
            puts("\n");
        }

#ifdef MODULE_SIXLOWPAN
        if (addr_ptr->addr_protocol & NET_IF_L3P_IPV6) {
            char addr_str[IPV6_MAX_ADDR_STR_LEN];
            printf("            inet6 addr: ");

            if (inet_ntop(AF_INET6, addr_ptr->addr_data, addr_str,
                          IPV6_MAX_ADDR_STR_LEN)) {
                printf("%s/%d", addr_str, addr_ptr->addr_len);
                printf("  scope: ");

                if (addr_ptr->addr_len > 2 && _is_link_local((uint8_t *)addr_ptr->addr_data)) {
                    printf("local");
                }
                else {
                    printf("global");
                }

                if (!(addr_ptr->addr_protocol & NET_IF_L3P_IPV6_UNICAST)) {
                    printf(" ");

                    if (addr_ptr->addr_protocol & NET_IF_L3P_IPV6_MULTICAST) {
                        printf("[multicast]");
                    }
                    else if (addr_ptr->addr_protocol & NET_IF_L3P_IPV6_ANYCAST) {
                        printf("[anycast]");
                    }
                }

                printf("\n");
            }
            else {
                printf("error in conversion\n");
            }
        }
#endif
    }

    puts("");
}
