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
#endif /* CONFIG_CIPHER_CBC_AES_128 */

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
#endif /* CONFIG_CIPHER_CBC_AES_192 */

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
#endif /* CONFIG_CIPHER_CBC_AES_256 */

#if IS_USED(MODULE_PSA_CIPHER_CHACHA20_POLY1305) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for a CHACHA20-Poly1305 encryption.
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
psa_status_t psa_cipher_chacha20_poly1305_encrypt(const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *output,
                                            size_t output_size,
                                            size_t *output_length);

/**
 * @brief   Low level wrapper function to call a driver for a CHACHA20-Poly1305 decryption.
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
 * @return psa_status_t
 */
psa_status_t psa_cipher_chacha20_poly1305_decrypt(const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            psa_algorithm_t alg,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *output,
                                            size_t output_size,
                                            size_t *output_length);
#endif /* CONFIG_CHACHA20_POLY1305 */

#if IS_USED(MODULE_PSA_CIPHER_CHACHA_20) || defined(DOXYGEN)

/**
 * @brief   ChaCha setup function
 *
 * @param   ctx             Driver specific ChaCha context of type @c psa_cipher_chacha20_ctx_t
 * @param   nonce           Nonce used in the ChaCha operation of type @c psa_cipher_chacha20_nonce_t .
 *                          Must be of size @c CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES .
 * @param   key             Key used in ChaCha operation. Must be of size @c CRYS_CHACHA_KEY_MAX_SIZE_IN_BYTES .
 * @param   initialCounter  Initial counter of the ChaCha operation 
 * @param   mode            ChaCha mode of type @c CRYS_CHACHA_EncryptMode_t
 * @return  psa_status_t
 */
psa_status_t psa_cipher_chacha20_setup(psa_cipher_chacha_20_ctx_t *ctx,
                                       psa_cipher_chacha_20_nonce_t nonce,
                                       uint8_t *key,
                                       uint32_t initial_counter,
                                       psa_encrypt_or_decrypt_t mode);

/**
 * @brief   ChaCha update function, encrypting/decrypting aligned blocks.
 *
 * @param   ctx         Driver specific ChaCha context of type @c psa_cipher_chacha20_ctx_t
 * @param   input       Input that is going to be encrypted/decrypted
 * @param   inputSize   Length of @p input . Must be multiple of @c CRYS_CHACHA_BLOCK_SIZE_IN_BYTES
 * @param   output      Output Buffer for the encrypted/decrypted input
 * @return  psa_status_t
 */
psa_status_t psa_cipher_chacha20_update(psa_cipher_chacha_20_ctx_t *ctx,
                                        uint8_t *input,
                                        uint32_t inputSize,
                                        uint8_t *output);

/**
 * @brief   ChaCha finish function, encrypting/decrypting remaining data.
 *
 * @param   ctx         Driver specific ChaCha context of type @c psa_cipher_chacha20_ctx_t
 * @param   input       Input that is going to be encrypted/decrypted
 * @param   inputSize   Length of @p input . Must be less of @c CRYS_CHACHA_BLOCK_SIZE_IN_BYTES
 * @param   output      Output Buffer for the encrypted/decrypted input
 * @return  psa_status_t
 */
psa_status_t psa_cipher_chacha20_finish(psa_cipher_chacha_20_ctx_t *ctx,
                                        uint8_t *input,
                                        uint32_t inputSize,
                                        uint8_t *output);

/**
 * @brief   Low level wrapper function to call a driver for ChaCha20 encryption/decryption.
 *          See @ref psa_cipher_encrypt().
 * @param key_buffer        Buffer containing the Key.
 * @param key_buffer_size   Size of the key in bytes.
 *                          Must be of size CHACHA20POLY1305_KEY_BYTES.
 * @param input             Input Buffer containing the message to be encrypted.
 * @param input_length      Size of input buffer.
 * @param output            Output buffer containing the generated nonce of size
 *                          CHACHA20POLY1305_NONCE_BYTES followed by the cipher of
 *                          the input.
 * @param output_size       Size of ouput buffer. Must be at least 
 *                          input_length + CHACHA20POLY1305_NONCE_BYTES bytes long.
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
 * @param key_buffer_size   Size of the key in bytes.
 *                          Must be of size CHACHA20POLY1305_KEY_BYTES.
 * @param input             Input buffer containing the nonce used in the 
 *                          encryption of size CHACHA20POLY1305_NONCE_BYTES followed by 
 *                          the cipher to be decrypted.
 * @param input_length      Size of input buffer (including nonce).
 * @param output            Output buffer.
 * @param output_size       Size of ouput buffer. Must be at least 
 *                          input_length + CHACHA20POLY1305_NONCE_BYTES bytes long.
 * @param output_length     Actual size of the output.
*/
psa_status_t psa_cipher_chacha20_decrypt(uint8_t *key_buffer,
                                         size_t key_buffer_size,
                                         const uint8_t *input,
                                         size_t input_length,
                                         uint8_t *output,
                                         size_t output_size,
                                         size_t *output_length);

#endif
#ifdef __cplusplus
}
#endif

#endif /* PSA_CIPHERS_H */
/** @} */
