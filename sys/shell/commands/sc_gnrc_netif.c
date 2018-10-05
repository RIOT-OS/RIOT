/*
 * Copyright (C) 2017 Freie Universität Berlin
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
 * @brief       Shell commands for interacting with network interfaces
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#include <stdio.h>
#include <string.h>

#include "net/ipv6/addr.h"
#include "net/gnrc.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/lora.h"

#ifdef MODULE_NETSTATS
#include "net/netstats.h"
#endif
#ifdef MODULE_L2FILTER
#include "net/l2filter.h"
#endif

/**
 * @brief   The default IPv6 prefix length if not specified.
 */
#define _IPV6_DEFAULT_PREFIX_LEN        (64U)

/**
 * @brief   Threshold for listed option flags
 */
#define _LINE_THRESHOLD                 (8U)

/**
 * @brief   Determine length of array in elements
 */
#define _ARRAY_LEN(x)                   (sizeof(x) / sizeof(x[0]))

/**
 * @brief   Flag command mapping
 *
 * @note    Add options that are changed with netopt_enable_t here
 */
static const struct {
    char *name;
    netopt_t opt;
} flag_cmds[] = {
    { "ack_req", NETOPT_ACK_REQ },
    { "autoack", NETOPT_AUTOACK },
    { "autocca", NETOPT_AUTOCCA },
    { "csma", NETOPT_CSMA },
    { "encrypt", NETOPT_ENCRYPTION },
    { "mac_no_sleep", NETOPT_MAC_NO_SLEEP },
    { "fwd", NETOPT_IPV6_FORWARDING },
    { "iphc", NETOPT_6LO_IPHC },
    { "preload", NETOPT_PRELOADING },
    { "promisc", NETOPT_PROMISCUOUSMODE },
    { "phy_busy", NETOPT_PHY_BUSY },
    { "raw", NETOPT_RAWMODE },
    { "rtr_adv", NETOPT_IPV6_SND_RTR_ADV },
    { "iq_invert", NETOPT_IQ_INVERT },
    { "rx_single", NETOPT_SINGLE_RECEIVE },
    { "chan_hop", NETOPT_CHANNEL_HOP },
    { "checksum", NETOPT_CHECKSUM },
};

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

static inline bool _is_iface(kernel_pid_t iface)
{
    return (gnrc_netif_get_by_pid(iface) != NULL);
}

#ifdef MODULE_NETSTATS
static const char *_netstats_module_to_str(uint8_t module)
{
    switch (module) {
        case NETSTATS_LAYER2:
            return "Layer 2";
        case NETSTATS_IPV6:
            return "IPv6";
        case NETSTATS_ALL:
            return "all";
        default:
            return "Unknown";
    }
}

static int _netif_stats(kernel_pid_t iface, unsigned module, bool reset)
{
    netstats_t *stats;
    int res = gnrc_netapi_get(iface, NETOPT_STATS, module, &stats,
                              sizeof(&stats));

    if (res < 0) {
        puts("           Protocol or device doesn't provide statistics.");
    }
    else if (reset) {
        memset(stats, 0, sizeof(netstats_t));
        printf("Reset statistics for module %s!\n", _netstats_module_to_str(module));
    }
    else {
        printf("          Statistics for %s\n"
               "            RX packets %u  bytes %u\n"
               "            TX packets %u (Multicast: %u)  bytes %u\n"
               "            TX succeeded %u errors %u\n",
               _netstats_module_to_str(module),
               (unsigned) stats->rx_count,
               (unsigned) stats->rx_bytes,
               (unsigned) (stats->tx_unicast_count + stats->tx_mcast_count),
               (unsigned) stats->tx_mcast_count,
               (unsigned) stats->tx_bytes,
               (unsigned) stats->tx_success,
               (unsigned) stats->tx_failed);
        res = 0;
    }
    return res;
}
#endif /* MODULE_NETSTATS */

static void _set_usage(char *cmd_name)
{
    printf("usage: %s <if_id> set <key> <value>\n", cmd_name);
    puts("      Sets an hardware specific specific value\n"
         "      <key> may be one of the following\n"
         "       * \"addr\" - sets (short) address\n"
         "       * \"addr_long\" - sets long address\n"
         "       * \"addr_short\" - alias for \"addr\"\n"
         "       * \"cca_threshold\" - set ED threshold during CCA in dBm\n"
         "       * \"freq\" - sets the \"channel\" center frequency\n"
         "       * \"channel\" - sets the frequency channel\n"
         "       * \"chan\" - alias for \"channel\"\n"
         "       * \"checksum\" - set checksumming on-off\n"
         "       * \"csma_retries\" - set max. number of channel access attempts\n"
         "       * \"encrypt\" - set the encryption on-off\n"
         "       * \"hop_limit\" - set hop limit\n"
         "       * \"hl\" - alias for \"hop_limit\"\n"
         "       * \"key\" - set the encryption key in hexadecimal format\n"
         "       * \"mtu\" - IPv6 maximum transition unit\n"
         "       * \"nid\" - sets the network identifier (or the PAN ID)\n"
         "       * \"page\" - set the channel page (IEEE 802.15.4)\n"
         "       * \"pan\" - alias for \"nid\"\n"
         "       * \"pan_id\" - alias for \"nid\"\n"
         "       * \"phy_busy\" - set busy mode on-off\n"
         "       * \"bw\" - alias for channel bandwidth\n"
         "       * \"sf\" - alias for spreading factor\n"
         "       * \"cr\" - alias for coding rate\n"
         "       * \"power\" - TX power in dBm\n"
         "       * \"retrans\" - max. number of retransmissions\n"
         "       * \"src_len\" - sets the source address length in byte\n"
         "       * \"state\" - set the device state\n");
}

