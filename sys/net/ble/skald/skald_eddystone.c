/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_skald_eddystone
 * @{
 *
 * @file
 * @brief       Skald's Eddystone implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "assert.h"
#include "net/skald/eddystone.h"

#define PREAMBLE_LEN            (11U)
#define PA_LEN                  (7U)
#define PB_LEN                  (3U)

#define URL_HDR_LEN             (6U)

#define UID_LEN                 (23U)

typedef struct __attribute__((packed)) {
    uint8_t txadd[BLE_ADDR_LEN];
    uint8_t pa[PA_LEN];
    uint8_t service_data_len;
    uint8_t pb[PB_LEN];
    uint8_t type;
} pre_t;

typedef struct __attribute__((packed)) {
    pre_t pre;
    uint8_t tx_pwr;
    uint8_t namespace[EDDYSTONE_NAMESPACE_LEN];
    uint8_t instance[EDDYSTONE_INSTANCE_LEN];
    uint8_t reserved[2];
} eddy_uid_t;

typedef struct __attribute__((packed)) {
    pre_t pre;
    uint8_t tx_pwr;
    uint8_t scheme;
    uint8_t url[];
} eddy_url_t;

/* ćonstant GAP data preamble parts, containing the following GAP fields:
 * - flags:  BR/EDR not support set
 * - complete list of 16-bit UUIDs: holding the Eddystone UUID only (0xfeaa)
 * - service data of type 0xfeaa (Eddystone) */
static const uint8_t _pa[PA_LEN] = { 0x02, 0x01, 0x04, 0x03, 0x03, 0xaa, 0xfe };
static const uint8_t _pb[PB_LEN] = { 0x16, 0xaa, 0xfe };

static void _init_pre(pre_t *data, uint8_t type, uint8_t len)
{
    skald_generate_random_addr(data->txadd);
    memcpy(data->pa, _pa, PA_LEN);
    memcpy(data->pb, _pb, PB_LEN);
    data->service_data_len = len;
    data->type = type;
}

void skald_eddystone_uid_adv(skald_ctx_t *ctx,
                             const skald_eddystone_uid_t *uid, uint8_t tx_pwr)
{
    assert(ctx && uid);

    eddy_uid_t *pdu = (eddy_uid_t *)ctx->pkt.pdu;
    _init_pre(&pdu->pre, EDDYSTONE_UID, UID_LEN);

    pdu->tx_pwr = tx_pwr;
    memcpy(pdu->namespace, uid->namespace, EDDYSTONE_NAMESPACE_LEN);
    memcpy(pdu->instance, uid->instance, EDDYSTONE_INSTANCE_LEN);
    memset(pdu->reserved, 0, 2);

    /* start advertising */
    ctx->pkt.len = sizeof(eddy_uid_t);
    skald_adv_start(ctx);
}

void skald_eddystone_url_adv(skald_ctx_t *ctx,
                             uint8_t scheme, const char *url, uint8_t tx_pwr)
{
    assert(url && ctx);
    size_t len = strlen(url);
    assert(len <= (NETDEV_BLE_PDU_MAXLEN - (URL_HDR_LEN + PREAMBLE_LEN)));

    eddy_url_t *pdu = (eddy_url_t *)ctx->pkt.pdu;
    _init_pre(&pdu->pre, EDDYSTONE_URL, (URL_HDR_LEN + len));

    /* set remaining service data fields */
    pdu->tx_pwr = tx_pwr;
    pdu->scheme = scheme;
    memcpy(pdu->url, url, len);

    /* start advertising */
    ctx->pkt.len = (sizeof(pre_t) + 2 + len);
    skald_adv_start(ctx);
}
