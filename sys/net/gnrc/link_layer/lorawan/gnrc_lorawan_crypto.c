/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  José Ignacio Alamos <jose.alamos@haw-hamburg.de>
 * @author  Francisco Molina <femolina@uc.cl>
 */
#include <stdio.h>
#include <string.h>

#include "hashes/aes128_cmac.h"
#include "crypto/ciphers.h"

#include "net/gnrc/lorawan.h"
#include "byteorder.h"
#include "net/lorawan/hdr.h"

#include "fmt.h"
#define ENABLE_DEBUG      0
#include "debug.h"

#define MIC_B0_START (0x49)
#define MIC_B1_START (0x49)
#define CRYPT_B0_START (0x01)
#define DIR_MASK (0x1)
#define SBIT_MASK (0xF)

#define APP_SKEY_B0_START       (0x2)
#define NWK_SKEY_B0_START       (0x2)
#define FNWKSINT_KEY_B0_START   (0x1)
#define SNWKSINT_KEY_B0_START   (0x3)
#define NWKSENC_KEY_B0_START    (0x4)
#define JSENC_KEY_B0_START      (0x5)
#define JSINT_KEY_B0_START      (0x6)

static aes128_cmac_context_t CmacContext;
static uint8_t digest[LORAMAC_APPKEY_LEN];
static cipher_t AesContext;

typedef struct __attribute__((packed)) {
    uint8_t fb;
    union {
        uint32_t u8_pad;
        le_uint16_t conf_fcnt;
    }__attribute__((packed));
    uint8_t dir;
    le_uint32_t dev_addr;
    le_uint32_t fcnt;
    uint8_t u32_pad;
    uint8_t len;
} lorawan_block0_t;

typedef struct  __attribute__((packed)) {
    uint8_t fb;
    le_uint16_t conf_fcnt;
    uint8_t tx_dr;
    uint8_t tx_ch;
    uint8_t dir;
    le_uint32_t dev_addr;
    le_uint32_t fcnt;
    uint8_t u32_pad;
    uint8_t len;
} lorawan_block1_t;

void gnrc_lorawan_calculate_join_req_mic(const uint8_t *buf, size_t len,
                                         uint8_t *key, le_uint32_t *out)
{
    aes128_cmac_init(&CmacContext, key, LORAMAC_APPKEY_LEN);
    aes128_cmac_update(&CmacContext, buf, len);
    aes128_cmac_final(&CmacContext, digest);

    memcpy(out, digest, sizeof(le_uint32_t));
}

void gnrc_lorawan_calculate_join_acpt_mic(const uint8_t *buf, size_t len,
                                          gnrc_lorawan_t *mac, le_uint32_t *out)
{
    uint8_t *key;

    if (IS_USED(MODULE_GNRC_LORAWAN_1_1) && gnrc_lorawan_optneg_is_set(mac)) {
        key = gnrc_lorawan_get_jsintkey(mac);
    }
    else {
        key = mac->ctx.nwksenckey;
    }

    aes128_cmac_init(&CmacContext, key, LORAMAC_JSINTKEY_LEN);

    if (IS_USED(MODULE_GNRC_LORAWAN_1_1) && gnrc_lorawan_optneg_is_set(mac)) {
        /**
         *  TODO: JoinReqType hardcoded for now. Will probably move into
         *  gnrc_lorawan_t struct once ReJoin requests are implemented.
         */
        mlme_join_req_type_t type = JOIN_REQ;
        aes128_cmac_update(&CmacContext, &type, 0x1);
        aes128_cmac_update(&CmacContext, mac->joineui, LORAMAC_JOINEUI_LEN);
        aes128_cmac_update(&CmacContext, mac->mlme.dev_nonce, GNRC_LORAWAN_DEV_NONCE_SIZE);
    }

    /* buf = HDR | JoinNonce | NetID | DevAddr | DLSettings | RxDelay | CFList */
    aes128_cmac_update(&CmacContext, buf, len);
    aes128_cmac_final(&CmacContext, digest);

    memcpy(out, digest, sizeof(le_uint32_t));
}