static void _flag_usage(char *cmd_name)
{
    printf("usage: %s <if_id> [-]{", cmd_name);
    for (unsigned i = 0; i < _ARRAY_LEN(flag_cmds); i++) {
        printf("%s", flag_cmds[i].name);
        if (i < (_ARRAY_LEN(flag_cmds) - 1)) {
            printf("|");
        }
    }
    puts("}");
}

static void _add_usage(char *cmd_name)
{
    printf("usage: %s <if_id> add [anycast|multicast|unicast] "
           "<ipv6_addr>[/prefix_len]\n", cmd_name);
}

static void _del_usage(char *cmd_name)
{
    printf("usage: %s <if_id> del <ipv6_addr>\n",
           cmd_name);
}

#ifdef MODULE_NETSTATS
static void _stats_usage(char *cmd_name)
{
    printf("usage: %s <if_id> stats [l2|ipv6] [reset]\n", cmd_name);
    puts("       reset can be only used if the module is specified.");
}
#endif

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

        case NETOPT_CHANNEL_FREQUENCY:
            printf("frequency [in Hz]");
            break;

        case NETOPT_CHANNEL_PAGE:
            printf("page");
            break;

        case NETOPT_HOP_LIMIT:
            printf("MTU");
            break;

        case NETOPT_MAX_PACKET_SIZE:
            printf("MTU");
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

        case NETOPT_ENCRYPTION:
            printf("encryption");
            break;

        case NETOPT_ENCRYPTION_KEY:
            printf("encryption key");
            break;

        case NETOPT_BANDWIDTH:
            printf("bandwidth");
            break;

        case NETOPT_SPREADING_FACTOR:
            printf("spreading factor");
            break;

        case NETOPT_CODING_RATE:
            printf("coding rate");
            break;

        case NETOPT_CHECKSUM:
            printf("checksum");
            break;

        case NETOPT_PHY_BUSY:
            printf("PHY busy");
            break;

        default:
            /* we don't serve these options here */
            break;
    }
}

static const char *_netopt_state_str[] = {
    [NETOPT_STATE_OFF] = "OFF",
    [NETOPT_STATE_SLEEP] = "SLEEP",
    [NETOPT_STATE_IDLE] = "IDLE",
    [NETOPT_STATE_RX] = "RX",
    [NETOPT_STATE_TX] = "TX",
    [NETOPT_STATE_RESET] = "RESET",
    [NETOPT_STATE_STANDBY] = "STANDBY"
};

static const char *_netopt_bandwidth_str[] = {
    [LORA_BW_125_KHZ] = "125",
    [LORA_BW_250_KHZ] = "250",
    [LORA_BW_500_KHZ] = "500"
};

static const char *_netopt_coding_rate_str[] = {
    [LORA_CR_4_5] = "4/5",
    [LORA_CR_4_6] = "4/6",
    [LORA_CR_4_7] = "4/7",
    [LORA_CR_4_8] = "4/8"
};

/* for some lines threshold might just be 0, so we can't use _LINE_THRESHOLD
 * here */
static unsigned _newline(unsigned threshold, unsigned line_thresh)
{
    if (line_thresh > threshold) {
        printf("\n          ");
        line_thresh = 0U;
    }
    return line_thresh;
}


static unsigned _netif_list_flag(kernel_pid_t iface, netopt_t opt, char *str,
                                 unsigned line_thresh)
{
    netopt_enable_t enable = NETOPT_DISABLE;
    int res = gnrc_netapi_get(iface, opt, 0, &enable,
                              sizeof(enable));

    if ((res >= 0) && (enable == NETOPT_ENABLE)) {
        printf("%s", str);
        line_thresh = _newline(_LINE_THRESHOLD, ++line_thresh);
    }
    return line_thresh;
}

#ifdef MODULE_GNRC_IPV6
static void _netif_list_ipv6(ipv6_addr_t *addr, uint8_t flags)
{
    char addr_str[IPV6_ADDR_MAX_STR_LEN];
    unsigned line_thresh = _LINE_THRESHOLD;

    printf("inet6 addr: ");
    ipv6_addr_to_str(addr_str, addr, sizeof(addr_str));
    printf("%s  scope: ", addr_str);
    if ((ipv6_addr_is_link_local(addr))) {
        printf("local");
    }
    else {
        printf("global");
    }
    if (flags & GNRC_NETIF_IPV6_ADDRS_FLAGS_ANYCAST) {
        printf(" [anycast]");
    }
    switch (flags & GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_MASK) {
        case GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_TENTATIVE:
            printf("  TNT");
            break;
        case GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_DEPRECATED:
            printf("  DPR");
            break;
        case GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID:
            printf("  VAL");
            break;
    }
    line_thresh = _newline(0U, line_thresh);
}

static void _netif_list_groups(ipv6_addr_t *addr)
{
    unsigned line_thresh = _LINE_THRESHOLD;

    if ((ipv6_addr_is_multicast(addr))) {
        char addr_str[IPV6_ADDR_MAX_STR_LEN];
        ipv6_addr_to_str(addr_str, addr, sizeof(addr_str));
        printf("inet6 group: %s", addr_str);
    }
    line_thresh = _newline(0U, line_thresh);
}
#endif

