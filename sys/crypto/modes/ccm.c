/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_crypto_modes
 * @{
 *
 * @file
 * @brief       Crypto mode - counter with CBC-MAC
 *
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 *
 * @}
 */

#include <string.h>
#include "debug.h"
#include "crypto/helper.h"
#include "crypto/modes/ctr.h"
#include "crypto/modes/ccm.h"

static inline int min(int a, int b)
{
    if (a < b) {
        return a;
    }
    else {
        return b;
    }
}

int ccm_compute_cbc_mac(cipher_t* cipher, uint8_t iv[16],
                        uint8_t* input, size_t length, uint8_t* mac)
{
    uint8_t offset, block_size, mac_enc[16] = {0};

    block_size = cipher_get_block_size(cipher);
    memcpy(mac, iv, 16);
    offset = 0;
    do {
        uint8_t block_size_input = (length - offset > block_size) ?
                                   block_size : length - offset;

        /* CBC-Mode: XOR plaintext with ciphertext of (n-1)-th block */
        for (int i = 0; i < block_size_input; ++i) {
            mac[i] ^= input[offset + i];
        }

        if (cipher_encrypt(cipher, mac, mac_enc) != 1) {
            return CIPHER_ERR_ENC_FAILED;
        }

        memcpy(mac, mac_enc, block_size);
        offset += block_size_input;
    } while (offset < length);

    return offset;
}


int ccm_create_mac_iv(cipher_t* cipher, uint8_t auth_data_len, uint8_t M,
                      uint8_t L, uint8_t* nonce, uint8_t nonce_len,
                      size_t plaintext_len, uint8_t X1[16])
{
    uint8_t M_, L_;

    /* ensure everything is set to zero */
    memset(X1, 0, 16);

    /* set flags in B[0] - bit format:
            7        6     5..3  2..0
        Reserved   Adata    M_    L_    */
    M_ = (M - 2) / 2;
    L_ = L - 1;
    X1[0] = 64 * (auth_data_len > 0) + 8 * M_ + L_;

    /* copy nonce to B[1..15-L] */
    memcpy(&X1[1], nonce, min(nonce_len, 15 - L));

    /* write plaintext_len to B[15..16-L] */
    for (uint8_t i = 15; i > 16 - L; --i) {
        X1[i] = plaintext_len & 0xff;
        plaintext_len >>= 8;
    }

    /* if there is still data, plaintext_len was too big */
    if (plaintext_len > 0) {
        return CIPHER_ERR_INVALID_LENGTH;
    }

    if (cipher_encrypt(cipher, X1, X1) != 1) {
        return CIPHER_ERR_ENC_FAILED;
    }
    return 0;
}

int ccm_compute_adata_mac(cipher_t* cipher, uint8_t* auth_data,
                          uint32_t auth_data_len, uint8_t X1[16])
{
    if (auth_data_len > 0) {
        int len;

        /* 16 octet block size + max. 10 len encoding  */
        uint8_t auth_data_encoded[26], len_encoding = 0;

        if ( auth_data_len < (((uint32_t) 2) << 16)) {       /* length (0x0001 ... 0xFEFF)  */
            len_encoding = 2;

            auth_data_encoded[1] = auth_data_len & 0xFF;
            auth_data_encoded[0] = (auth_data_len >> 8) & 0xFF;
        } else {
            DEBUG("UNSUPPORTED Adata length");
            return -1;
        }

        memcpy(auth_data_encoded + len_encoding, auth_data, auth_data_len);
        len = ccm_compute_cbc_mac(cipher, X1, auth_data_encoded, auth_data_len + len_encoding, X1);
        if (len < 0) {
            return -1;
        }
    }

    return 0;
}


