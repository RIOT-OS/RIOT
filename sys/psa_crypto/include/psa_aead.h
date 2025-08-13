/*
 * Copyright (C) 2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @defgroup    sys_psa_crypto_aead   PSA Wrapper Functions: AEAD
 * @{
 *
 * @file        psa_aead.h
 * @brief       Function declarations for low level wrapper functions for aead operations.
 *
 * @author      Lukas Luger <lukas.luger@mailbox.tu-dresden.de>
 *
 */
#ifndef PSA_AEAD_H
#define PSA_AEAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"
#include "psa/crypto_contexts.h"

#if IS_USED(MODULE_PSA_AEAD_AES_128_CCM) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for an AES 128 CCM encryption.
 *          See @ref psa_aead_encrypt()
 */
psa_status_t psa_aead_aes_128_ccm_encrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *plaintext,
                                          size_t plaintext_length, uint8_t *ciphertext,
                                          size_t ciphertext_size, size_t *ciphertext_length);

/**
 * @brief   Low level wrapper function to call a driver for an AES 128 CCM decryption.
 *          See @ref psa_aead_decrypt()
 */
psa_status_t psa_aead_aes_128_ccm_decrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *ciphertext,
                                          size_t ciphertext_length, uint8_t *plaintext,
                                          size_t plaintext_size, size_t *plaintext_length);
#endif /* MODULE_PSA_AEAD_AES_128_CCM */

#if IS_USED(MODULE_PSA_AEAD_AES_192_CCM) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for an AES 192 CCM encryption.
 *          See @ref psa_aead_encrypt()
 */
psa_status_t psa_aead_aes_192_ccm_encrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *plaintext,
                                          size_t plaintext_length, uint8_t *ciphertext,
                                          size_t ciphertext_size, size_t *ciphertext_length);

/**
* @brief   Low level wrapper function to call a driver for an AES 192 CCM decryption.
*          See @ref psa_aead_decrypt()
*/
psa_status_t psa_aead_aes_192_ccm_decrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *ciphertext,
                                          size_t ciphertext_length, uint8_t *plaintext,
                                          size_t plaintext_size, size_t *plaintext_length);
#endif /* MODULE_PSA_AEAD_AES_192_CCM */

#if IS_USED(MODULE_PSA_AEAD_AES_256_CCM) || defined(DOXYGEN)
/**
 * @brief   Low level wrapper function to call a driver for an AES 256 CCM encryption.
 *          See @ref psa_aead_encrypt()
 */
psa_status_t psa_aead_aes_256_ccm_encrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *plaintext,
                                          size_t plaintext_length, uint8_t *ciphertext,
                                          size_t ciphertext_size, size_t *ciphertext_length);

/**
* @brief   Low level wrapper function to call a driver for an AES 256 CCM decryption.
*          See @ref psa_aead_decrypt()
*/
psa_status_t psa_aead_aes_256_ccm_decrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *ciphertext,
                                          size_t ciphertext_length, uint8_t *plaintext,
                                          size_t plaintext_size, size_t *plaintext_length);
#endif /* MODULE_PSA_AEAD_AES_256_CCM */
#ifdef __cplusplus
}
#endif

#endif /* PSA_AEAD_H */
/** @} */
