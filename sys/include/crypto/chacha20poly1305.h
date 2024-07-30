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

#define CHACHA20POLY1305_CONSTANT_BYTES (16U)   /**< Constants length in bytes */
#define CHACHA20POLY1305_KEY_BYTES      (32U)   /**< Key length in bytes */
#define CHACHA20POLY1305_COUNTER_BYTES  (4U)    /**< Counter length in bytes */
#define CHACHA20POLY1305_NONCE_BYTES    (12U)   /**< Nonce length in bytes */
#define CHACHA20POLY1305_TAG_BYTES      (16U)   /**< Tag length in bytes */
#define CHACHA20POLY1305_BLOCK_BYTES    (64U)   /**< Block length in bytes */

/**
 * @brief Context of a ChaCha20 multipart operation
 */
typedef union {
    /**< Array Access to ChaCha20 state */
    uint32_t words[CHACHA20POLY1305_BLOCK_BYTES / sizeof(uint32_t)];
    struct {
        /**< Constant Value */
        uint32_t constant[CHACHA20POLY1305_CONSTANT_BYTES / sizeof(uint32_t)];
        /**< Key of the ChaCha20 operation */
        uint32_t key[CHACHA20POLY1305_KEY_BYTES / sizeof(uint32_t)];
        /**< Counter of the ChaCha20 operation */
        uint32_t counter[CHACHA20POLY1305_COUNTER_BYTES / sizeof(uint32_t)];
        /**< Nonce of the ChaCha20 operation */
        uint32_t nonce[CHACHA20POLY1305_NONCE_BYTES / sizeof(uint32_t)];
    }__attribute__ ((__packed__)) split;    /**< Individual access to ChaCha20 state */
} chacha20_ctx_t;

/**
 * @brief Chacha20poly1305 state struct
 */
typedef union {
    /* We need both the state matrix and the poly1305 state, but nearly not at
     * the same time. This works as long as the first 8 members of state
     * overlap fully or completely not with the first and second key parts
     * from the @ref poly1305_ctx_t struct */
    chacha20_ctx_t chacha20;   /**< The context of the ChaCha20 operation. */
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
 * @param[in]   key             Key to encrypt/decrypt with, must be
 *                              @ref CHACHA20POLY1305_KEY_BYTES long.
 * @param[in]   nonce           Nonce to use. Must be CHACHA20POLY1305_NONCE_BYTES
 *                              long.
 * @param[in]   counter         Initial counter for the ChaCha20 operation.
 * @param[in]   input           Input for the encryption/decryption.
 * @param[in]   input_length    Length of the input byte array.
 * @param[out]  output          The resulting encrypted cipher/decrypted message.
*/
void chacha20_encrypt_decrypt(  const uint8_t *key,
                                const uint8_t *nonce,
                                uint32_t counter,
                                const uint8_t *input,
                                size_t input_length,
                                uint8_t *output);
/**
 * @brief Setup a ChaCha20 encrypt or decrypt multipart operation.
 *
 * @param[out]  ctx     Context of the multipart ChaCha20 operation.
 * @param[in]   key     Key to encrypt/decrypt with, must be
 *                      @ref CHACHA20POLY1305_KEY_BYTES long.
 * @param[in]   nonce   Nonce to use. Must be CHACHA20POLY1305_NONCE_BYTES long.
 * @param[in]   counter Initial counter for the multipart ChaCha20 operation.
 */
void chacha20_setup(chacha20_ctx_t *ctx,
                    const uint8_t *key,
                    const uint8_t *nonce,
                    const uint32_t counter);

/**
 * @brief Update a ChaCha20 encrypt or decrypt multipart operation.
 *
 * @param[in]   ctx             Context of the multipart ChaCha20 operation.
 * @param[in]   input           Input buffer containing one block of input data (64B).
 * @param[out]  output          Output buffer. Must be at least length of input buffer.
 */
void chacha20_update(   chacha20_ctx_t *ctx,
                        const uint8_t *input,
                        uint8_t *output);

/**
 * @brief Finish a ChaCha20 encrypt or decrypt multipart operation.
 *
 * @param[in]   ctx             Context of the multipart ChaCha20 operation.
 * @param[in]   input           Input buffer.
 * @param[in]   input_length    Length of input buffer. Must be a less than 64B.
 * @param[out]  output          Output buffer. Must be at least length of input buffer.
 */
void chacha20_finish(   chacha20_ctx_t *ctx,
                        const uint8_t *input,
                        size_t input_length,
                        uint8_t *output);

#ifdef __cplusplus
}
#endif
#endif /* CRYPTO_CHACHA20POLY1305_H */
/** @} */
