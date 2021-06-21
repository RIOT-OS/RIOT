/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "net/netdev/ieee802154.h"
#include "net/ieee802154.h"

#include "common.h"

#include "od.h"

#define _MAX_ADDR_LEN    (8)
#define MAC_VECTOR_SIZE  (2) /* mhr + payload */

static size_t _parse_addr(uint8_t *out, size_t out_len, const char *in);
static int send(int iface, le_uint16_t dst_pan, uint8_t *dst_addr,
                size_t dst_len, char *data);

int ifconfig_list(int idx)
{
    int res;
    netdev_ieee802154_t *dev = &devs[idx].netdev;

    int (*get)(netdev_t *, netopt_t, void *, size_t) = dev->netdev.driver->get;
    netopt_enable_t enable_val;
    uint16_t u16_val;

    printf("Iface %3d  HWaddr: ", idx);
    print_addr(dev->short_addr, IEEE802154_SHORT_ADDRESS_LEN);
    printf(", Long HWaddr: ");
    print_addr(dev->long_addr, IEEE802154_LONG_ADDRESS_LEN);
    printf(", PAN: 0x%04x", dev->pan);

    res = get(&dev->netdev, NETOPT_ADDR_LEN, &u16_val, sizeof(u16_val));
    if (res < 0) {
        puts("(err)");
        return 1;
    }
    printf("\n           Address length: %u", (unsigned)u16_val);

    res = get(&dev->netdev, NETOPT_SRC_LEN, &u16_val, sizeof(u16_val));
    if (res < 0) {
        puts("(err)");
        return 1;
    }
    printf(", Source address length: %u", (unsigned)u16_val);

    res = get(&dev->netdev, NETOPT_MAX_PDU_SIZE, &u16_val,
              sizeof(u16_val));
    if (res < 0) {
        puts("(err)");
        return 1;
    }
    printf(", Max.Payload: %u", (unsigned)u16_val);
    printf("\n           Channel: %u", dev->chan);

    res = get(&dev->netdev, NETOPT_CHANNEL_PAGE, &u16_val, sizeof(u16_val));
    if (res < 0) {
        puts("(err)");
        return 1;
    }
    printf(", Ch.page: %u", (unsigned)u16_val);

    res = get(&dev->netdev, NETOPT_TX_POWER, &u16_val, sizeof(u16_val));
    if (res < 0) {
        puts("(err)");
        return 1;
    }
    printf(", TXPower: %d dBm", (int)u16_val);
    res = get(&dev->netdev, NETOPT_IS_WIRED, &u16_val, sizeof(u16_val));
    if (res < 0) {
        puts(", wireless");
    }
    else {
        puts(", wired");
    }

    printf("         ");
    res = get(&dev->netdev, NETOPT_PRELOADING, &enable_val,
              sizeof(netopt_enable_t));
    if ((res > 0) && (enable_val == NETOPT_ENABLE)) {
        printf("  PRELOAD");
    }
    res = get(&dev->netdev, NETOPT_AUTOACK, &enable_val,
              sizeof(netopt_enable_t));
    if ((res > 0) && (enable_val == NETOPT_ENABLE)) {
        printf("  AUTOACK");
    }
    res = get(&dev->netdev, NETOPT_RAWMODE, &enable_val,
              sizeof(netopt_enable_t));
    if ((res > 0) && (enable_val == NETOPT_ENABLE)) {
        printf("  RAW");
    }
    res = get(&dev->netdev, NETOPT_AUTOCCA, &enable_val,
              sizeof(netopt_enable_t));
    if ((res > 0) && (enable_val == NETOPT_ENABLE)) {
        printf("  AUTOCCA");
    }
    res = get(&dev->netdev, NETOPT_CSMA, &enable_val,
              sizeof(netopt_enable_t));
    if ((res > 0) && (enable_val == NETOPT_ENABLE)) {
        printf("  CSMA");
    }
    puts("");

    return 0;
}

int ifconfig(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    for (unsigned int i = 0; i < AT86RF2XX_NUM; i++) {
        ifconfig_list(i);
    }
    return 0;
}

static void txtsnd_usage(char *cmd_name)
{
    printf("usage: %s <iface> [<pan>] <addr> <text>\n", cmd_name);
}

