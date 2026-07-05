/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file
 * @brief       Crypto mode - counter with CBC-MAC
 *
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>
#include <string.h>

#include "crypto/helper.h"
#include "crypto/modes/ccm.h"
#include "crypto/modes/ctr.h"
#include "debug.h"

static inline size_t min(size_t a, size_t b)
{
    if (a < b) {
        return a;
    }

    return b;
}

static ssize_t ccm_compute_cbc_mac(const cipher_t *cipher, const uint8_t iv[16],
                                   const uint8_t *input, size_t length, uint8_t *mac)
{
    uint8_t block_size;
    uint8_t mac_enc[16] = { 0 };
    size_t offset;

    block_size = cipher_get_block_size(cipher);
    memmove(mac, iv, 16);
    offset = 0;

    /* no input message */
    if (length == 0) {
        return 0;
    }

    do {
        uint8_t block_size_input = (length - offset > block_size) ?
                                       block_size :
                                       (length - offset);

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

    return (ssize_t)offset;
}

static int ccm_create_mac_iv(const cipher_t *cipher, uint8_t auth_data_len, uint8_t M,
                             uint8_t L, const uint8_t *nonce, uint8_t nonce_len,
                             size_t plaintext_len, uint8_t X1[16])
{
    uint8_t M_;
    uint8_t L_;

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

    /* write plaintext_len to B[15..16-L] (reverse) */
    for (uint8_t i = 15; i > 16 - L - 1; --i) {
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

static int ccm_compute_adata_mac(const cipher_t *cipher, const uint8_t *auth_data,
                                 uint32_t auth_data_len, uint8_t X1[16])
{
    if (auth_data_len > 0) {
        ssize_t len;

        /* Create a block with the encoded length. Block length is always 16 */
        uint8_t auth_data_encoded[CCM_BLOCK_SIZE];
        uint8_t len_encoding = 0;

        /* If 0 < l(a) < (2^16 - 2^8), then the length field is encoded as two
         * octets. (RFC3610 page 2)
         */
        if (auth_data_len <= 0xFEFF) {
            /* length (0x0001 ... 0xFEFF)  */
            len_encoding = 2;

            auth_data_encoded[1] = auth_data_len & 0xFF;
            auth_data_encoded[0] = (auth_data_len >> 8) & 0xFF;
        }
        else {
            DEBUG("UNSUPPORTED Adata length: %" PRIu32 "\n", auth_data_len);
            return -1;
        }

        uint8_t auth_data_len_in_encoded =
            (auth_data_len >= (uint32_t)CCM_BLOCK_SIZE - len_encoding) ?
                ((uint32_t)CCM_BLOCK_SIZE - len_encoding) :
                auth_data_len;
        memcpy(auth_data_encoded + len_encoding, auth_data,
               auth_data_len_in_encoded);
        /* Calculate the MAC over the first block of AAD + heading length encoding */
        len = ccm_compute_cbc_mac(cipher, X1, auth_data_encoded,
                                  auth_data_len_in_encoded + len_encoding, X1);

        if (len < 0) {
            return -1;
        }

        /* Calculate the MAC for the remainder of the AAD (if there is one) */
        if (auth_data_len_in_encoded < auth_data_len) {
            len = ccm_compute_cbc_mac(cipher, X1,
                                      auth_data + auth_data_len_in_encoded,
                                      auth_data_len - auth_data_len_in_encoded,
                                      X1);
            if (len < 0) {
                return -1;
            }
        }
    }

    return 0;
}

/* Check if 'value' can be stored in 'num_bytes' */
static inline int _fits_in_nbytes(size_t value, uint8_t num_bytes)
{
    /* Not allowed to shift more or equal than left operand width
     * So we shift by maximum num bits of size_t -1 and compare to 1
     */
    unsigned shift = (8 * min(sizeof(size_t), num_bytes)) - 1;

    return (value >> shift) <= 1;
}

ssize_t cipher_encrypt_ccm(const cipher_t *cipher,
                           const uint8_t *auth_data, size_t auth_data_len,
                           uint8_t mac_length,
                           cipher_ccm_q_t length_encoding,
                           const uint8_t *nonce, size_t nonce_len,
                           const uint8_t *input, size_t input_len,
                           uint8_t output[input_len + mac_length])
{
    ssize_t len = -1;
    uint8_t nonce_counter[16] = { 0 };
    uint8_t mac_iv[16] = { 0 };
    uint8_t mac[16] = { 0 };
    uint8_t stream_block[16] = { 0 };
    uint8_t zero_block[16] = { 0 };
    size_t block_size;

    if (mac_length % 2 != 0 || mac_length < 4 || mac_length > 16) {
        return CCM_ERR_INVALID_MAC_LENGTH;
    }

    if (length_encoding < 2 || length_encoding > 8) {
        return CCM_ERR_INVALID_LENGTH_ENCODING;
    }

    if (!_fits_in_nbytes(input_len, length_encoding)) {
        return CCM_ERR_INVALID_DATA_LENGTH;
    }

    block_size = cipher_get_block_size(cipher);
    if (block_size != CCM_BLOCK_SIZE) {
        assert(0);
        return CCM_ERR_INVALID_BLOCK_SIZE;
    }

    /* Section A.1 "Length Requirements" from
     * https://nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38c.pdf
     * n + q == 15 */
    if (nonce_len + length_encoding != 15) {
        return CCM_ERR_INVALID_NONCE_LENGTH;
    }

    /* Create B0, encrypt it (X1) and use it as mac_iv */
    if (ccm_create_mac_iv(cipher, auth_data_len, mac_length, length_encoding,
                          nonce, nonce_len, input_len, mac_iv) < 0) {
        /* This should never happen, we tested for !_fits_in_nbytes() before */
        assert(0);
        return CCM_ERR_INVALID_DATA_LENGTH;
    }

    /* MAC calculation (T) with additional data and plaintext */
    len = ccm_compute_adata_mac(cipher, auth_data, auth_data_len, mac_iv);
    if (len < 0) {
        return len;
    }

    len = ccm_compute_cbc_mac(cipher, mac_iv, input, input_len, mac);
    if (len < 0) {
        return len;
    }

    /* Compute first stream block */
    nonce_counter[0] = length_encoding - 1;
    memcpy(&nonce_counter[1], nonce,
           min(nonce_len, (size_t)15 - length_encoding));
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

ssize_t cipher_decrypt_ccm(const cipher_t *cipher,
                           const uint8_t *auth_data, size_t auth_data_len,
                           uint8_t mac_length,
                           cipher_ccm_q_t length_encoding,
                           const uint8_t *nonce, size_t nonce_len,
                           const uint8_t *input, size_t input_len,
                           uint8_t output[input_len - mac_length])
{
    ssize_t len = -1;
    uint8_t nonce_counter[16] = { 0 };
    uint8_t mac_iv[16] = { 0 };
    uint8_t mac[16] = { 0 };
    uint8_t mac_recv[16] = { 0 };
    uint8_t stream_block[16] = { 0 };
    uint8_t zero_block[16] = { 0 };
    size_t block_size;
    size_t plain_len;

    if (mac_length % 2 != 0 || mac_length < 4 || mac_length > 16) {
        return CCM_ERR_INVALID_MAC_LENGTH;
    }

    if (length_encoding < 2 || length_encoding > 8 ||
        !_fits_in_nbytes(input_len, length_encoding)) {
        return CCM_ERR_INVALID_LENGTH_ENCODING;
    }

    block_size = cipher_get_block_size(cipher);
    if (block_size != CCM_BLOCK_SIZE) {
        assert(0);
        return CCM_ERR_INVALID_BLOCK_SIZE;
    }

    /* Section A.1 "Length Requirements" from
     * https://nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38c.pdf
     * n + q == 15 */
    if (nonce_len + length_encoding != 15) {
        return CCM_ERR_INVALID_NONCE_LENGTH;
    }

    /* Compute first stream block */
    nonce_counter[0] = length_encoding - 1;
    memcpy(&nonce_counter[1], nonce, min(nonce_len, (size_t)15 - length_encoding));
    len = cipher_encrypt_ctr(cipher, nonce_counter, block_size, zero_block,
                             block_size, stream_block);
    if (len < 0) {
        return len;
    }

    /* Decrypt message in counter mode */
    plain_len = input_len - mac_length;
    crypto_block_inc_ctr(nonce_counter, block_size - nonce_len);
    len = cipher_encrypt_ctr(cipher, nonce_counter, nonce_len, input,
                             plain_len, output);
    if (len < 0) {
        return len;
    }

    /* Create B0, encrypt it (X1) and use it as mac_iv */
    if (ccm_create_mac_iv(cipher, auth_data_len, mac_length, length_encoding,
                          nonce, nonce_len, plain_len, mac_iv) < 0) {
        return CCM_ERR_INVALID_DATA_LENGTH;
    }

    /* MAC calculation (T) with additional data and plaintext */
    len = ccm_compute_adata_mac(cipher, auth_data, auth_data_len, mac_iv);
    if (len < 0) {
        return len;
    }
    len = ccm_compute_cbc_mac(cipher, mac_iv, output, plain_len, mac);
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

    return (ssize_t)plain_len;
}