static void _netif_list(kernel_pid_t iface)
{
#ifdef MODULE_GNRC_IPV6
    ipv6_addr_t ipv6_addrs[GNRC_NETIF_IPV6_ADDRS_NUMOF];
    ipv6_addr_t ipv6_groups[GNRC_NETIF_IPV6_GROUPS_NUMOF];
#endif
    uint8_t hwaddr[GNRC_NETIF_L2ADDR_MAXLEN];
    uint32_t u32;
    uint16_t u16;
    int16_t i16;
    uint8_t u8;
    int res;
    netopt_state_t state;
    unsigned line_thresh = 1;

    printf("Iface %2d ", iface);
    res = gnrc_netapi_get(iface, NETOPT_ADDRESS, 0, hwaddr, sizeof(hwaddr));
    if (res >= 0) {
        char hwaddr_str[res * 3];
        printf(" HWaddr: %s ",
               gnrc_netif_addr_to_str(hwaddr, res, hwaddr_str));
    }
    res = gnrc_netapi_get(iface, NETOPT_CHANNEL, 0, &u16, sizeof(u16));
    if (res >= 0) {
        printf(" Channel: %" PRIu16 " ", u16);
    }
    res = gnrc_netapi_get(iface, NETOPT_CHANNEL_FREQUENCY, 0, &u32, sizeof(u32));
    if (res >= 0) {
        printf(" Frequency: %" PRIu32 "Hz ", u32);
    }
    res = gnrc_netapi_get(iface, NETOPT_CHANNEL_PAGE, 0, &u16, sizeof(u16));
    if (res >= 0) {
        printf(" Page: %" PRIu16 " ", u16);
    }
    res = gnrc_netapi_get(iface, NETOPT_NID, 0, &u16, sizeof(u16));
    if (res >= 0) {
        printf(" NID: 0x%" PRIx16, u16);
    }
    res = gnrc_netapi_get(iface, NETOPT_BANDWIDTH, 0, &u8, sizeof(u8));
    if (res >= 0) {
        printf(" BW: %skHz ", _netopt_bandwidth_str[u8]);
    }
    res = gnrc_netapi_get(iface, NETOPT_SPREADING_FACTOR, 0, &u8, sizeof(u8));
    if (res >= 0) {
        printf(" SF: %u ", u8);
    }
    res = gnrc_netapi_get(iface, NETOPT_CODING_RATE, 0, &u8, sizeof(u8));
    if (res >= 0) {
        printf(" CR: %s ", _netopt_coding_rate_str[u8]);
    }
    res = gnrc_netapi_get(iface, NETOPT_LINK_CONNECTED, 0, &u8, sizeof(u8));
    if (res >= 0) {
        printf(" Link: %s ", (netopt_enable_t)u8 ? "up" : "down" );
    }
    line_thresh = _newline(0U, line_thresh);
    res = gnrc_netapi_get(iface, NETOPT_ADDRESS_LONG, 0, hwaddr, sizeof(hwaddr));
    if (res >= 0) {
        char hwaddr_str[res * 3];
        printf("Long HWaddr: ");
        printf("%s ", gnrc_netif_addr_to_str(hwaddr, res, hwaddr_str));
        line_thresh++;
    }
    line_thresh = _newline(0U, line_thresh);
    res = gnrc_netapi_get(iface, NETOPT_TX_POWER, 0, &i16, sizeof(i16));
    if (res >= 0) {
        printf(" TX-Power: %" PRIi16 "dBm ", i16);
    }
    res = gnrc_netapi_get(iface, NETOPT_STATE, 0, &state, sizeof(state));
    if (res >= 0) {
        printf(" State: %s ", _netopt_state_str[state]);
        line_thresh++;
    }
    res = gnrc_netapi_get(iface, NETOPT_RETRANS, 0, &u8, sizeof(u8));
    if (res >= 0) {
        printf(" max. Retrans.: %u ", (unsigned)u8);
        line_thresh++;
    }
    res = gnrc_netapi_get(iface, NETOPT_CSMA_RETRIES, 0, &u8, sizeof(u8));
    if (res >= 0) {
        netopt_enable_t enable = NETOPT_DISABLE;
        res = gnrc_netapi_get(iface, NETOPT_CSMA, 0, &enable, sizeof(enable));
        if ((res >= 0) && (enable == NETOPT_ENABLE)) {
            printf(" CSMA Retries: %u ", (unsigned)u8);
        }
        line_thresh++;
    }
    line_thresh = _newline(0U, line_thresh);
    line_thresh = _netif_list_flag(iface, NETOPT_PROMISCUOUSMODE, "PROMISC  ",
                                   line_thresh);
    line_thresh = _netif_list_flag(iface, NETOPT_AUTOACK, "AUTOACK  ",
                                   line_thresh);
    line_thresh = _netif_list_flag(iface, NETOPT_ACK_REQ, "ACK_REQ  ",
                                   line_thresh);
    line_thresh = _netif_list_flag(iface, NETOPT_PRELOADING, "PRELOAD  ",
                                   line_thresh);
    line_thresh = _netif_list_flag(iface, NETOPT_RAWMODE, "RAWMODE  ",
                                   line_thresh);
    line_thresh = _netif_list_flag(iface, NETOPT_MAC_NO_SLEEP, "MAC_NO_SLEEP  ",
                                   line_thresh);
    line_thresh = _netif_list_flag(iface, NETOPT_CSMA, "CSMA  ",
                                   line_thresh);
    line_thresh += _LINE_THRESHOLD + 1; /* enforce linebreak after this option */
    line_thresh = _netif_list_flag(iface, NETOPT_AUTOCCA, "AUTOCCA",
                                   line_thresh);
    line_thresh = _netif_list_flag(iface, NETOPT_IQ_INVERT, "IQ_INVERT",
                                   line_thresh);
    line_thresh = _netif_list_flag(iface, NETOPT_SINGLE_RECEIVE, "RX_SINGLE",
                                   line_thresh);
    line_thresh = _netif_list_flag(iface, NETOPT_CHANNEL_HOP, "CHAN_HOP",
                                   line_thresh);
#ifdef MODULE_GNRC_IPV6
    res = gnrc_netapi_get(iface, NETOPT_MAX_PACKET_SIZE, GNRC_NETTYPE_IPV6, &u16, sizeof(u16));
    if (res > 0) {
        printf("MTU:%" PRIu16 "  ", u16);
        line_thresh++;
    }
    res = gnrc_netapi_get(iface, NETOPT_HOP_LIMIT, 0, &u8, sizeof(u8));
    if (res > 0) {
        printf("HL:%u  ", u8);
        line_thresh++;
    }
    line_thresh = _netif_list_flag(iface, NETOPT_IPV6_FORWARDING, "RTR  ",
                                   line_thresh);
#ifndef MODULE_GNRC_SIXLOWPAN_IPHC
    line_thresh += _LINE_THRESHOLD + 1; /* enforce linebreak after this option */
#endif
    line_thresh = _netif_list_flag(iface, NETOPT_IPV6_SND_RTR_ADV, "RTR_ADV  ",
                                   line_thresh);
#ifdef MODULE_GNRC_SIXLOWPAN_IPHC
    line_thresh += _LINE_THRESHOLD + 1; /* enforce linebreak after this option */
    line_thresh = _netif_list_flag(iface, NETOPT_6LO_IPHC, "IPHC  ",
                                   line_thresh);
#endif
#endif
    res = gnrc_netapi_get(iface, NETOPT_SRC_LEN, 0, &u16, sizeof(u16));
    if (res >= 0) {
        printf("Source address length: %" PRIu16 , u16);
        line_thresh++;
    }
    line_thresh = _newline(0U, line_thresh);
#ifdef MODULE_GNRC_IPV6
    printf("Link type: %s",
           (gnrc_netapi_get(iface, NETOPT_IS_WIRED, 0, &u16, sizeof(u16)) > 0) ?
            "wired" : "wireless");
    line_thresh = _newline(0U, ++line_thresh);
    res = gnrc_netapi_get(iface, NETOPT_IPV6_ADDR, 0, ipv6_addrs,
                          sizeof(ipv6_addrs));
    if (res >= 0) {
        uint8_t ipv6_addrs_flags[GNRC_NETIF_IPV6_ADDRS_NUMOF];

        memset(ipv6_addrs_flags, 0, sizeof(ipv6_addrs_flags));
        /* assume it to succeed (otherwise array will stay 0) */
        gnrc_netapi_get(iface, NETOPT_IPV6_ADDR_FLAGS, 0, ipv6_addrs_flags,
                        sizeof(ipv6_addrs_flags));
        /* yes, the res of NETOPT_IPV6_ADDR is meant to be here ;-) */
        for (unsigned i = 0; i < (res / sizeof(ipv6_addr_t)); i++) {
            _netif_list_ipv6(&ipv6_addrs[i], ipv6_addrs_flags[i]);
        }
    }
    res = gnrc_netapi_get(iface, NETOPT_IPV6_GROUP, 0, ipv6_groups,
                          sizeof(ipv6_groups));
    if (res >= 0) {
        for (unsigned i = 0; i < (res / sizeof(ipv6_addr_t)); i++) {
            _netif_list_groups(&ipv6_groups[i]);
        }
    }
#endif

#ifdef MODULE_L2FILTER
    l2filter_t *filter = NULL;
    res = gnrc_netapi_get(iface, NETOPT_L2FILTER, 0, &filter, sizeof(filter));
    if (res > 0) {
#ifdef MODULE_L2FILTER_WHITELIST
        puts("\n           White-listed link layer addresses:");
#else
        puts("\n           Black-listed link layer addresses:");
#endif
        int count = 0;
        for (unsigned i = 0; i < L2FILTER_LISTSIZE; i++) {
            if (filter[i].addr_len > 0) {
                char hwaddr_str[filter[i].addr_len * 3];
                gnrc_netif_addr_to_str(filter[i].addr, filter[i].addr_len,
                                       hwaddr_str);
                printf("            %2i: %s\n", count++, hwaddr_str);
            }
        }
        if (count == 0) {
            puts("            --- none ---");
        }
    }
#endif

#ifdef MODULE_NETSTATS_L2
    puts("");
    _netif_stats(iface, NETSTATS_LAYER2, false);
#endif
#ifdef MODULE_NETSTATS_IPV6
    _netif_stats(iface, NETSTATS_IPV6, false);
#endif
    puts("");
}

