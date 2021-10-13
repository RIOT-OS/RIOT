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

#include "hashes/cmac.h"
#include "crypto/ciphers.h"

#include "net/gnrc/lorawan.h"
#include "byteorder.h"
#include "net/lorawan/hdr.h"

#define MIC_B0_START (0x49)
#define CRYPT_B0_START (0x01)
#define DIR_MASK (0x1)
#define SBIT_MASK (0xF)

#define APP_SKEY_B0_START (0x1)
#define NWK_SKEY_B0_START (0x2)

static cmac_context_t CmacContext;
static uint8_t digest[LORAMAC_APPKEY_LEN];
static cipher_t AesContext;

typedef struct  __attribute__((packed)) {
    uint8_t fb;
    uint32_t u8_pad;
    uint8_t dir;
    le_uint32_t dev_addr;
    le_uint32_t fcnt;
    uint8_t u32_pad;
    uint8_t len;
} lorawan_block_t;

void gnrc_lorawan_calculate_join_mic(const uint8_t *buf, size_t len,
                                     const uint8_t *key, le_uint32_t *out)
{
    cmac_init(&CmacContext, key, LORAMAC_APPKEY_LEN);
    cmac_update(&CmacContext, buf, len);
    cmac_final(&CmacContext, digest);

    memcpy(out, digest, sizeof(le_uint32_t));
}

void gnrc_lorawan_calculate_mic(const le_uint32_t *dev_addr, uint32_t fcnt,
                                uint8_t dir, iolist_t *frame,
                                const uint8_t *nwkskey, le_uint32_t *out)
{
    lorawan_block_t block;

    block.fb = MIC_B0_START;
    block.u8_pad = 0;
    block.dir = dir & DIR_MASK;

    memcpy(&block.dev_addr, dev_addr, sizeof(le_uint32_t));

    block.fcnt = byteorder_htoll(fcnt);

    block.u32_pad = 0;

    block.len = iolist_size(frame);

    cmac_init(&CmacContext, nwkskey, LORAMAC_APPKEY_LEN);
    cmac_update(&CmacContext, &block, sizeof(block));
    for (iolist_t *io = frame; io != NULL; io = io->iol_next) {
        cmac_update(&CmacContext, io->iol_base, io->iol_len);
    }
    cmac_final(&CmacContext, digest);

    memcpy(out, digest, sizeof(le_uint32_t));
}

void gnrc_lorawan_encrypt_payload(iolist_t *iolist, const le_uint32_t *dev_addr,
                                  uint32_t fcnt, uint8_t dir,
                                  const uint8_t *appskey)
{
    uint8_t s_block[16];
    uint8_t a_block[16];

    memset(s_block, 0, sizeof(s_block));
    memset(a_block, 0, sizeof(a_block));

    lorawan_block_t *block = (lorawan_block_t *)a_block;

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

void gnrc_lorawan_generate_session_keys(const uint8_t *app_nonce,
                                        const uint8_t *dev_nonce,
                                        const uint8_t *appkey, uint8_t *nwkskey,
                                        uint8_t *appskey)
{
    uint8_t buf[LORAMAC_APPSKEY_LEN];

    memset(buf, 0, sizeof(buf));

    cipher_init(&AesContext, CIPHER_AES, appkey, LORAMAC_APPSKEY_LEN);

    /* net_id comes right after app_nonce */
    memcpy(buf + 1, app_nonce,
           GNRC_LORAWAN_APP_NONCE_SIZE + GNRC_LORAWAN_NET_ID_SIZE);
    memcpy(buf + 1 + GNRC_LORAWAN_APP_NONCE_SIZE + GNRC_LORAWAN_NET_ID_SIZE,
           dev_nonce, GNRC_LORAWAN_DEV_NONCE_SIZE);

    /* Calculate Application Session Key */
    buf[0] = APP_SKEY_B0_START;
    cipher_encrypt(&AesContext, buf, nwkskey);

    /* Calculate Network Session Key */
    buf[0] = NWK_SKEY_B0_START;
    cipher_encrypt(&AesContext, buf, appskey);
}

/** @} */
