/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @defgroup    sys_psa_crypto_cipher   PSA Wrapper Functions: Cipher
 * @{
 *
 * @file        psa_ciphers.h
 * @brief       Function declarations for low level wrapper functions for cipher operations.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_CIPHERS_H
#define PSA_CIPHERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"
#include "psa/crypto_contexts.h"

#if IS_USED(MODULE_PSA_CIPHER_AES_128_CBC) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for an AES 128 CBC encryption.
 *          See @ref psa_cipher_encrypt()
 *
 * @param attributes
 * @param key_buffer
 * @param key_buffer_size
 * @param alg
 * @param input
 * @param input_length
 * @param output
 * @param output_size
 * @param output_length
 * @return @ref psa_status_t
 */
psa_status_t psa_cipher_cbc_aes_128_encrypt(const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            psa_algorithm_t alg,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *output,
                                            size_t output_size,
                                            size_t *output_length);

/**
 * @brief   Low level wrapper function to call a driver for an AES 128 CBC decryption.
 *          See @ref psa_cipher_decrypt()
 *
 * @param attributes
 * @param key_buffer
 * @param key_buffer_size
 * @param alg
 * @param input
 * @param input_length
 * @param output
 * @param output_size
 * @param output_length
 * @return @ref psa_status_t
 */
psa_status_t psa_cipher_cbc_aes_128_decrypt(const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            psa_algorithm_t alg,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *output,
                                            size_t output_size,
                                            size_t *output_length);
#endif /* MODULE_PSA_CIPHER_AES_128_CBC */

#if IS_USED(MODULE_PSA_CIPHER_AES_192_CBC) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for an AES 192 CBC encryption.
 *          See @ref psa_cipher_encrypt()
 *
 * @param attributes
 * @param key_buffer
 * @param key_buffer_size
 * @param alg
 * @param input
 * @param input_length
 * @param output
 * @param output_size
 * @param output_length
 * @return @ref psa_status_t
 */
psa_status_t psa_cipher_cbc_aes_192_encrypt(const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            psa_algorithm_t alg,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *output,
                                            size_t output_size,
                                            size_t *output_length);
#endif /* MODULE_PSA_CIPHER_AES_192_CBC */

#if IS_USED(MODULE_PSA_CIPHER_AES_256_CBC) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for an AES 256 CBC encryption.
 *          See @ref psa_cipher_encrypt()
 *
 * @param attributes
 * @param key_buffer
 * @param key_buffer_size
 * @param alg
 * @param input
 * @param input_length
 * @param output
 * @param output_size
 * @param output_length
 * @return psa_status_t
 */
psa_status_t psa_cipher_cbc_aes_256_encrypt(const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            psa_algorithm_t alg,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *output,
                                            size_t output_size,
                                            size_t *output_length);
#endif /* MODULE_PSA_CIPHER_AES_256_CBC */

#if IS_USED(MODULE_PSA_CIPHER_CHACHA20) || defined(DOXYGEN)

/**
 * @brief   ChaCha20 setup function
 *
 * @param   ctx         Driver specific ChaCha20 context of type @c psa_cipher_chacha20_ctx_t
 * @param   key_data    Key Buffer.
 * @param   key_length  Length of the key buffer. Must be @c 32 bytes.
 * @return  psa_status_t
 */
psa_status_t psa_cipher_chacha20_encrypt_setup( psa_cipher_chacha20_ctx_t *ctx,
                                                uint8_t *key_data,
                                                size_t key_length);

/**
 * @brief   ChaCha20 setup function
 *
 * @param   ctx         Driver specific ChaCha20 context of type @c psa_cipher_chacha20_ctx_t
 * @param   key_data    Key Buffer.
 * @param   key_length  Length of the key buffer. Must be @c 32 bytes.
 * @return  psa_status_t
 */
psa_status_t psa_cipher_chacha20_decrypt_setup( psa_cipher_chacha20_ctx_t *ctx,
                                                uint8_t *key_data,
                                                size_t key_length);

/**
 * @brief ChaCha20 set nonce and initial counter function
 *
 * @param ctx       Driver specific ChaCha20 context of type @c psa_cipher_chacha20_ctx_t
 * @param iv        Buffer of IV to be set
 * @param iv_length Length of the IV Buffer.
 *                  - 12 bytes: the provided IV is used as the nonce, and the
 *                  counter value is set to zero.
 *                  - 16 bytes: the first four bytes of the IV are used as the
 *                  counter value (encoded as little-endian), and the remaining
 *                  12 bytes are used as the nonce.
 *                  - 8 bytes: the cipher operation uses the original [CHACHA20]
 *                  definition of ChaCha20: the provided IV is used as the
 *                  64-bit nonce, and the 64-bit counter value is set to zero.
 *                  - It is recommended that implementations do not support
 *                  other sizes of IV.
 */
psa_status_t psa_cipher_chacha20_set_iv(psa_cipher_chacha20_ctx_t *ctx,
                                        const uint8_t *iv,
                                        size_t iv_length);

/**
 * @brief   ChaCha20 update function, encrypting/decrypting aligned blocks.
 *
 * @param   ctx             Driver specific ChaCha20 context of type @c psa_cipher_chacha20_ctx_t
 * @param   input           Input that is going to be encrypted/decrypted
 * @param   input_length    Length of @c input buffer
 * @param   output          Output Buffer for the encrypted/decrypted input
 * @param   output_size     Length of @c output buffer. Must be large enough to
 *                          fit the output blocks from buffer and input
 * @param   output_length   Actual length of the output data. Will be multiple
 *                          of Block size (64B).
 * @return  psa_status_t
 */
psa_status_t psa_cipher_chacha20_update(psa_cipher_chacha20_ctx_t *ctx,
                                        const uint8_t *input,
                                        size_t input_length,
                                        uint8_t *output,
                                        size_t output_size,
                                        size_t *output_length);

/**
 * @brief   ChaCha20 finish function. Outputs the xcrytion of the remaining buffer.
 *
 * @param   ctx             Driver specific ChaCha context of type @c psa_cipher_chacha20_ctx_t
 * @param   output          Output Buffer for the encrypted/decrypted data
 * @param   output_size     Length of @c output buffer. Must be large enough to
 *                          fit the buffer (64B).
 * @param   output_length   Actual length of the output data. Will be less than
 *                          Block size (64B).
 * @return  psa_status_t
 */
psa_status_t psa_cipher_chacha20_finish(psa_cipher_chacha20_ctx_t *ctx,
                                        uint8_t *output,
                                        size_t output_size,
                                        size_t *output_length);

/**
 * @brief   Low level wrapper function to call a driver for ChaCha20 encryption/decryption.
 *          See @ref psa_cipher_encrypt().
 * @param key_buffer        Buffer containing the Key.
 * @param key_buffer_size   Size of the key in bytes. Must be 32 bytes.
 * @param input             Input Buffer containing the message to be encrypted.
 * @param input_length      Size of input buffer.
 * @param output            Output buffer containing the generated 12 byte nonce
 *                          followed by the cipher of the input.
 * @param output_size       Size of output buffer. Must be at least
 *                          (input_length + 12) bytes long.
 * @param output_length     Actual size of the output (including nonce).
*/
psa_status_t psa_cipher_chacha20_encrypt(   uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *output,
                                            size_t output_size,
                                            size_t *output_length);

/**
 * @brief   Low level wrapper function to call a driver for ChaCha20 decryption.
 *          See @ref  psa_cipher_decrypt().
 * @param key_buffer        Buffer containing the Key.
 * @param key_buffer_size   Size of the key in bytes. Must be 32 bytes long.
 * @param input             Input buffer containing the 12 byte nonce used in the
 *                          encryption followed by the cipher to be decrypted.
 * @param input_length      Size of input buffer (including 12 byte nonce).
 * @param output            Output buffer.
 * @param output_size       Size of output buffer. Must be at least
 *                          (input_length - 12) bytes long.
 * @param output_length     Actual size of the output.
*/
psa_status_t psa_cipher_chacha20_decrypt(   uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *output,
                                            size_t output_size,
                                            size_t *output_length);

#endif /* MODULE_PSA_CIPHER_CHACHA20 */
#ifdef __cplusplus
}
#endif

#endif /* PSA_CIPHERS_H */
/** @} */