static int _netif_set_u32(kernel_pid_t iface, netopt_t opt, uint32_t context,
                          char *u32_str)
{
    unsigned long int res;
    bool hex = false;

    if (_is_number(u32_str)) {
        if ((res = strtoul(u32_str, NULL, 10)) == ULONG_MAX) {
            puts("error: unable to parse value.\n"
                 "Must be a 32-bit unsigned integer (dec or hex)\n");
            return 1;
        }
    }
    else {
        if ((res = strtoul(u32_str, NULL, 32)) == ULONG_MAX) {
            puts("error: unable to parse value.\n"
                 "Must be a 32-bit unsigned integer (dec or hex)\n");
            return 1;
        }

        hex = true;
    }

    assert(res <= ULONG_MAX);

    if (gnrc_netapi_set(iface, opt, context, (uint32_t *)&res,
                        sizeof(uint32_t)) < 0) {
        printf("error: unable to set ");
        _print_netopt(opt);
        puts("");
        return 1;
    }

    printf("success: set ");
    _print_netopt(opt);
    printf(" on interface %" PRIkernel_pid " to ", iface);

    if (hex) {
        printf("0x%04lx\n", res);
    }
    else {
        printf("%lu\n", res);
    }

    return 0;
}

static int _netif_set_bandwidth(kernel_pid_t iface, char *value)
{
    uint8_t bw;

    if (strcmp("125", value) == 0) {
        bw = LORA_BW_125_KHZ;
    }
    else if (strcmp("250", value) == 0) {
        bw = LORA_BW_250_KHZ;
    }
    else if (strcmp("500", value) == 0) {
        bw = LORA_BW_500_KHZ;
    }
    else {
        puts("usage: ifconfig <if_id> set bw [125|250|500]");
        return 1;
    }
    if (gnrc_netapi_set(iface, NETOPT_BANDWIDTH, 0,
                        &bw, sizeof(uint8_t)) < 0) {
        printf("error: unable to set bandwidth to %s\n", value);
        return 1;
    }
    printf("success: set bandwidth of interface %" PRIkernel_pid " to %s\n",
           iface, value);

    return 0;
}

