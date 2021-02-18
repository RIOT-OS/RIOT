/*
 * Copyright (C) 2021 Nils Ollrogge
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup fido2_ctap
 * @{
 * @file
 *
 * @author      Nils Ollrogge <nils-ollrogge@outlook.de>
 * @}
 */

#define ENABLE_DEBUG    (0)
#include <string.h>

#include "debug.h"
#include "assert.h"

#include "crypto/ciphers.h"
#include "crypto/modes/ccm.h"
#include "crypto/modes/cbc.h"

#include "periph/hwrng.h"

#include "uECC.h"

#include "fido2/ctap/ctap_crypto.h"
#include "fido2/ctap/ctap_status.h"
#include "fido2/ctap/ctap_utils.h"

static ctap_crypto_key_agreement_key_t g_ag_key;
static int sig_to_der_format(uint8_t *r, uint8_t *s, uint8_t *sig,
                             size_t *sig_len);

int fido2_ctap_crypto_init(void)
{
    int ret;

    hwrng_init();

    /**
     * Generate authenticatorKeyAgreementKey.
     * Configuration operations upon power up CTAP specification
     * (version 20190130) section 5.5.2
     */
    return fido2_ctap_crypto_gen_keypair(&g_ag_key.pub,
                                         (uint8_t *)&g_ag_key.priv);
}

void fido2_ctap_crypto_prng(uint8_t *buf, size_t size)
{
    hwrng_read(buf, size);
}

int fido2_ctap_crypto_reset_key_agreement(void)
{
    return fido2_ctap_crypto_gen_keypair(&g_ag_key.pub,
                                         (uint8_t *)&g_ag_key.priv);
}

void fido2_ctap_crypto_get_key_agreement(ctap_crypto_pub_key_t *key)
{
    memmove(key->x, g_ag_key.pub.x, CTAP_CRYPTO_KEY_SIZE);
    memmove(key->y, g_ag_key.pub.y, CTAP_CRYPTO_KEY_SIZE);
}

int fido2_ctap_crypto_ecdh(uint8_t *out, size_t size,
                           ctap_crypto_pub_key_t *key)
{
    assert(size == CTAP_CRYPTO_KEY_SIZE);
    int ret;
    const struct uECC_Curve_t *curve = uECC_secp256r1();

    ret = uECC_shared_secret((uint8_t *)key, g_ag_key.priv, out, curve);

    if (ret == 0) {
        return CTAP1_ERR_OTHER;
    }

    return CTAP2_OK;
}

int fido2_ctap_crypto_aes_enc(uint8_t *out, size_t *out_size, uint8_t *in,
                              size_t in_size, const uint8_t *key,
                              size_t key_size)
{
    assert(*out_size >= in_size);
    int ret;
    cipher_t cipher;
    uint8_t iv[16] = { 0 };

    ret = cipher_init(&cipher, CIPHER_AES, key, key_size);

    if (ret < 0) {
        return CTAP1_ERR_OTHER;
    }

    ret = cipher_encrypt_cbc(&cipher, iv, in, in_size, out);

    if (ret < 0) {
        return CTAP1_ERR_OTHER;
    }

    return CTAP2_OK;
}

int fido2_ctap_crypto_aes_dec(uint8_t *out, size_t *out_size, uint8_t *in,
                              size_t in_size, const uint8_t *key,
                              size_t key_size)
{
    assert(*out_size >= in_size);
    int ret;
    cipher_t cipher;
    uint8_t iv[16] = { 0 };

    ret = cipher_init(&cipher, CIPHER_AES, key, key_size);

    if (ret < 0) {
        return CTAP1_ERR_OTHER;
    }

    ret = cipher_decrypt_cbc(&cipher, iv, in, in_size, out);

    if (ret < 0) {
        return CTAP1_ERR_OTHER;
    }

    return CTAP2_OK;
}

int fido2_ctap_crypto_aes_ccm_enc(uint8_t *out, size_t out_size,
                                  const uint8_t *in, size_t in_size,
                                  uint8_t *auth_data, size_t auth_data_size,
                                  uint8_t mac_size, uint8_t length_encoding,
                                  const uint8_t *nonce, size_t nonce_size,
                                  const uint8_t *key, size_t key_size)
{
    cipher_t cipher;
    int ret;

    ret = cipher_init(&cipher, CIPHER_AES_128, key, key_size);

    if (ret != 1) {
        return CTAP1_ERR_OTHER;
    }

    ret = cipher_encrypt_ccm(&cipher, auth_data, auth_data_size, mac_size,
                             length_encoding, nonce, nonce_size,
                             in, in_size, out);

    if (ret < 0) {
        return CTAP1_ERR_OTHER;
    }

    return CTAP2_OK;
}