int cipher_encrypt_ccm(cipher_t* cipher, uint8_t* auth_data, uint32_t auth_data_len,
                       uint8_t mac_length, uint8_t length_encoding,
                       uint8_t* nonce, size_t nonce_len,
                       uint8_t* input, size_t input_len,
                       uint8_t* output)
{
    int len = -1;
    uint32_t length_max;
    uint8_t nonce_counter[16] = {0}, mac_iv[16] = {0}, mac[16] = {0},
                                stream_block[16] = {0}, zero_block[16] = {0}, block_size;

    if (mac_length % 2 != 0  || mac_length < 4 || mac_length > 16) {
        return CCM_ERR_INVALID_MAC_LENGTH;
    }

    length_max = 2 << (8 * length_encoding);
    if (length_encoding < 2 || length_encoding > 8 ||
            input_len - auth_data_len > length_max) {
        return CCM_ERR_INVALID_LENGTH_ENCODING;
    }

    /* Create B0, encrypt it (X1) and use it as mac_iv */
    block_size = cipher_get_block_size(cipher);
    if (ccm_create_mac_iv(cipher, auth_data_len, mac_length, length_encoding,
                          nonce, nonce_len, input_len, mac_iv) < 0) {
        return CCM_ERR_INVALID_DATA_LENGTH;
    }

    /* MAC calulation (T) with additional data and plaintext */
    ccm_compute_adata_mac(cipher, auth_data, auth_data_len, mac_iv);
    len = ccm_compute_cbc_mac(cipher, mac_iv, input, input_len, mac);
    if (len < 0) {
        return len;
    }

    /* Compute first stream block */
    nonce_counter[0] = length_encoding - 1;
    memcpy(&nonce_counter[1], nonce,
           min(nonce_len, (size_t) 15 - length_encoding));
    len = cipher_encrypt_ctr(cipher, nonce_counter, block_size,
                             zero_block, block_size, stream_block);
    if (len < 0) {
        return len;
    }

    /* Encrypt message in counter mode  */
    crypto_block_inc_ctr(nonce_counter, block_size - nonce_len);
    len = cipher_encrypt_ctr(cipher, nonce_counter, nonce_len, input,
                             input_len, output);
    if (len < 0) {
        return len;
    }

    /* auth value: mac ^ first stream block */
    for (uint8_t i = 0; i < mac_length; ++i) {
        output[len + i] = mac[i] ^ stream_block[i];
    }

    return len + mac_length;
}


int cipher_decrypt_ccm(cipher_t* cipher, uint8_t* auth_data,
                       uint32_t auth_data_len, uint8_t mac_length,
                       uint8_t length_encoding, uint8_t* nonce, size_t nonce_len,
                       uint8_t* input, size_t input_len, uint8_t* plain)
{
    int len = -1;
    uint32_t length_max;
    uint8_t nonce_counter[16] = {0}, mac_iv[16] = {0}, mac[16] = {0},
                                mac_recv[16] = {0}, stream_block[16] = {0}, zero_block[16] = {0},
                                        plain_len, block_size;

    if (mac_length % 2 != 0  || mac_length < 4 || mac_length > 16) {
        return CCM_ERR_INVALID_MAC_LENGTH;
    }

    length_max = 2 << (8 * length_encoding);
    if (length_encoding < 2 || length_encoding > 8 ||
            input_len - auth_data_len > length_max) {
        return CCM_ERR_INVALID_LENGTH_ENCODING;
    }

    /* Compute first stream block */
    nonce_counter[0] = length_encoding - 1;
    block_size = cipher_get_block_size(cipher);
    memcpy(&nonce_counter[1], nonce, min(nonce_len, (size_t) 15 - length_encoding));
    len = cipher_encrypt_ctr(cipher, nonce_counter, block_size, zero_block,
                             block_size, stream_block);
    if (len < 0) {
        return len;
    }

    /* Decrypt message in counter mode */
    plain_len = input_len - mac_length;
    crypto_block_inc_ctr(nonce_counter, block_size - nonce_len);
    len = cipher_encrypt_ctr(cipher, nonce_counter, nonce_len, input,
                             plain_len, plain);
    if (len < 0) {
        return len;
    }

    /* Create B0, encrypt it (X1) and use it as mac_iv */
    if (ccm_create_mac_iv(cipher, auth_data_len, mac_length, length_encoding,
                          nonce, nonce_len, plain_len, mac_iv) < 0) {
        return CCM_ERR_INVALID_DATA_LENGTH;
    }

    /* MAC calulation (T) with additional data and plaintext */
    ccm_compute_adata_mac(cipher, auth_data, auth_data_len, mac_iv);
    len = ccm_compute_cbc_mac(cipher, mac_iv, plain, plain_len, mac);
    if (len < 0) {
        return len;
    }

    /* mac = input[plain_len...plain_len+mac_length] ^ first stream block */
    for (uint8_t i = 0; i < mac_length; ++i) {
        mac_recv[i] = input[len + i] ^ stream_block[i];
    }

    if (!crypto_equals(mac_recv, mac, mac_length)) {
        return CCM_ERR_INVALID_CBC_MAC;
    }

    return plain_len;
}
