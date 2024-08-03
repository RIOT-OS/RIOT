/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_crypto_chacha20poly1305 chacha20poly1305 AEAD cipher
 * @ingroup     sys_crypto
 * @brief       Provides RFC 8439 style chacha20poly1305
 *
 * This module provides the chacha20poly1305 AEAD symmetric cipher following
 * [rfc 8439](https://tools.ietf.org/html/rfc8439).
 *
 * Nonces must be unique per message for a single key. They are allowed to be
 * predictable, e.g. a message counter and are allowed to be visible during
 * transmission.
 * @{
 *
 * @file
 * @brief       Chacha20poly1305 functions
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef CRYPTO_CHACHA20POLY1305_H
#define CRYPTO_CHACHA20POLY1305_H

#include "crypto/poly1305.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CHACHA20POLY1305_KEY_BYTES      (32U)   /**< Key length in bytes */
#define CHACHA20POLY1305_NONCE_BYTES    (12U)   /**< Nonce length in bytes */
#define CHACHA20POLY1305_TAG_BYTES      (16U)   /**< Tag length in bytes */

/**
 * @brief Chacha20poly1305 state struct
 */
typedef union {
    /* We need both the state matrix and the poly1305 state, but nearly not at
     * the same time. This works as long as the first 8 members of state
     * overlap fully or completely not with the first and second key parts
     * from the @ref poly1305_ctx_t struct */
    uint32_t state[16];     /**< The current state of the key stream. */
    poly1305_ctx_t poly;    /**< Poly1305 state for the MAC */
} chacha20poly1305_ctx_t;

/**
 * @brief Encrypt a plaintext to ciphertext and append a tag to protect the
 * ciphertext and additional data.
 *
 * It is allowed to have cipher == msg as long
 * as there is @ref CHACHA20POLY1305_TAG_BYTES space left to hold the
 * authentication tag
 *
 *
 * @param[out]  cipher      resulting ciphertext, is CHACHA20POLY1305_TAG_BYTES
 *                          longer than the message length
 * @param[in]   msg         message to encrypt
 * @param[in]   msglen      length in bytes of the message
 * @param[in]   aad         additional authenticated data to protect
 * @param[in]   aadlen      length of the additional authenticated data
 * @param[in]   key         key to encrypt with, must be
 *                          CHACHA20POLY1305_KEY_BYTES long
 * @param[in]   nonce       Nonce to use. Must be CHACHA20POLY1305_NONCE_BYTES
 *                          long
 */
void chacha20poly1305_encrypt(uint8_t *cipher, const uint8_t *msg,
                              size_t msglen, const uint8_t *aad, size_t aadlen,
                              const uint8_t *key, const uint8_t *nonce);

/**
 * @brief Verify the tag and decrypt a ciphertext to plaintext.
 *
 * It is allowed to have cipher == msg
 *
 * @param[in]   cipher      resulting ciphertext, is CHACHA20POLY1305_TAG_BYTES
 *                          longer than the message length
 * @param[in]   cipherlen   length of the ciphertext
 * @param[out]  msg         message to encrypt
 * @param[in]   msglen      resulting length in bytes of the message
 * @param[in]   aad         additional authenticated data to verify
 * @param[in]   aadlen      length of the additional authenticated data
 * @param[in]   key         key to decrypt with, must be
 *                          CHACHA20POLY1305_KEY_BYTES long
 * @param[in]   nonce       Nonce to use. Must be CHACHA20POLY1305_NONCE_BYTES
 *                          long
 */
int chacha20poly1305_decrypt(const uint8_t *cipher, size_t cipherlen,
                             uint8_t *msg, size_t *msglen,
                             const uint8_t *aad, size_t aadlen,
                             const uint8_t *key, const uint8_t *nonce);

/**
 * @brief Encrypt a plaintext to ciphertext with the ChaCha20 algorithm.
 *
 * @param[in]   input       Input for the encryption/decryption.
 * @param[out]  output      The resulting encrypted cipher/decrypted message.
 * @param[in]   key         Key to encrypt/decrypt with, must be
 *                          @ref CHACHA20POLY1305_KEY_BYTES long.
 * @param[in]   nonce       Nonce to use. Must be CHACHA20POLY1305_NONCE_BYTES
 *                          long.
 * @param[in]   inputlen    Length of the input byte array.
*/
void chacha20_encrypt_decrypt(const uint8_t *input, uint8_t *output,
                             const uint8_t *key, const uint8_t *nonce,
                             size_t inputlen);

#ifdef __cplusplus
}
#endif
#endif /* CRYPTO_CHACHA20POLY1305_H */
/** @} */