static int _netif_set_coding_rate(kernel_pid_t iface, char *value)
{
    uint8_t cr;

    if (strcmp("4/5", value) == 0) {
        cr = LORA_CR_4_5;
    }
    else if (strcmp("4/6", value) == 0) {
        cr = LORA_CR_4_6;
    }
    else if (strcmp("4/7", value) == 0) {
        cr = LORA_CR_4_7;
    }
    else if (strcmp("4/8", value) == 0) {
        cr = LORA_CR_4_8;
    }
    else {
        puts("usage: ifconfig <if_id> set cr [4/5|4/6|4/7|4/8]");
        return 1;
    }
    if (gnrc_netapi_set(iface, NETOPT_CODING_RATE, 0,
                        &cr, sizeof(uint8_t)) < 0) {
        printf("error: unable to set coding rate to %s\n", value);
        return 1;
    }
    printf("success: set coding rate of interface %" PRIkernel_pid " to %s\n",
           iface, value);

    return 0;
}

static int _netif_set_u16(kernel_pid_t iface, netopt_t opt, uint16_t context,
                          char *u16_str)
{
    unsigned long int res;
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

    if (gnrc_netapi_set(iface, opt, context, (uint16_t *)&res,
                        sizeof(uint16_t)) < 0) {
        printf("error: unable to set ");
        _print_netopt(opt);
        puts("");
        return 1;
    }

    printf("success: set ");
    _print_netopt(opt);
    printf(" on interface %" PRIkernel_pid " to ", iface);

    if (hex) {
        printf("0x%04lx\n", res);
    }
    else {
        printf("%lu\n", res);
    }

    return 0;
}

static int _netif_set_i16(kernel_pid_t iface, netopt_t opt, char *i16_str)
{
    int16_t val = atoi(i16_str);

    if (gnrc_netapi_set(iface, opt, 0, (int16_t *)&val, sizeof(int16_t)) < 0) {
        printf("error: unable to set ");
        _print_netopt(opt);
        puts("");
        return 1;
    }

    printf("success: set ");
    _print_netopt(opt);
    printf(" on interface %" PRIkernel_pid " to %i\n", iface, val);

    return 0;
}

static int _netif_set_u8(kernel_pid_t iface, netopt_t opt, uint16_t context,
                         char *u8_str)
{
    uint8_t val = atoi(u8_str);

    if (gnrc_netapi_set(iface, opt, context, (uint8_t *)&val,
                        sizeof(uint8_t)) < 0) {
        printf("error: unable to set ");
        _print_netopt(opt);
        puts("");
        return 1;
    }

    printf("success: set ");
    _print_netopt(opt);
    printf(" on interface %" PRIkernel_pid " to %i\n", iface, val);

    return 0;
}

static int _netif_set_flag(kernel_pid_t iface, netopt_t opt,
                           netopt_enable_t set)
{
    if (gnrc_netapi_set(iface, opt, 0, &set, sizeof(netopt_enable_t)) < 0) {
        puts("error: unable to set option");
        return 1;
    }
    printf("success: %sset option\n", (set) ? "" : "un");
    return 0;
}

