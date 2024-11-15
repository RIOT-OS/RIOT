/*
 * SPDX-FileCopyrightText: 2024 Martine S. Lenders
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine S. Lenders <mail@martine-lenders.eu>
 */

#include "random.h"

#include "net/skald/bthome.h"

#define AD_TYPE_FLAGS               0x01
#define AD_TYPE_SVC_DATA            0x16
#define AD_TYPE_LOCAL_NAME_SHORT    0x08
#define AD_TYPE_LOCAL_NAME_COMPLETE 0x09

#define BT_HOME_DEV_INFO_MASK       0x01
#define BT_HOME_DEV_INFO_BTHOME_V2  (2 << 5)

typedef struct __attribute__((packed)) {
    uint8_t length;
    uint8_t ad_type;
} _ad_element_hdr_t;

typedef struct __attribute__((packed)) {
    _ad_element_hdr_t hdr;
    uint8_t flags;
} _flags_t;

typedef struct __attribute__((packed)) {
    uint8_t txadd[BLE_ADDR_LEN];
    _flags_t flags;
} _bthome_msg_hdr_t;

typedef struct __attribute__((packed)) {
    /* MUST be { 0xd2, 0xfc }, see https://bthome.io/images/License_Statement_-_BTHOME.pdf */
    uint8_t uuid[2];
    uint8_t dev_info;
} _bthome_data_hdr_t;

/* flags: LE General Discoverable Mode | BR/EDR Not Supported */
#define BTHOME_FLAGS { .hdr = { .length = 0x02, .ad_type = 0x01 }, 0x06 }

static int _set_name(skald_ctx_t *ctx, const char *name, uint8_t offset, uint8_t type)
{
    if (!name) {
        return 0;
    }

    _ad_element_hdr_t *name_hdr;
    uint8_t name_len = strlen(name);
    uint8_t exp_size = (offset + name_len + sizeof(_ad_element_hdr_t));

    assert(name_len <= NETDEV_BLE_PDU_MAXLEN);
    if (exp_size >= NETDEV_BLE_PDU_MAXLEN) {
        assert(exp_size < NETDEV_BLE_PDU_MAXLEN);
        return -EMSGSIZE;
    }
    name_hdr = (_ad_element_hdr_t *)(&ctx->pkt.pdu[offset]);
    name_hdr->length = name_len + 1;
    name_hdr->ad_type = type;
    strncpy((char *)(&ctx->pkt.pdu[offset + sizeof(*name_hdr)]), name, name_len);
    return sizeof(*name_hdr) + name_len;
}

static inline int _set_shortened_name(skald_ctx_t *ctx, const char *name, uint8_t offset)
{
    return _set_name(ctx, name, offset, AD_TYPE_LOCAL_NAME_SHORT);
}

static inline int _set_complete_name(skald_ctx_t *ctx, const char *name, uint8_t offset)
{
    return _set_name(ctx, name, offset, AD_TYPE_LOCAL_NAME_COMPLETE);
}

static int _init_svc_data(skald_bthome_ctx_t *ctx, uint8_t dev_info, uint8_t offset)
{
    _ad_element_hdr_t *svc_data_hdr;
    _bthome_data_hdr_t *data_hdr;
    uint8_t exp_size = (offset + sizeof(*svc_data_hdr) + sizeof(*data_hdr));

    if (exp_size >= NETDEV_BLE_PDU_MAXLEN) {
        assert(exp_size < NETDEV_BLE_PDU_MAXLEN);
        return -EMSGSIZE;
    }
    svc_data_hdr = (_ad_element_hdr_t *)(&ctx->skald.pkt.pdu[offset]);
    data_hdr = (_bthome_data_hdr_t *)(&ctx->skald.pkt.pdu[offset + sizeof(*svc_data_hdr)]);
    svc_data_hdr->length = sizeof(*data_hdr) + 1;
    svc_data_hdr->ad_type = AD_TYPE_SVC_DATA;
    ctx->svc_data_len = &svc_data_hdr->length;
    /* https://bthome.io/images/License_Statement_-_BTHOME.pdf */
    data_hdr->uuid[0] = 0xd2;
    data_hdr->uuid[1] = 0xfc;
    data_hdr->dev_info = ((dev_info & BT_HOME_DEV_INFO_MASK) | BT_HOME_DEV_INFO_BTHOME_V2);
    return sizeof(*svc_data_hdr) + sizeof(*data_hdr);
}

int skald_bthome_init(
    skald_bthome_ctx_t *ctx,
    const char *shortened_name, const char *complete_name,
    uint8_t dev_info
)
{
    assert(ctx);

    uint8_t offset = sizeof(_bthome_msg_hdr_t);
    int res;
    static const _flags_t flags = BTHOME_FLAGS;
    _bthome_msg_hdr_t *hdr = (_bthome_msg_hdr_t *)ctx->skald.pkt.pdu;

    skald_generate_random_addr(hdr->txadd);
    hdr->flags = flags;
    res = _set_shortened_name(&ctx->skald, shortened_name, offset);
    if (res < 0) {
        return res;
    }
    offset += res;
    res = _set_complete_name(&ctx->skald, complete_name, offset);
    if (res < 0) {
        return res;
    }
    offset += res;
    res = _init_svc_data(ctx, dev_info, offset);
    if (res < 0) {
        return res;
    }
    offset += res;
    ctx->skald.pkt.len = offset;
    return offset;
}

int skald_bthome_add_measurement(
    skald_bthome_ctx_t *ctx,
    skald_bthome_id_t obj_id,
    const void *data,
    uint8_t data_len
)
{
    /* check if skald_bthome_init() was called */
    assert(
        ctx
        && (ctx->svc_data_len > (&ctx->skald.pkt.pdu[0]))
        && (ctx->svc_data_len < (&ctx->skald.pkt.pdu[NETDEV_BLE_PDU_MAXLEN]))
        && (ctx->skald.pkt.len > 0)
        && (ctx->skald.pkt.len < NETDEV_BLE_PDU_MAXLEN)
    );
    assert(data);
    bool data_contains_length = ((obj_id == BTHOME_ID_RAW) || (obj_id == BTHOME_ID_TEXT));
    uint16_t exp_size = ctx->skald.pkt.len + sizeof(obj_id) + data_len + data_contains_length;
    uint8_t offset = ctx->skald.pkt.len;

    if (exp_size >= NETDEV_BLE_PDU_MAXLEN) {
        assert(exp_size < NETDEV_BLE_PDU_MAXLEN);
        return -EMSGSIZE;
    }
    ctx->skald.pkt.pdu[offset++] = (uint8_t)obj_id;
    if (data_contains_length) {
        ctx->skald.pkt.pdu[offset++] = data_len;
    }
    memcpy(&ctx->skald.pkt.pdu[offset], data, data_len);
    offset += data_len;
    /* store diff in offset to return it later */
    *ctx->svc_data_len += offset - ctx->skald.pkt.len;
    ctx->skald.pkt.len = offset;
    return ctx->skald.pkt.len;
}

void skald_bthome_advertise(skald_bthome_ctx_t *ctx, uint32_t adv_itvl_ms)
{
    ctx->skald.adv_itvl_ms = adv_itvl_ms;
    skald_adv_start(&ctx->skald);
}

/** @} */