int fido2_ctap_crypto_aes_ccm_dec(uint8_t *out, size_t out_size,
                                  const uint8_t *in, size_t in_size,
                                  uint8_t *auth_data, size_t auth_data_size,
                                  uint8_t mac_size, uint8_t length_encoding,
                                  const uint8_t *nonce, size_t nonce_size,
                                  const uint8_t *key, size_t key_size)
{
    cipher_t cipher;
    int ret, len;

    ret = cipher_init(&cipher, CIPHER_AES, key, CCM_BLOCK_SIZE);

    if (ret != 1) {
        return CTAP1_ERR_OTHER;
    }

    len = cipher_decrypt_ccm(&cipher, auth_data, auth_data_size,
                             mac_size, length_encoding, nonce, nonce_size,
                             in, in_size, out);

    if (len < 0) {
        return CTAP1_ERR_OTHER;
    }

    return CTAP2_OK;
}

int fido2_ctap_crypto_gen_keypair(ctap_crypto_pub_key_t *pub_key,
                                  uint8_t *priv_key)
{
    int ret;
    const struct uECC_Curve_t *curve = uECC_secp256r1();

    ret = uECC_make_key((uint8_t *)pub_key, priv_key, curve);
    if (ret == 0) {
        return CTAP1_ERR_OTHER;
    }

    return CTAP2_OK;
}

int fido2_ctap_crypto_get_sig(uint8_t *hash, size_t hash_size, uint8_t *sig,
                              size_t *sig_size, const uint8_t *key,
                              size_t key_size)
{
    assert(*sig_size >= CTAP_CRYPTO_ES256_DER_MAX_SIZE);

    uint8_t r_raw[CTAP_CRYPTO_KEY_SIZE];
    uint8_t s_raw[CTAP_CRYPTO_KEY_SIZE];
    int ret;

    const struct uECC_Curve_t *curve = uECC_secp256r1();

    ret = uECC_sign(key, hash, hash_size, sig, curve);

    if (ret == 0) {
        return CTAP1_ERR_OTHER;
    }

    memmove(r_raw, sig, sizeof(r_raw));
    memmove(s_raw, sig + sizeof(r_raw), sizeof(s_raw));

    ret = sig_to_der_format(r_raw, s_raw, sig, sig_size);

    if (ret != CTAP2_OK) {
        return ret;
    }

    return CTAP2_OK;
}

/* Encode signature in ASN.1 DER format */
static int sig_to_der_format(uint8_t *r, uint8_t *s, uint8_t *sig,
                             size_t *sig_len)
{
    uint8_t offset = 0;
    uint8_t lead_r = 0;
    uint8_t lead_s = 0;
    uint8_t pad_s, pad_r;
    uint8_t i;

    if (*sig_len < CTAP_CRYPTO_ES256_DER_MAX_SIZE) {
        return CTAP1_ERR_OTHER;
    }

    /* get leading zeros to save space */
    for (i = 0; i < CTAP_CRYPTO_KEY_SIZE; i++) {
        if (r[i] == 0) {
            lead_r++;
        }
        else {
            break;
        }
    }

    for (i = 0; i < CTAP_CRYPTO_KEY_SIZE; i++) {
        if (s[i] == 0) {
            lead_s++;
        }
        else {
            break;
        }
    }

    /*
       if number is negative after removing leading zeros,
       pad with 1 zero byte in order to turn number positive again
     */
    pad_r = ((r[lead_r] & 0x80) == 0x80);
    pad_s = ((s[lead_s] & 0x80) == 0x80);

    memset(sig, 0, *sig_len);

    /* sequence tag number, constructed method */
    sig[offset++] = 0x30;

    /* length octet (number of content octets) */
    sig[offset++] = 0x44 + pad_r + pad_s - lead_r - lead_s;

    /* integer tag number */
    sig[offset++] = 0x02;
    sig[offset + pad_r] = 0x00;
    sig[offset++] = 0x20 + pad_r - lead_r;
    offset += pad_r;

    memmove(sig + offset, r + lead_r, 32 - lead_r);

    offset += 32 - lead_r;

    /* integer tag number */
    sig[offset++] = 0x02;
    sig[offset] = 0x00;
    sig[offset++] = 0x20 + pad_s - lead_s;
    offset += pad_s;

    memmove(sig + offset, s + lead_s, 32 - lead_s);

    offset += 32 - lead_s;

    *sig_len = offset;

    return CTAP2_OK;
}
