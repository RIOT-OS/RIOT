/*
 * Copyright (C) Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdio.h>

#include "at86rf2xx.h"
#include "od.h"
#include "net/ieee802154.h"
#include "net/netdev.h"

#include "common.h"

#define MAX_LINE    (80)

static uint8_t buffer[AT86RF2XX_MAX_PKT_LENGTH];

void recv(netdev_t *dev)
{
    uint8_t src[IEEE802154_LONG_ADDRESS_LEN], dst[IEEE802154_LONG_ADDRESS_LEN];
    size_t mhr_len, data_len, src_len, dst_len;
    netdev_ieee802154_rx_info_t rx_info;
    le_uint16_t src_pan, dst_pan;

    putchar('\n');
    data_len = dev->driver->recv(dev, buffer, sizeof(buffer), &rx_info);
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
    print_addr(dst, dst_len);
    printf("\nSrc. PAN: 0x%04x, Src. addr.: ",
           byteorder_ltohs(src_pan));
    print_addr(src, src_len);
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
    for (size_t i = mhr_len; i < data_len; i++) {
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

/** @} */
