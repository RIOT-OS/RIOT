/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef FIDO2_CTAP_CTAP_CRYPTO_H
#define FIDO2_CTAP_CTAP_CRYPTO_H

/**
 * @defgroup    fido2_ctap_crypto FIDO2 CTAP crypto
 * @ingroup     fido2_ctap
 * @brief       FIDO2 CTAP crypto helper
 *
 * @{
 *
 * @file
 * @brief       FIDO2 CTAP crypto helper defines, structures and function
 *              declarations.
 *
 * @author      Nils Ollrogge <nils.ollrogge@fu-berlin.de>
 */

#include <stdint.h>

#include "hashes/sha256.h"
#include "fido2/ctap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Size in bytes of cryptographic keys used
 */
#define CTAP_CRYPTO_KEY_SIZE 32

/**
 * @brief Max size of ES256 signature in ASN.1 DER format
 */
#define CTAP_CRYPTO_ES256_DER_MAX_SIZE 72

/**
 * @brief Elliptic curve public key
 */
typedef struct {
    uint8_t x[CTAP_CRYPTO_KEY_SIZE];    /**< x coordinate of curve point */
    uint8_t y[CTAP_CRYPTO_KEY_SIZE];    /**< y coordinate of curve point */
} ctap_crypto_pub_key_t;

/**
 * @brief Key agreement key
 *
 * CTAP specification (version 20190130) section 5.5.4
 */
typedef struct {
    ctap_crypto_pub_key_t pub;          /**< public key */
    uint8_t priv[CTAP_CRYPTO_KEY_SIZE]; /**< private key */
} ctap_crypto_key_agreement_key_t;