int txtsnd(int argc, char **argv)
{
    char *text;
    uint8_t addr[_MAX_ADDR_LEN];
    int iface, idx = 2;
    size_t res;
    le_uint16_t pan = { 0 };

    switch (argc) {
        case 4:
            break;
        case 5:
            res = _parse_addr((uint8_t *)&pan, sizeof(pan), argv[idx++]);
            if ((res == 0) || (res > sizeof(pan))) {
                txtsnd_usage(argv[0]);
                return 1;
            }
            pan.u16 = byteorder_swaps(pan.u16);
            break;
        default:
            txtsnd_usage(argv[0]);
            return 1;
    }

    iface = atoi(argv[1]);
    res = _parse_addr(addr, sizeof(addr), argv[idx++]);
    if (res == 0) {
        txtsnd_usage(argv[0]);
        return 1;
    }
    text = argv[idx++];
    return send(iface, pan, addr, res, text);
}

static inline int _dehex(char c, int default_)
{
    if ('0' <= c && c <= '9') {
        return c - '0';
    }
    else if ('A' <= c && c <= 'F') {
        return c - 'A' + 10;
    }
    else if ('a' <= c && c <= 'f') {
        return c - 'a' + 10;
    }
    else {
        return default_;
    }
}

static size_t _parse_addr(uint8_t *out, size_t out_len, const char *in)
{
    const char *end_str = in;
    uint8_t *out_end = out;
    size_t count = 0;
    int assert_cell = 1;

    if (!in || !*in) {
        return 0;
    }
    while (end_str[1]) {
        ++end_str;
    }

    while (end_str >= in) {
        int a = 0, b = _dehex(*end_str--, -1);
        if (b < 0) {
            if (assert_cell) {
                return 0;
            }
            else {
                assert_cell = 1;
                continue;
            }
        }
        assert_cell = 0;

        if (end_str >= in) {
            a = _dehex(*end_str--, 0);
        }

        if (++count > out_len) {
            return 0;
        }
        *out_end++ = (a << 4) | b;
    }
    if (assert_cell) {
        return 0;
    }
    /* out is reversed */

    while (out < --out_end) {
        uint8_t tmp = *out_end;
        *out_end = *out;
        *out++ = tmp;
    }

    return count;
}

static int send(int iface, le_uint16_t dst_pan, uint8_t *dst, size_t dst_len,
                char *data)
{
    int res;
    netdev_ieee802154_t *dev;
    uint8_t *src;
    size_t src_len;
    uint8_t mhr[IEEE802154_MAX_HDR_LEN];
    uint8_t flags;
    le_uint16_t src_pan;

    if (((unsigned)iface) > (AT86RF2XX_NUM - 1)) {
        printf("txtsnd: %d is not an interface\n", iface);
        return 1;
    }

    iolist_t iol_data = {
        .iol_base = data,
        .iol_len = strlen(data)
    };

    dev = &devs[iface].netdev;
    flags = (uint8_t)(dev->flags & NETDEV_IEEE802154_SEND_MASK);
    flags |= IEEE802154_FCF_TYPE_DATA;
    src_pan = byteorder_btols(byteorder_htons(dev->pan));
    if (dst_pan.u16 == 0) {
        dst_pan = src_pan;
    }
    if (dev->flags & NETDEV_IEEE802154_SRC_MODE_LONG) {
        src_len = 8;
        src = dev->long_addr;
    }
    else {
        src_len = 2;
        src = dev->short_addr;
    }
    /* fill MAC header, seq should be set by device */
    if ((res = ieee802154_set_frame_hdr(mhr, src, src_len,
                                        dst, dst_len,
                                        src_pan, dst_pan,
                                        flags, dev->seq++)) < 0) {
        puts("txtsnd: Error preperaring frame");
        return 1;
    }

    iolist_t iol_hdr = {
        .iol_next = &iol_data,
        .iol_base = mhr,
        .iol_len = (size_t)res
    };

    res = dev->netdev.driver->send(&dev->netdev, &iol_hdr);
    if (res < 0) {
        puts("txtsnd: Error on sending");
        return 1;
    }
    else {
        printf("txtsnd: send %u bytes to ", (unsigned)iol_data.iol_len);
        print_addr(dst, dst_len);
        printf(" (PAN: ");
        print_addr((uint8_t *)&dst_pan, sizeof(dst_pan));
        puts(")");
    }
    return 0;
}

/** @} */
