/*
 * Copyright (C) 2021 Freie Universität Berlin
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
#include "random.h"

#include "crypto/ciphers.h"
#include "crypto/modes/ccm.h"
#include "crypto/modes/cbc.h"

#include "uECC.h"
#include "tiny-asn1.h"

#include "fido2/ctap/ctap_crypto.h"
#include "fido2/ctap/ctap_status.h"
#include "fido2/ctap/ctap_utils.h"

static ctap_crypto_key_agreement_key_t g_ag_key;
static int sig_to_der_format(uint8_t *r, uint8_t *s, uint8_t *sig,
                             size_t *sig_len);
static int RNG(uint8_t *dest, size_t size);

int fido2_ctap_crypto_init(void)
{

    uECC_set_rng(&RNG);
    /**
     * Generate authenticatorKeyAgreementKey.
     *
     * Configuration operations upon power up CTAP specification
     * (version 20190130) section 5.5.2
     */
    return fido2_ctap_crypto_reset_key_agreement();
}

void fido2_ctap_crypto_prng(uint8_t *buf, size_t size)
{
    random_bytes(buf, size);
}

void *fido2_ctap_crypto_sha256(const void *data, size_t len,
                               void *digest)
{
    return sha256(data, len, digest);
}

void fido2_ctap_crypto_hmac_sha256_init(hmac_context_t *ctx, const void *key,
                                        size_t key_length)
{
    return hmac_sha256_init(ctx, key, key_length);
}

void fido2_ctap_crypto_hmac_sha256_update(hmac_context_t *ctx, const void *data, size_t len)
{
    return hmac_sha256_update(ctx, data, len);
}

void fido2_ctap_crypto_hmac_sha256_final(hmac_context_t *ctx, void *digest)
{
    return hmac_sha256_final(ctx, digest);
}

const void *fido2_ctap_crypto_hmac_sha256(const void *key,
                                          size_t key_length, const void *data, size_t len,
                                          void *digest)
{
    return hmac_sha256(key, key_length, data, len, digest);
}

int fido2_ctap_crypto_reset_key_agreement(void)
{
    return fido2_ctap_crypto_gen_keypair(&g_ag_key.pub,
                                         (uint8_t *)&g_ag_key.priv);
}

void fido2_ctap_crypto_get_pub_key_agreement_key(ctap_crypto_pub_key_t *key)
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

    /**
     * +1 to pad with leading zero to prevent integer from being interpreted as
     * negative (e.g. MSB of r >= 0x80)
     */
    uint8_t r[CTAP_CRYPTO_KEY_SIZE + 1] = { 0 };
    uint8_t s[CTAP_CRYPTO_KEY_SIZE + 1] = { 0 };
    int ret;

    const struct uECC_Curve_t *curve = uECC_secp256r1();

    ret = uECC_sign(key, hash, hash_size, sig, curve);

    if (ret == 0) {
        return CTAP1_ERR_OTHER;
    }

    memmove(r + 1, sig, CTAP_CRYPTO_KEY_SIZE);
    memmove(s + 1, sig + CTAP_CRYPTO_KEY_SIZE, CTAP_CRYPTO_KEY_SIZE);

    ret = sig_to_der_format(r, s, sig, sig_size);

    if (ret != CTAP2_OK) {
        return ret;
    }

    return CTAP2_OK;
}

static int RNG(uint8_t *dest, size_t size)
{
    fido2_ctap_crypto_prng(dest, size);
    return 1;
}

static int sig_to_der_format(uint8_t *r, uint8_t *s, uint8_t *sig,
                             size_t *sig_len)
{
    asn1_tree t;
    asn1_tree c1;
    asn1_tree c2;
    uint8_t pad_s, pad_r;
    int ret;

    /**
     * if MSB >= 0x80, pad with leading zero byte in order to have number
     * interpreted as positive.
     */
    pad_r = ((r[1] & 0x80) == 0x80);
    pad_s = ((s[1] & 0x80) == 0x80);

    memset(sig, 0, *sig_len);

    list_init(&t);
    list_init(&c1);
    list_init(&c2);

    t.type = ASN1_TYPE_SEQUENCE;

    c1.type = ASN1_TYPE_INTEGER;
    c1.length = 0x20 + pad_r;
    c1.data = pad_r ? r : r + 1;

    ret = add_child(&t, &c1);

    if (ret < 0) {
        return CTAP1_ERR_OTHER;
    }

    c2.type = ASN1_TYPE_INTEGER;
    c2.length = 0x20 + pad_s;
    c2.data = pad_s ? s : s + 1;

    ret = add_child(&t, &c2);

    if (ret < 0) {
        return CTAP1_ERR_OTHER;
    }

    *sig_len = der_encode(&t, sig, *sig_len);

    return CTAP2_OK;
}