/**
 * @brief Initialize crypto helper
 *
 * Initializes crypto libs and creates key_agreement key pair
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_crypto_init(void);

/**
 * @brief Wrapper function for @ref random_bytes
 *
 * @param[in] buf   buffer to hold random bytes
 * @param[in] len   length of @p buf
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_crypto_prng(uint8_t *buf, size_t len);

/**
 * @brief Wrapper function for @ref sha256_init
 *
 * @param ctx  sha256_context_t handle to init
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_crypto_sha256_init(sha256_context_t *ctx);

/**
 * @brief Wrapper function for @ref sha256_update
 *
 * @param ctx      sha256_context_t handle to use
 * @param[in] data Input data
 * @param[in] len  Length of @p data
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_crypto_sha256_update(sha256_context_t *ctx, const void *data, size_t len);

/**
 * @brief Wrapper for @ref sha256_final
 *
 * @param ctx    sha256_context_t handle to use
 * @param digest resulting digest, this is the hash of all the bytes
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_crypto_sha256_final(sha256_context_t *ctx, void *digest);

/**
 * @brief Wrapper function for @ref sha256
 *
 * @param[in] data    pointer to the buffer to generate hash from
 * @param[in] len     length of @p data
 * @param[out] digest optional pointer to an array for the result, length must
 *                    be SHA256_DIGEST_LENGTH
 *
 * @note discards the pointer returned by @ref sha256
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_crypto_sha256(const void *data, size_t len,
                             void *digest);

/**
 * @brief Wrapper function for @ref hmac_sha256_init
 *
 * @param[in] ctx hmac_context_t handle to use
 * @param[in] key key used in the hmac-sha256 computation
 * @param[in] key_length length of @p key
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_crypto_hmac_sha256_init(hmac_context_t *ctx, const void *key,
                                       size_t key_length);

/**
 * @brief Wrapper function for @ref hmac_sha256_update
 *
 * @param[in] ctx hmac_context_t handle to use
 * @param[in] data pointer to the buffer to generate hash from
 * @param[in] len length of @p data
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_crypto_hmac_sha256_update(hmac_context_t *ctx, const void *data, size_t len);

/**
 * @brief Wrapper function for @ref hmac_sha256_final
 *
 * @param[in] ctx hmac_context_t handle to use
 * @param[out] digest the computed hmac-sha256,
 *             length MUST be SHA256_DIGEST_LENGTH
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_crypto_hmac_sha256_final(hmac_context_t *ctx, void *digest);

/**
 * @brief Wrapper function for @ref hmac_sha256
 *
 * @param[in] key key used in the hmac-sha256 computation
 * @param[in] key_length length of @p key
 * @param[in] data pointer to the buffer to generate the hmac-sha256
 * @param[in] len length of @p data
 * @param[out] digest the computed hmac-sha256,
 *             length MUST be SHA256_DIGEST_LENGTH
 *
 * @note discards the pointer returned by @ref hmac_sha256
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_crypto_hmac_sha256(const void *key,
                                  size_t key_length, const void *data, size_t len,
                                  void *digest);

/**
 * @brief Generate cryptographic key pair
 *
 * @param[in] pub_key   public key buffer
 * @param[in] priv_key  private key buffer
 * @param[in] len       length of @p priv_key
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_crypto_gen_keypair(ctap_crypto_pub_key_t *pub_key, uint8_t *priv_key, size_t len);

/**
 * @brief Elliptic-curve Diffie-Hellmann
 *
 * @param[in] out       shared secret buffer
 * @param[in] len       length of @p out
 * @param[in] pub_key   public key of other party
 * @param[in] priv_key  private key
 * @param[in] key_len   length of @p priv_key
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_crypto_ecdh(uint8_t *out, size_t len,
                           ctap_crypto_pub_key_t *pub_key, uint8_t *priv_key, size_t key_len);

/**
 * @brief Create cryptographic signature
 *
 * @param[in] hash              Hash to be signed
 * @param[in] hash_len          length of @p hash
 * @param[in] sig               signature buffer
 * @param[in] sig_len           length of @p sig
 * @param[in] key               private key to use for signature
 * @param[in] key_len           length of @p key
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_crypto_get_sig(uint8_t *hash, size_t hash_len, uint8_t *sig,
                              size_t *sig_len, const uint8_t *key, size_t key_len);

/**
 * @brief Encrypt data using AES-256-CBC
 *
 * @param[in] out              encrypted data
 * @param[in] out_len          length of @p out
 * @param[in] in               data to be encrypted
 * @param[in] in_len           length of @p in
 * @param[in] key              symmetric key to use for encryption
 * @param[in] key_len          length of @p key
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_crypto_aes_enc(uint8_t * out, size_t *out_len, uint8_t * in,
                              size_t in_len, const uint8_t * key, size_t key_len);

/**
 * @brief Decrypt data using AES-256-CBC
 *
 * @param[in] out              decrypted data
 * @param[in] out_len          length of @p out
 * @param[in] in               encrypted data
 * @param[in] in_len           len of @p in
 * @param[in] key              symmetric key to use for decryption
 * @param[in] key_len          length of @p key
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_crypto_aes_dec(uint8_t * out, size_t *out_len, uint8_t * in,
                              size_t in_len, const uint8_t * key, size_t key_len);

/**
 * @brief Encrypt data using AES-128-CCM
 *
 * @param[in] out              encrypted data
 * @param[in] out_len          length of @p out
 * @param[in] in               data to be encrypted
 * @param[in] in_len           length of @p in
 * @param[in] auth_data        additional data to authenticate in MAC
 * @param[in] auth_data_len    length of @p auth_data
 * @param[in] mac_len          length of appended MAC
 * @param[in] length_encoding  max supported length of plaintext
 * @param[in] nonce            nonce for ctr mode encryption
 * @param[in] nonce_len        length of @p nonce
 * @param[in] key              symmetric key to use for encryption
 * @param[in] key_len          length of @p key
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_crypto_aes_ccm_enc(uint8_t *out, size_t out_len,
                                  const uint8_t *in, size_t in_len,
                                  uint8_t *auth_data, size_t auth_data_len,
                                  uint8_t mac_len, uint8_t length_encoding,
                                  const uint8_t *nonce, size_t nonce_len,
                                  const uint8_t *key, size_t key_len);

/**
 * @brief Encrypt data using AES-128-CCM
 *
 * @param[in] out              encrypted data
 * @param[in] out_len          length of @p out
 * @param[in] in               data to be encrypted
 * @param[in] in_len           length of @p in
 * @param[in] auth_data        additional data to authenticate in MAC
 * @param[in] auth_data_len    length of @p auth_data
 * @param[in] mac_len          length of appended MAC
 * @param[in] length_encoding  max supported length of plaintext
 * @param[in] nonce            nonce for ctr mode encryption
 * @param[in] nonce_len        length of @p nonce
 * @param[in] key              symmetric key to use for encryption
 * @param[in] key_len          length of @p key
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_crypto_aes_ccm_dec(uint8_t *out, size_t out_len,
                                  const uint8_t *in, size_t in_len,
                                  uint8_t *auth_data, size_t auth_data_len,
                                  uint8_t mac_len, uint8_t length_encoding,
                                  const uint8_t *nonce, size_t nonce_len,
                                  const uint8_t *key, size_t key_len);
#ifdef __cplusplus
}
#endif
/** @} */
#endif /* FIDO2_CTAP_CTAP_CRYPTO_H */