static int _netif_set_addr(kernel_pid_t iface, netopt_t opt, char *addr_str)
{
    uint8_t addr[GNRC_NETIF_L2ADDR_MAXLEN];
    size_t addr_len = gnrc_netif_addr_from_str(addr_str, addr);

    if (addr_len == 0) {
        puts("error: unable to parse address.\n"
             "Must be of format [0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*\n"
             "(hex pairs delimited by colons)");
        return 1;
    }

    if (gnrc_netapi_set(iface, opt, 0, addr, addr_len) < 0) {
        printf("error: unable to set ");
        _print_netopt(opt);
        puts("");
        return 1;
    }

    printf("success: set ");
    _print_netopt(opt);
    printf(" on interface %" PRIkernel_pid " to %s\n", iface, addr_str);

    return 0;
}

static int _netif_set_state(kernel_pid_t iface, char *state_str)
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
    else if ((strcmp("rx", state_str) == 0) ||
             (strcmp("RX", state_str) == 0)) {
        state = NETOPT_STATE_RX;
    }
    else if ((strcmp("tx", state_str) == 0) ||
             (strcmp("TX", state_str) == 0)) {
        state = NETOPT_STATE_TX;
    }
    else if ((strcmp("reset", state_str) == 0) ||
             (strcmp("RESET", state_str) == 0)) {
        state = NETOPT_STATE_RESET;
    }
    else if ((strcmp("standby", state_str) == 0) ||
             (strcmp("STANDBY", state_str) == 0)) {
        state = NETOPT_STATE_STANDBY;
    }
    else {
        puts("usage: ifconfig <if_id> set state [off|sleep|idle|rx|tx|reset|standby]");
        return 1;
    }
    if (gnrc_netapi_set(iface, NETOPT_STATE, 0,
                        &state, sizeof(netopt_state_t)) < 0) {
        printf("error: unable to set state to %s\n", _netopt_state_str[state]);
        return 1;
    }
    printf("success: set state of interface %" PRIkernel_pid " to %s\n", iface,
           _netopt_state_str[state]);

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

static int _netif_set_encrypt_key(kernel_pid_t iface, netopt_t opt, char *key_str)
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

    if (gnrc_netapi_set(iface, opt, 0, key, key_len) < 0) {
        puts("error: unable to set encryption key");
        return 1;
    }

    printf("success: set encryption key on interface %" PRIkernel_pid " to \n",
           iface);
    for (size_t i = 0; i < key_len; i++) {
        /* print the hex value of the key */
        printf("%02x", key[i]);
    }
    puts("");
    return 0;
}

#ifdef MODULE_L2FILTER
static int _netif_addrm_l2filter(kernel_pid_t iface, char *val, bool add)
{
    uint8_t addr[GNRC_NETIF_L2ADDR_MAXLEN];
    size_t addr_len = gnrc_netif_addr_from_str(val, addr);

    if ((addr_len == 0) || (addr_len > L2FILTER_ADDR_MAXLEN)) {
        puts("error: given address is invalid");
        return 1;
    }

    if (add) {
        if (gnrc_netapi_set(iface, NETOPT_L2FILTER, 0, addr, addr_len) < 0) {
            puts("unable to add link layer address to filter");
            return 1;
        }
        puts("successfully added address to filter");
    }
    else {
        if (gnrc_netapi_set(iface, NETOPT_L2FILTER_RM, 0, addr, addr_len) < 0) {
            puts("unable to remove link layer address from filter");
            return 1;
        }
        puts("successfully removed address to filter");
    }
    return 0;
}

static void _l2filter_usage(const char *cmd)
{
    printf("usage: %s <if_id> l2filter {add|del} <addr>\n", cmd);
}
#endif

static void _usage(char *cmd)
{
    printf("usage: %s\n", cmd);
    _set_usage(cmd);
    _flag_usage(cmd);
    _add_usage(cmd);
    _del_usage(cmd);
#ifdef MODULE_L2FILTER
    _l2filter_usage(cmd);
#endif
#ifdef MODULE_NETSTATS
    _stats_usage(cmd);
#endif
}

