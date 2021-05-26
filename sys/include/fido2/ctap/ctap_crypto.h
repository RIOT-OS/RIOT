/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     fido2_ctap
 * @brief       FIDO2 CTAP crypto helper
 *
 * @{
 *
 * @file
 * @brief       FIDO2 CTAP crypto helper defines, structures and function
 *              declarations.
 *
 * @author      Nils Ollrogge <nils-ollrogge@outlook.de>
 */

#ifndef FIDO2_CTAP_CTAP_CRYPTO_H
#define FIDO2_CTAP_CTAP_CRYPTO_H

#include <stdint.h>

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
 */
int fido2_ctap_crypto_init(void);

/**
 * @brief Pseudorandom number generator
 *
 * Wrapper for hwrng_read
 *
 * @param[in] buf   buffer to hold random bytes
 * @param[in] size  size of buffer
 */
void fido2_ctap_crypto_prng(uint8_t *buf, size_t size);

/**
 * @brief Recreate key_agreement key pair
 *
 * @return CTAP status code
 */
int fido2_ctap_crypto_reset_key_agreement(void);

/**
 * @brief Get public key of key_agreement key pair
 *
 * @param[in] key  public key buffer
 */
void fido2_ctap_crypto_get_key_agreement(ctap_crypto_pub_key_t *key);

/**
 * @brief Generate cryptographic key pair
 *
 * @param[in] pub_key   public key buffer
 * @param[in] priv_key  private key buffer
 *
 * @return CTAP status code
 */
int fido2_ctap_crypto_gen_keypair(ctap_crypto_pub_key_t *pub_key, uint8_t *priv_key);

/**
 * @brief Elliptic-curve Diffie-Hellmann
 *
 * @param[in] out   shared secret buffer
 * @param[in] size   size of shared secret buffer
 * @param[in] key   public key of other party
 *
 * @return CTAP status code
 */
int fido2_ctap_crypto_ecdh(uint8_t *out, size_t size, ctap_crypto_pub_key_t *key);

/**
 * @brief Create cryptographic signature
 *
 * @param[in] hash              Hash to be signed
 * @param[in] hash_len          size of hash
 * @param[in] sig               signature buffer
 * @param[in] sig_size          size of signature
 * @param[in] key               private key to use for signature
 * @param[in] key_size          size of key
 *
 * @return CTAP status code
 */
int fido2_ctap_crypto_get_sig(uint8_t *hash, size_t hash_len, uint8_t *sig,
                              size_t *sig_size, const uint8_t *key, size_t key_size);

/**
 * @brief Encrypt data using AES-256-CBC
 *
 * @param[in] out              encrypted data
 * @param[in] out_size         size of encrypted data
 * @param[in] in               data to be encrypted
 * @param[in] in_size          size of data to be encrypted
 * @param[in] key              symmetric key to use for encryption
 * @param[in] key_size         size of key
 *
 * @return CTAP status code
 */
int fido2_ctap_crypto_aes_enc(uint8_t * out, size_t *out_size, uint8_t * in,
                              size_t in_size, const uint8_t * key, size_t key_size);

/**
 * @brief Decrypt data using AES-256-CBC
 *
 * @param[in] out              decrypted data
 * @param[in] out_size         size of decrypted data
 * @param[in] in               encrypted data
 * @param[in] in_size          size of encrypted data
 * @param[in] key              symmetric key to use for decryption
 * @param[in] key_size         size of key
 *
 * @return CTAP status code
 */
int fido2_ctap_crypto_aes_dec(uint8_t * out, size_t *out_size, uint8_t * in,
                              size_t in_size, const uint8_t * key, size_t key_size);

/**
 * @brief Encrypt data using AES-128-CCM
 *
 * @param[in] out              encrypted data
 * @param[in] out_size         size of encrypted data
 * @param[in] in               data to be encrypted
 * @param[in] in_size          size of data to be encrypted buffer
 * @param[in] auth_data        additional data to authenticate in MAC
 * @param[in] auth_data_size   size of auth_data
 * @param[in] mac_size         size of appended MAC
 * @param[in] length_encoding  max supported length of plaintext
 * @param[in] nonce            nonce for ctr mode encryption
 * @param[in] nonce_size       size of nonce
 * @param[in] key              symmetric key to use for encryption
 * @param[in] key_size         size of key
 *
 * @return CTAP status code
 */
int fido2_ctap_crypto_aes_ccm_enc(uint8_t *out, size_t out_size,
                                  const uint8_t *in, size_t in_size,
                                  uint8_t *auth_data, size_t auth_data_size,
                                  uint8_t mac_size, uint8_t length_encoding,
                                  const uint8_t *nonce, size_t nonce_size,
                                  const uint8_t *key, size_t key_size);

/**
 * @brief Encrypt data using AES-128-CCM
 *
 * @param[in] out              encrypted data
 * @param[in] out_size         size of encrypted data
 * @param[in] in               data to be encrypted
 * @param[in] in_size          size of data to be encrypted buffer
 * @param[in] auth_data        additional data to authenticate in MAC
 * @param[in] auth_data_size   size of auth_data
 * @param[in] mac_size         size of appended MAC
 * @param[in] length_encoding  max supported length of plaintext
 * @param[in] nonce            nonce for ctr mode encryption
 * @param[in] nonce_size       size of nonce
 * @param[in] key              symmetric key to use for encryption
 * @param[in] key_size         size of key
 *
 * @return CTAP status code
 */
int fido2_ctap_crypto_aes_ccm_dec(uint8_t *out, size_t out_size,
                                  const uint8_t *in, size_t in_size,
                                  uint8_t *auth_data, size_t auth_data_size,
                                  uint8_t mac_size, uint8_t length_encoding,
                                  const uint8_t *nonce, size_t nonce_size,
                                  const uint8_t *key, size_t key_size);
#ifdef __cplusplus
}
#endif
#endif /* FIDO2_CTAP_CTAP_CRYPTO_H */
/** @} */
