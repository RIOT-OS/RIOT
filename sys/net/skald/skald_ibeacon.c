/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_skald_ibeacon
 * @{
 *
 * @file
 * @brief       Skald's iBeacon implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "byteorder.h"

#include "net/skald/ibeacon.h"

#define PREFIX_LEN          (9U)

/**
 * @brief   PDU format for iBeacon packets
 */
typedef struct __attribute__((packed)) {
    uint8_t txadd[BLE_ADDR_LEN];
    uint8_t prefix[PREFIX_LEN];
    skald_uuid_t uuid;
    be_uint16_t major;
    be_uint16_t minor;
    uint8_t txpower;
} ibeacon_t;

/* constant GAP type value fields, fixed for the iBeacon format */
static const uint8_t prefix[PREFIX_LEN] = { 0x02, 0x01, 0x06, 0x1a, 0xff,
                                            0x4c, 0x00, 0x02, 0x15 };

void skald_ibeacon_advertise(skald_ctx_t *ctx, const skald_uuid_t *uuid,
                             uint16_t major, uint16_t minor, uint8_t txpower)
{
    /* configure the iBeacon PDU */
    ibeacon_t *pdu = (ibeacon_t *)ctx->pkt.pdu;

    ctx->pkt.len = (uint8_t)sizeof(ibeacon_t);

    skald_generate_random_addr(pdu->txadd);
    memcpy(pdu->prefix, prefix, PREFIX_LEN);
    memcpy(&pdu->uuid, uuid, sizeof(skald_uuid_t));
    be_uint16_t tmp = byteorder_htons(major);
    memcpy(&pdu->major, &tmp, sizeof(uint16_t));
    tmp = byteorder_htons(minor);
    memcpy(&pdu->minor, &tmp, sizeof(uint16_t));
    pdu->txpower = txpower;

    skald_adv_start(ctx);
}