static int _netif_set(char *cmd_name, kernel_pid_t iface, char *key, char *value)
{
    if ((strcmp("addr", key) == 0) || (strcmp("addr_short", key) == 0)) {
        return _netif_set_addr(iface, NETOPT_ADDRESS, value);
    }
    else if (strcmp("addr_long", key) == 0) {
        return _netif_set_addr(iface, NETOPT_ADDRESS_LONG, value);
    }
    else if (strcmp("cca_threshold", key) == 0) {
        return _netif_set_u8(iface, NETOPT_CCA_THRESHOLD, 0, value);
    }
    else if ((strcmp("frequency", key) == 0) || (strcmp("freq", key) == 0)) {
        return _netif_set_u32(iface, NETOPT_CHANNEL_FREQUENCY, 0, value);
    }
    else if ((strcmp("bandwidth", key) == 0) || (strcmp("bw", key) == 0)) {
        return _netif_set_bandwidth(iface, value);
    }
    else if ((strcmp("spreading_factor", key) == 0) || (strcmp("sf", key) == 0)) {
        return _netif_set_u8(iface, NETOPT_SPREADING_FACTOR, 0, value);
    }
    else if ((strcmp("coding_rate", key) == 0) || (strcmp("cr", key) == 0)) {
        return _netif_set_coding_rate(iface, value);
    }
    else if ((strcmp("channel", key) == 0) || (strcmp("chan", key) == 0)) {
        return _netif_set_u16(iface, NETOPT_CHANNEL, 0, value);
    }
    else if (strcmp("csma_retries", key) == 0) {
        return _netif_set_u8(iface, NETOPT_CSMA_RETRIES, 0, value);
    }
    else if ((strcmp("hl", key) == 0) || (strcmp("hop_limit", key) == 0)) {
        return _netif_set_u8(iface, NETOPT_HOP_LIMIT, 0, value);
    }
    else if (strcmp("key", key) == 0) {
        return _netif_set_encrypt_key(iface, NETOPT_ENCRYPTION_KEY, value);
    }
#ifdef MODULE_GNRC_IPV6
    else if (strcmp("mtu", key) == 0) {
        return _netif_set_u16(iface, NETOPT_MAX_PACKET_SIZE, GNRC_NETTYPE_IPV6,
                              value);
    }
#endif
    else if ((strcmp("nid", key) == 0) || (strcmp("pan", key) == 0) ||
             (strcmp("pan_id", key) == 0)) {
        return _netif_set_u16(iface, NETOPT_NID, 0, value);
    }
    else if (strcmp("page", key) == 0) {
        return _netif_set_u16(iface, NETOPT_CHANNEL_PAGE, 0, value);
    }
    else if (strcmp("power", key) == 0) {
        return _netif_set_i16(iface, NETOPT_TX_POWER, value);
    }
    else if (strcmp("retrans", key) == 0) {
        return _netif_set_u8(iface, NETOPT_RETRANS, 0, value);
    }
    else if (strcmp("src_len", key) == 0) {
        return _netif_set_u16(iface, NETOPT_SRC_LEN, 0, value);
    }
    else if (strcmp("state", key) == 0) {
        return _netif_set_state(iface, value);
    }

    _set_usage(cmd_name);
    return 1;
}

static int _netif_flag(char *cmd, kernel_pid_t iface, char *flag)
{
    netopt_enable_t set = NETOPT_ENABLE;

    if (flag[0] == '-') {
        set = NETOPT_DISABLE;
        flag++;
    }
    for (unsigned i = 0; i < _ARRAY_LEN(flag_cmds); i++) {
        if (strcmp(flag_cmds[i].name, flag) == 0) {
            return _netif_set_flag(iface, flag_cmds[i].opt, set);
        }
    }
    _flag_usage(cmd);
    return 1;
}

#ifdef MODULE_GNRC_IPV6
static uint8_t _get_prefix_len(char *addr)
{
    int prefix_len = ipv6_addr_split(addr, '/', _IPV6_DEFAULT_PREFIX_LEN);

    if (prefix_len < 1) {
        prefix_len = _IPV6_DEFAULT_PREFIX_LEN;
    }

    return prefix_len;
}
#endif

static int _netif_add(char *cmd_name, kernel_pid_t iface, int argc, char **argv)
{
#ifdef MODULE_GNRC_IPV6
    enum {
        _UNICAST = 0,
        _ANYCAST
    } type = _UNICAST;
    char *addr_str = argv[0];
    ipv6_addr_t addr;
    uint16_t flags = GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID;
    uint8_t prefix_len;

    if (argc > 1) {
        if (strcmp(argv[0], "anycast") == 0) {
            type = _ANYCAST;
            addr_str = argv[1];
        }
        else if (strcmp(argv[0], "unicast") == 0) {
            /* type already set to unicast */
            addr_str = argv[1];
        }
        else {
            _add_usage(cmd_name);
            return 1;
        }
    }

    prefix_len = _get_prefix_len(addr_str);

    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("error: unable to parse IPv6 address.");
        return 1;
    }

    if (ipv6_addr_is_multicast(&addr)) {
        if (gnrc_netapi_set(iface, NETOPT_IPV6_GROUP, 0, &addr,
                            sizeof(addr)) < 0) {
            printf("error: unable to join IPv6 multicast group\n");
            return 1;
        }
    }
    else {
        if (type == _ANYCAST) {
            flags |= GNRC_NETIF_IPV6_ADDRS_FLAGS_ANYCAST;
        }
        flags |= (prefix_len << 8U);
        if (gnrc_netapi_set(iface, NETOPT_IPV6_ADDR, flags, &addr,
                            sizeof(addr)) < 0) {
            printf("error: unable to add IPv6 address\n");
            return 1;
        }
    }

    printf("success: added %s/%d to interface %" PRIkernel_pid "\n", addr_str,
           prefix_len, iface);

    return 0;
#else
    (void)cmd_name;
    (void)iface;
    (void)argc;
    (void)argv;
    puts("error: unable to add IPv6 address.");

    return 1;
#endif
}

static int _netif_del(kernel_pid_t iface, char *addr_str)
{
#ifdef MODULE_GNRC_IPV6
    ipv6_addr_t addr;

    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("error: unable to parse IPv6 address.");
        return 1;
    }

    if (ipv6_addr_is_multicast(&addr)) {
        if (gnrc_netapi_set(iface, NETOPT_IPV6_GROUP_LEAVE, 0, &addr,
                            sizeof(addr)) < 0) {
            printf("error: unable to leave IPv6 multicast group\n");
            return 1;
        }
    }
    else {
        if (gnrc_netapi_set(iface, NETOPT_IPV6_ADDR_REMOVE, 0, &addr,
                            sizeof(addr)) < 0) {
            printf("error: unable to remove IPv6 address\n");
            return 1;
        }
    }

    printf("success: removed %s to interface %" PRIkernel_pid "\n", addr_str,
           iface);

    return 0;