void gnrc_lorawan_calculate_mic_uplink(iolist_t *frame, uint16_t conf_fcnt,
                                       gnrc_lorawan_t *mac, le_uint32_t *out)
{
    lorawan_block0_t block0 = { 0 };

    block0.fb = MIC_B0_START;
    block0.dir = 0x00;

    memcpy(&block0.dev_addr, &mac->dev_addr, sizeof(le_uint32_t));

    block0.fcnt = byteorder_htoll(mac->mcps.fcnt);
    block0.len = iolist_size(frame);

    /* cmacF = aes128_cmac(FNwkSIntKey, B0 | msg) */
    aes128_cmac_init(&CmacContext, mac->ctx.fnwksintkey, LORAMAC_FNWKSINTKEY_LEN);
    aes128_cmac_update(&CmacContext, &block0, sizeof(block0));
    for (iolist_t *io = frame; io != NULL; io = io->iol_next) {
        aes128_cmac_update(&CmacContext, io->iol_base, io->iol_len);
    }
    aes128_cmac_final(&CmacContext, digest);

    if (IS_USED(MODULE_GNRC_LORAWAN_1_1) && gnrc_lorawan_optneg_is_set(mac)) {
        /* cmacF[0..1] (MIC = cmacS[0..1] | cmacF[0..1]) */
        memcpy((uint8_t *)out + 0x2, digest, 0x2);
    }
    else {
        /* MIC = cmacF[0..3] */
        memcpy(out, digest, sizeof(le_uint32_t));
    }

    if (IS_USED(MODULE_GNRC_LORAWAN_1_1) && gnrc_lorawan_optneg_is_set(mac)) {
        lorawan_block1_t block1 = { 0 };

        block1.fb = MIC_B1_START;
        block1.conf_fcnt = byteorder_htols(conf_fcnt);
        block1.tx_dr = mac->last_dr;
        block1.tx_ch = mac->last_chan_idx;
        block1.dir = 0x00;

        memcpy(&block1.dev_addr, &mac->dev_addr, sizeof(le_uint32_t));

        block1.fcnt = byteorder_htoll(mac->mcps.fcnt);
        block1.len = iolist_size(frame);

        /* cmacS = aes128_cmac(SNwkSIntKey, B1 | msg) */
        aes128_cmac_init(&CmacContext, mac->ctx.snwksintkey, LORAMAC_SNWKSINTKEY_LEN);
        aes128_cmac_update(&CmacContext, &block1, sizeof(block1));
        for (iolist_t *io = frame; io != NULL; io = io->iol_next) {
            aes128_cmac_update(&CmacContext, io->iol_base, io->iol_len);
        }
        aes128_cmac_final(&CmacContext, digest);

        /* cmacS[0..1] (MIC = cmacS[0..1] | cmacF[0..1]) */
        memcpy(out, digest, 0x2);
    }
}

void gnrc_lorawan_calculate_mic_downlink(const le_uint32_t *dev_addr,
                                         uint32_t fcnt, uint16_t conf_fcnt,
                                         iolist_t *frame,
                                         const uint8_t *snwksintkey,
                                         le_uint32_t *out)
{
    lorawan_block0_t block = { 0 };

    block.fb = MIC_B0_START;
    block.conf_fcnt =  byteorder_htols(conf_fcnt);
    block.dir = 0x1;

    memcpy(&block.dev_addr, dev_addr, sizeof(le_uint32_t));

    block.fcnt = byteorder_htoll(fcnt);
    block.len = iolist_size(frame);

    aes128_cmac_init(&CmacContext, snwksintkey, LORAMAC_SNWKSINTKEY_LEN);
    aes128_cmac_update(&CmacContext, &block, sizeof(block));
    for (iolist_t *io = frame; io != NULL; io = io->iol_next) {
        aes128_cmac_update(&CmacContext, io->iol_base, io->iol_len);
    }
    aes128_cmac_final(&CmacContext, digest);

    memcpy(out, digest, sizeof(le_uint32_t));
}

#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
void gnrc_lorawan_encrypt_fopts(uint8_t *fopts, size_t len,
                                const le_uint32_t *dev_addr, uint32_t fcnt,
                                bool afcnt, uint8_t dir, const uint8_t *key)
{
    uint8_t s_block[16] = { 0 };
    uint8_t a_block[16] = { 0 };

    lorawan_block0_t *block = (lorawan_block0_t *)a_block;

    cipher_init(&AesContext, CIPHER_AES, key, LORAMAC_APPKEY_LEN);

    block->fb = CRYPT_B0_START;

    if (afcnt) {
        a_block[0x4] = 0x02;
    }
    else {
        a_block[0x4] = 0x01;
    }

    block->dir = dir & DIR_MASK;
    block->dev_addr = *dev_addr;
    block->fcnt = byteorder_htoll(fcnt);

    block->len = 0x01;

    cipher_encrypt(&AesContext, a_block, s_block);

    for (size_t i = 0; i < len; i++) {
        fopts[i] ^= s_block[i];
    }
}
#endif /* IS_USED(MODULE_GNRC_LORAWAN_1_1) */

void gnrc_lorawan_encrypt_payload(iolist_t *iolist, const le_uint32_t *dev_addr,
                                  uint32_t fcnt, uint8_t dir,
                                  const uint8_t *appskey)
{
    uint8_t s_block[16];
    uint8_t a_block[16];

    memset(s_block, 0, sizeof(s_block));
    memset(a_block, 0, sizeof(a_block));

    lorawan_block0_t *block = (lorawan_block0_t *)a_block;

    cipher_init(&AesContext, CIPHER_AES, appskey, LORAMAC_APPKEY_LEN);

    block->fb = CRYPT_B0_START;

    block->u8_pad = 0;
    block->dir = dir & DIR_MASK;

    block->dev_addr = *dev_addr;
    block->fcnt = byteorder_htoll(fcnt);

    block->u32_pad = 0;

    int c = 0;

    for (iolist_t *io = iolist; io != NULL; io = io->iol_next) {
        for (unsigned i = 0; i < io->iol_len; i++) {
            uint8_t *v = io->iol_base;

            if ((c & SBIT_MASK) == 0) {
                block->len = (c >> 4) + 1;
                cipher_encrypt(&AesContext, a_block, s_block);
            }

            v[i] = v[i] ^ s_block[c & SBIT_MASK];
            c++;
        }
    }
}

