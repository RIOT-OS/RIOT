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
psa_status_t psa_cipher_chacha20_encrypt(uint8_t *key_buffer,
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
psa_status_t psa_cipher_chacha20_decrypt(uint8_t *key_buffer,
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
