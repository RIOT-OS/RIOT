/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for AT86RF2xx network device driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sys/uio.h"
#include "luid.h"

#include "net/netdev.h"
#include "shell.h"
#include "shell_commands.h"
#include "net/ieee802154/submac.h"
#include "net/ieee802154.h"
#include "net/netdev/ieee802154_submac.h"

#include "common.h"

#define MAX_LINE    (80)

/* Only the first radio is supported so far */
#define RADIO_DEFAULT_ID (0U)

netdev_ieee802154_submac_t netdev_submac;

void _ack_timeout(void *arg);

uint8_t buffer[IEEE802154_FRAME_LEN_MAX];
uint8_t seq;

static int print_addr(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    uint8_t *_p = (uint8_t *)&netdev_submac.submac.ext_addr;

    for (int i = 0; i < 8; i++) {
        printf("%02x", *_p++);
    }
    printf("\n");
    return 0;
}

extern const netdev_driver_t netdev_submac_driver;

static void _netdev_isr_handler(event_t *event)
{
    (void)event;
    netdev_t *netdev = &netdev_submac.dev.netdev;

    netdev->driver->isr(netdev);
}

void _print_addr(uint8_t *addr, size_t addr_len)
{
    for (size_t i = 0; i < addr_len; i++) {
        if (i != 0) {
            printf(":");
        }
        printf("%02x", (unsigned)addr[i]);
    }
}
static event_t _netdev_ev = { .handler = _netdev_isr_handler };

void recv(netdev_t *dev)
{
    uint8_t src[IEEE802154_LONG_ADDRESS_LEN], dst[IEEE802154_LONG_ADDRESS_LEN];
    int data_len;
    size_t mhr_len, src_len, dst_len;
    netdev_ieee802154_rx_info_t rx_info;
    le_uint16_t src_pan, dst_pan;

    putchar('\n');
    data_len = dev->driver->recv(dev, buffer, sizeof(buffer), &rx_info);
    if (data_len < 0) {
        puts("Couldn't read frame");
        return;
    }
    mhr_len = ieee802154_get_frame_hdr_len(buffer);
    if (mhr_len == 0) {
        puts("Unexpected MHR for incoming packet");
        return;
    }
    dst_len = ieee802154_get_dst(buffer, dst, &dst_pan);
    src_len = ieee802154_get_src(buffer, src, &src_pan);
    switch (buffer[0] & IEEE802154_FCF_TYPE_MASK) {
    case IEEE802154_FCF_TYPE_BEACON:
        puts("BEACON");
        break;
    case IEEE802154_FCF_TYPE_DATA:
        puts("DATA");
        break;
    case IEEE802154_FCF_TYPE_ACK:
        puts("ACK");
        break;
    case IEEE802154_FCF_TYPE_MACCMD:
        puts("MACCMD");
        break;
    default:
        puts("UNKNOWN");
        break;
    }
    printf("Dest. PAN: 0x%04x, Dest. addr.: ",
           byteorder_ltohs(dst_pan));
    _print_addr(dst, dst_len);
    printf("\nSrc. PAN: 0x%04x, Src. addr.: ",
           byteorder_ltohs(src_pan));
    _print_addr(src, src_len);
    printf("\nSecurity: ");
    if (buffer[0] & IEEE802154_FCF_SECURITY_EN) {
        printf("1, ");
    }
    else {
        printf("0, ");
    }
    printf("Frame pend.: ");
    if (buffer[0] & IEEE802154_FCF_FRAME_PEND) {
        printf("1, ");
    }
    else {
        printf("0, ");
    }
    printf("ACK req.: ");
    if (buffer[0] & IEEE802154_FCF_ACK_REQ) {
        printf("1, ");
    }
    else {
        printf("0, ");
    }
    printf("PAN comp.: ");
    if (buffer[0] & IEEE802154_FCF_PAN_COMP) {
        puts("1");
    }
    else {
        puts("0");
    }
    printf("Version: ");
    printf("%u, ", (unsigned)((buffer[1] & IEEE802154_FCF_VERS_MASK) >> 4));
    printf("Seq.: %u\n", (unsigned)ieee802154_get_seq(buffer));
    od_hex_dump(buffer + mhr_len, data_len - mhr_len, 0);
    printf("txt: ");
    for (int i = mhr_len; i < data_len; i++) {
        if ((buffer[i] > 0x1F) && (buffer[i] < 0x80)) {
            putchar((char)buffer[i]);
        }
        else {
            putchar('?');
        }
        if (((((i - mhr_len) + 1) % (MAX_LINE - sizeof("txt: "))) == 1) &&
            (i - mhr_len) != 0) {
            printf("\n     ");
        }
    }
    printf("\n");
    printf("RSSI: %i, LQI: %u\n\n", rx_info.rssi, rx_info.lqi);
}
static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    (void)dev;
    if (event == NETDEV_EVENT_ISR) {
        event_post(EVENT_PRIO_HIGHEST, &_netdev_ev);
    }
    else {
        switch (event) {
        case NETDEV_EVENT_RX_COMPLETE:
        {
            recv(dev);
            return;
        }
        case NETDEV_EVENT_TX_COMPLETE:
            puts("Tx complete");
            break;
        case NETDEV_EVENT_TX_COMPLETE_DATA_PENDING:
            puts("Tx complete with pending data");
            break;
        case NETDEV_EVENT_TX_MEDIUM_BUSY:
            puts("Medium Busy");
            break;
        case NETDEV_EVENT_TX_NOACK:
            puts("No ACK");
            break;
        default:
            assert(false);
        }
    }
}
static int _init(void)
{
    ieee802154_hal_test_init_devs();

    netdev_t *dev = &netdev_submac.dev.netdev;

    dev->event_callback = _event_cb;
    netdev_ieee802154_submac_init(&netdev_submac,
                                  ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID));
    dev->driver->init(dev);
    return 0;
}

