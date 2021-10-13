/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       RIOT adaption definition of the "cryptoengine" module
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "opendefs.h"

#include "crypto/ciphers.h"
#include "crypto/modes/ecb.h"
#include "crypto/modes/ccm.h"

/* CCM does not restrict the input message size, but these function will
   only be called with a message length of at most 125b in OpenWSN */
#define MAX_MESSAGE_LEN   (125U)

owerror_t cryptoengine_aes_ccms_enc(uint8_t *a, uint8_t len_a, uint8_t *m,
                                    uint8_t *len_m, uint8_t *nonce, uint8_t l,
                                    uint8_t *key, uint8_t len_mac)
{
    cipher_t cipher;
    int ret, len;
    uint8_t tmp_buff[MAX_MESSAGE_LEN + CCM_MAC_MAX_LEN];

    ret = cipher_init(&cipher, CIPHER_AES, key, CCM_BLOCK_SIZE);

    if (ret != 1) {
        return E_FAIL;
    }

    len = cipher_encrypt_ccm(&cipher, a, len_a, len_mac, l,
                             nonce, 15 - l, m, *len_m, tmp_buff);
    if (len < 0) {
        return E_FAIL;
    }

    *len_m = len;
    memcpy(m, tmp_buff, *len_m);

    return E_SUCCESS;
}

owerror_t cryptoengine_aes_ccms_dec(uint8_t *a, uint8_t len_a, uint8_t *m,
                                    uint8_t *len_m, uint8_t *nonce, uint8_t l,
                                    uint8_t *key, uint8_t len_mac)
{
    cipher_t cipher;
    int ret, len;
    uint8_t tmp_buff[MAX_MESSAGE_LEN];

    ret = cipher_init(&cipher, CIPHER_AES, key, CCM_BLOCK_SIZE);

    if (ret != 1) {
        return E_FAIL;
    }

    len = cipher_decrypt_ccm(&cipher, a, len_a, len_mac, l, nonce, 15 - l, m,
                             *len_m, tmp_buff);

    if (len < 0) {
        return E_FAIL;
    }

    *len_m = len;
    memcpy(m, tmp_buff, *len_m);

    return E_SUCCESS;
}

owerror_t cryptoengine_aes_ecb_enc(uint8_t *buffer, uint8_t *key)
{
    cipher_t cipher;
    int ret, len;

    ret = cipher_init(&cipher, CIPHER_AES, key, CCM_BLOCK_SIZE);

    if (ret != 1) {
        return E_FAIL;
    }

    len = cipher_encrypt_ecb(&cipher, buffer, CCM_BLOCK_SIZE, buffer);

    if (len < 0) {
        return E_FAIL;
    }

    return E_SUCCESS;
}