void gnrc_lorawan_decrypt_join_accept(const uint8_t *key, uint8_t *pkt,
                                      int has_clist, uint8_t *out)
{
    cipher_init(&AesContext, CIPHER_AES, key, LORAMAC_APPKEY_LEN);
    cipher_encrypt(&AesContext, pkt, out);

    if (has_clist) {
        cipher_encrypt(&AesContext, pkt + LORAMAC_APPKEY_LEN,
                       out + LORAMAC_APPKEY_LEN);
    }
}

#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
void gnrc_lorawan_generate_lifetime_session_keys(const uint8_t *deveui,
                                                 const uint8_t *nwkkey,
                                                 uint8_t *jsintkey,
                                                 uint8_t *jsenckey)
{
    uint8_t buf[LORAMAC_JSINTKEY_LEN] = { 0 };

    cipher_init(&AesContext, CIPHER_AES, nwkkey, LORAMAC_NWKKEY_LEN);

    memcpy(buf + 1, deveui, LORAMAC_DEVEUI_LEN);

    buf[0] = JSENC_KEY_B0_START;
    cipher_encrypt(&AesContext, buf, jsenckey);

    buf[0] = JSINT_KEY_B0_START;
    cipher_encrypt(&AesContext, buf, jsintkey);
}
#endif /* IS_USED(MODULE_GNRC_LORAWAN_1_1) */

void gnrc_lorawan_generate_session_keys(const uint8_t *join_nonce,
                                        const uint8_t *dev_nonce,
                                        const uint8_t *joineui,
                                        gnrc_lorawan_t *mac)
{
    uint8_t buf[LORAMAC_APPSKEY_LEN] = { 0 };
    uint8_t *nwkkey = mac->ctx.nwksenckey;
    uint8_t *appkey = mac->ctx.appskey;

    if (IS_USED(MODULE_GNRC_LORAWAN_1_1) && gnrc_lorawan_optneg_is_set(mac)) {
        /* JoinNonce | JoinEUI | DevNonce */
        memcpy(buf + 1, join_nonce, GNRC_LORAWAN_JOIN_NONCE_SIZE);
        memcpy(buf + 1 + GNRC_LORAWAN_JOIN_NONCE_SIZE, joineui,
               LORAMAC_JOINEUI_LEN);
        memcpy(buf + 1 + GNRC_LORAWAN_JOIN_NONCE_SIZE + LORAMAC_JOINEUI_LEN,
               dev_nonce, GNRC_LORAWAN_DEV_NONCE_SIZE);
    }
    else {
        /* AppNonce | NetID | DevNonce */
        /* net_id comes right after join_nonce */
        memcpy(buf + 1, join_nonce,
               GNRC_LORAWAN_APP_NONCE_SIZE + GNRC_LORAWAN_NET_ID_SIZE);
        memcpy(buf + 1 + GNRC_LORAWAN_APP_NONCE_SIZE + GNRC_LORAWAN_NET_ID_SIZE,
               dev_nonce, GNRC_LORAWAN_DEV_NONCE_SIZE);
    }

    if (IS_USED(MODULE_GNRC_LORAWAN_1_1) && gnrc_lorawan_optneg_is_set(mac)) {
        cipher_init(&AesContext, CIPHER_AES, appkey, LORAMAC_APPKEY_LEN);

        /* derive application session key */
        buf[0] = APP_SKEY_B0_START;
        cipher_encrypt(&AesContext, buf, mac->ctx.appskey);
    }

    cipher_init(&AesContext, CIPHER_AES, nwkkey, LORAMAC_NWKKEY_LEN);

    /* derive forwarding Network session integrity key */
    buf[0] = FNWKSINT_KEY_B0_START;
    cipher_encrypt(&AesContext, buf, mac->ctx.fnwksintkey);

    if (IS_USED(MODULE_GNRC_LORAWAN_1_1) && gnrc_lorawan_optneg_is_set(mac)) {
        /* derive serving network session integrity key */
        buf[0] = SNWKSINT_KEY_B0_START;
        cipher_encrypt(&AesContext, buf, mac->ctx.snwksintkey);

        /* derive Network session encryption key */
        buf[0] = NWKSENC_KEY_B0_START;
        cipher_encrypt(&AesContext, buf, mac->ctx.nwksenckey);
    }
    else {
        /* derive application session key */
        buf[0] = APP_SKEY_B0_START;
        cipher_encrypt(&AesContext, buf, mac->ctx.appskey);

        mac->ctx.snwksintkey = mac->ctx.fnwksintkey;
        mac->ctx.nwksenckey = mac->ctx.fnwksintkey;
    }
}
/** @} */