uint8_t payload[] =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam ornare lacinia mi elementum interdum ligula.";

static iolist_t iol_hdr;

static int send(uint8_t *dst, size_t dst_len,
                size_t len)
{
    uint8_t flags;
    uint8_t mhr[IEEE802154_MAX_HDR_LEN];
    int mhr_len;

    le_uint16_t src_pan, dst_pan;
    iolist_t iol_data = {
        .iol_base = payload,
        .iol_len = len,
        .iol_next = NULL,
    };

    flags = IEEE802154_FCF_TYPE_DATA | 0x20;
    src_pan = byteorder_btols(byteorder_htons(0x23));
    dst_pan = byteorder_btols(byteorder_htons(0x23));
    uint8_t src_len = 8;
    void *src = &netdev_submac.submac.ext_addr;

    /* fill MAC header, seq should be set by device */
    if ((mhr_len = ieee802154_set_frame_hdr(mhr, src, src_len,
                                            dst, dst_len,
                                            src_pan, dst_pan,
                                            flags, seq++)) < 0) {
        puts("txtsnd: Error preperaring frame");
        return 1;
    }

    iol_hdr.iol_next = &iol_data;
    iol_hdr.iol_base = mhr;
    iol_hdr.iol_len = mhr_len;

    netdev_t *dev = &netdev_submac.dev.netdev;

    dev->driver->send(dev, &iol_hdr);
    return 0;
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

int txtsnd(int argc, char **argv)
{
    uint8_t addr[8];
    size_t len;
    size_t res;

    if (argc != 3) {
        puts("Usage: txtsnd <long_addr> <len>");
        return 1;
    }

    res = _parse_addr(addr, sizeof(addr), argv[1]);
    if (res == 0) {
        puts("Usage: txtsnd <long_addr> <len>");
        return 1;
    }
    len = atoi(argv[2]);
    return send(addr, res, len);
}

static const shell_command_t shell_commands[] = {
    { "print_addr", "Print IEEE802.15.4 addresses", print_addr },
    { "txtsnd", "Send IEEE 802.15.4 packet", txtsnd },
    { NULL, NULL, NULL }
};

int main(void)
{
    _init();

    /* start the shell */
    puts("Initialization successful - starting the shell now");

    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