#else
    (void)iface;
    (void)addr_str;
    puts("error: unable to delete IPv6 address.");
    return 1;
#endif
}

/* shell commands */
#ifdef MODULE_GNRC_TXTSND
int _gnrc_netif_send(int argc, char **argv)
{
    kernel_pid_t iface;
    uint8_t addr[GNRC_NETIF_L2ADDR_MAXLEN];
    size_t addr_len;
    gnrc_pktsnip_t *pkt, *hdr;
    gnrc_netif_hdr_t *nethdr;
    uint8_t flags = 0x00;

    if (argc < 4) {
        printf("usage: %s <if> [<L2 addr>|bcast] <data>\n", argv[0]);
        return 1;
    }

    /* parse interface */
    iface = atoi(argv[1]);

    if (!_is_iface(iface)) {
        puts("error: invalid interface given");
        return 1;
    }

    /* parse address */
    addr_len = gnrc_netif_addr_from_str(argv[2], addr);

    if (addr_len == 0) {
        if (strcmp(argv[2], "bcast") == 0) {
            flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;
        }
        else {
            puts("error: invalid address given");
            return 1;
        }
    }

    /* put packet together */
    pkt = gnrc_pktbuf_add(NULL, argv[3], strlen(argv[3]), GNRC_NETTYPE_UNDEF);
    if (pkt == NULL) {
        puts("error: packet buffer full");
        return 1;
    }
    hdr = gnrc_netif_hdr_build(NULL, 0, addr, addr_len);
    if (hdr == NULL) {
        puts("error: packet buffer full");
        gnrc_pktbuf_release(pkt);
        return 1;
    }
    LL_PREPEND(pkt, hdr);
    nethdr = (gnrc_netif_hdr_t *)hdr->data;
    nethdr->flags = flags;
    /* and send it */
    if (gnrc_netapi_send(iface, pkt) < 1) {
        puts("error: unable to send");
        gnrc_pktbuf_release(pkt);
        return 1;
    }

    return 0;
}
#endif

int _gnrc_netif_config(int argc, char **argv)
{
    if (argc < 2) {
        gnrc_netif_t *netif = NULL;

        while ((netif = gnrc_netif_iter(netif))) {
            _netif_list(netif->pid);
        }

        return 0;
    }
    else if (_is_number(argv[1])) {
        kernel_pid_t iface = atoi(argv[1]);

        if (_is_iface(iface)) {
            if (argc < 3) {
                _netif_list(iface);
                return 0;
            }
            else if (strcmp(argv[2], "set") == 0) {
                if (argc < 5) {
                    _set_usage(argv[0]);
                    return 1;
                }

                return _netif_set(argv[0], iface, argv[3], argv[4]);
            }
            else if (strcmp(argv[2], "add") == 0) {
                if (argc < 4) {
                    _add_usage(argv[0]);
                    return 1;
                }

                return _netif_add(argv[0], (kernel_pid_t)iface, argc - 3, argv + 3);
            }
            else if (strcmp(argv[2], "del") == 0) {
                if (argc < 4) {
                    _del_usage(argv[0]);
                    return 1;
                }

                return _netif_del((kernel_pid_t)iface, argv[3]);
            }
#ifdef MODULE_L2FILTER
            else if (strcmp(argv[2], "l2filter") == 0) {
                if (argc < 5) {
                    _l2filter_usage(argv[2]);
                }
                else if (strcmp(argv[3], "add") == 0) {
                    return _netif_addrm_l2filter(iface, argv[4], true);
                }
                else if (strcmp(argv[3], "del") == 0) {
                    return _netif_addrm_l2filter(iface, argv[4], false);
                }
                else {
                    _l2filter_usage(argv[2]);
                }
                return 1;
            }
#endif
#ifdef MODULE_NETSTATS
            else if (strcmp(argv[2], "stats") == 0) {
                uint8_t module;
                bool reset = false;

                /* check for requested module */
                if ((argc == 3) || (strcmp(argv[3], "all") == 0)) {
                    module = NETSTATS_ALL;
                }
                else if (strcmp(argv[3], "l2") == 0) {
                    module = NETSTATS_LAYER2;
                }
                else if (strcmp(argv[3], "ipv6") == 0) {
                    module = NETSTATS_IPV6;
                }
                else {
                    printf("Module %s doesn't exist or does not provide statistics.\n", argv[3]);

                    return 0;
                }

                /* check if reset flag was given */
                if ((argc > 4) && (strncmp(argv[4], "reset", 5) == 0)) {
                    reset = true;
                }
                if (module & NETSTATS_LAYER2) {
                    _netif_stats((kernel_pid_t) iface, NETSTATS_LAYER2, reset);
                }
                if (module & NETSTATS_IPV6) {
                    _netif_stats((kernel_pid_t) iface, NETSTATS_IPV6, reset);
                }

                return 1;
            }
#endif
            else if (strcmp(argv[2], "help") == 0) {
                _usage(argv[0]);
                return 0;
            }
            else {
                return _netif_flag(argv[0], iface, argv[2]);
            }
        }
        else {
            puts("error: invalid interface given");
            return 1;
        }
    }

    _usage(argv[0]);
    return 1;
}
