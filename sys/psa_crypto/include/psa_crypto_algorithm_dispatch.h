/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @defgroup    sys_psa_crypto_alg_disp PSA Crypto Algorithm Dispatcher
 * @{
 *
 * @file        psa_crypto_algorithm_dispatch.h
 * @brief       Function declarations for PSA Crypto algorithm dispatcher
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "kernel_defines.h"
#include "psa/crypto.h"

#if IS_USED(MODULE_PSA_KEY_MANAGEMENT)
#include "psa_crypto_slot_management.h"
#endif

#if IS_USED(MODULE_PSA_HASH)
/**
 * @brief   Dispatch a hash setup function to a specific backend.
 *          See @ref psa_hash_setup()
 */
psa_status_t psa_algorithm_dispatch_hash_setup(psa_hash_operation_t *operation,
                                               psa_algorithm_t alg);

/**
 * @brief   Dispatch a hash update function to a specific backend.
 *          See @ref psa_hash_update()
 */
psa_status_t psa_algorithm_dispatch_hash_update(psa_hash_operation_t *operation,
                                                const uint8_t *input,
                                                size_t input_length);

/**
 * @brief   Dispatch a hash finish function to a specific backend.
 *          See @ref psa_hash_finish()
 */
psa_status_t psa_algorithm_dispatch_hash_finish(psa_hash_operation_t *operation,
                                                uint8_t *hash,
                                                size_t hash_size,
                                                size_t *hash_length);
#endif /* MODULE_PSA_HASH */

#if IS_USED(MODULE_PSA_ASYMMETRIC)
/**
 * @brief   Dispatch a hash signature function to a specific backend.
 *          See @ref psa_sign_hash()
 */
psa_status_t psa_algorithm_dispatch_sign_hash(  const psa_key_attributes_t *attributes,
                                                psa_algorithm_t alg,
                                                const psa_key_slot_t *slot,
                                                const uint8_t *hash,
                                                size_t hash_length,
                                                uint8_t *signature,
                                                size_t signature_size,
                                                size_t *signature_length);

/**
 * @brief   Dispatch a message signature function to a specific backend.
 *          See @ref psa_sign_message()
 */
psa_status_t psa_algorithm_dispatch_sign_message(const psa_key_attributes_t *attributes,
                                                psa_algorithm_t alg,
                                                const psa_key_slot_t *slot,
                                                const uint8_t *input,
                                                size_t input_length,
                                                uint8_t *signature,
                                                size_t signature_size,
                                                size_t *signature_length);

/**
 * @brief   Dispatch a hash verification function to a specific backend.
 *          See @ref psa_verify_hash()
 */
psa_status_t psa_algorithm_dispatch_verify_hash(  const psa_key_attributes_t *attributes,
                                                  psa_algorithm_t alg,
                                                  const psa_key_slot_t *slot,
                                                  const uint8_t *hash,
                                                  size_t hash_length,
                                                  const uint8_t *signature,
                                                  size_t signature_length);

/**
 * @brief   Dispatch a message verification function to a specific backend.
 *          See @ref psa_verify_message()
 */
psa_status_t psa_algorithm_dispatch_verify_message( const psa_key_attributes_t *attributes,
                                                    psa_algorithm_t alg,
                                                    const psa_key_slot_t *slot,
                                                    const uint8_t *input,
                                                    size_t input_length,
                                                    const uint8_t *signature,
                                                    size_t signature_length);
#endif /* MODULE_PSA_ASYMMETRIC */

#if IS_USED(MODULE_PSA_KEY_MANAGEMENT)
/**
 * @brief   Dispatch the key generation function to a specific backend.
 *          See @ref psa_generate_key()
 */
psa_status_t psa_algorithm_dispatch_generate_key(   const psa_key_attributes_t *attributes,
                                                    psa_key_slot_t *slot);

/**
 * @brief   Dispatch the key import function to a specific backend.
 *          See psa_import_key()
 */
psa_status_t psa_algorithm_dispatch_import_key(const psa_key_attributes_t *attributes,
                                               const uint8_t *data, size_t data_length,
                                               psa_key_slot_t *slot, size_t *bits);
#endif

#if IS_USED(MODULE_PSA_CIPHER)
/**
 * @brief   Dispatch a cipher encrypt function to a specific backend.
 *          See @ref psa_cipher_encrypt()
 */
psa_status_t psa_algorithm_dispatch_cipher_encrypt( const psa_key_attributes_t *attributes,
                                                    psa_algorithm_t alg,
                                                    const psa_key_slot_t *slot,
                                                    const uint8_t *input,
                                                    size_t input_length,
                                                    uint8_t *output,
                                                    size_t output_size,
                                                    size_t *output_length);

/**
 * @brief   Dispatch a cipher decrypt function to a specific backend.
 *          See @ref psa_cipher_decrypt()
 */
psa_status_t psa_algorithm_dispatch_cipher_decrypt( const psa_key_attributes_t *attributes,
                                                    psa_algorithm_t alg,
                                                    const psa_key_slot_t *slot,
                                                    const uint8_t *input,
                                                    size_t input_length,
                                                    uint8_t *output,
                                                    size_t output_size,
                                                    size_t *output_length);
#endif /* MODULE_PSA_CIPHER */

#if IS_USED(MODULE_PSA_AEAD)
/**
 * @brief   Dispatch a aead encrypt function to a specific backend.
 *          See @ref psa_aead_encrypt()
 */
psa_status_t psa_algorithm_dispatch_aead_encrypt(   const psa_key_attributes_t *attributes,
                                                    psa_algorithm_t alg,
                                                    const psa_key_slot_t *slot,
                                                    const uint8_t *nonce,
                                                    size_t nonce_length,
                                                    const uint8_t *additional_data,
                                                    size_t additional_data_length,
                                                    const uint8_t *plaintext,
                                                    size_t plaintext_length,
                                                    uint8_t *ciphertext,
                                                    size_t ciphertext_size,
                                                    size_t *ciphertext_length);

/**
 * @brief   Dispatch a aead decrypt function to a specific backend.
 *          See @ref psa_aead_decrypt()
 */
psa_status_t psa_algorithm_dispatch_aead_decrypt(   const psa_key_attributes_t *attributes,
                                                    psa_algorithm_t alg,
                                                    const psa_key_slot_t *slot,
                                                    const uint8_t *nonce,
                                                    size_t nonce_length,
                                                    const uint8_t *additional_data,
                                                    size_t additional_data_length,
                                                    const uint8_t *ciphertext,
                                                    size_t ciphertext_length,
                                                    uint8_t *plaintext,
                                                    size_t plaintext_size,
                                                    size_t *plaintext_length);
#endif /* MODULE_PSA_AEAD */

#if IS_USED(MODULE_PSA_MAC)
/**
 * @brief   Dispatch a mac computation function to a specific backend.
 *          See @ref psa_mac_compute()
 */
psa_status_t psa_algorithm_dispatch_mac_compute(const psa_key_attributes_t *attributes,
                                                psa_algorithm_t alg,
                                                const psa_key_slot_t *slot,
                                                const uint8_t *input,
                                                size_t input_length,
                                                uint8_t *mac,
                                                size_t mac_size,
                                                size_t *mac_length);

/**
 * @brief   Dispatch a mac verification function to a specific backend.
 *          See @ref psa_mac_verify()
 */
psa_status_t psa_algorithm_dispatch_mac_verify(const psa_key_attributes_t *attributes,
                                               psa_algorithm_t alg,
                                               const psa_key_slot_t *slot,
                                               const uint8_t *input,
                                               size_t input_length,
                                               const uint8_t *mac,
                                               size_t mac_length);

/**
 * @brief   Dispatch call of a mac sign setup function to a specific backend.
 *          See @ref psa_mac_sign_setup()
 */
psa_status_t psa_algorithm_dispatch_mac_sign_setup(psa_mac_operation_t *operation,
                                                   const psa_key_attributes_t *attributes,
                                                   const psa_key_slot_t *slot,
                                                   psa_algorithm_t alg);

/**
 * @brief   Dispatch call of a mac verify setup function to a specific backend.
 *          See @ref psa_mac_verify_setup()
 */
psa_status_t psa_algorithm_dispatch_mac_verify_setup(psa_mac_operation_t *operation,
                                                     const psa_key_attributes_t *attributes,
                                                     const psa_key_slot_t *slot,
                                                     psa_algorithm_t alg);

/**
 * @brief   Dispatch call of a mac update function to a specific backend.
 *          See @ref psa_mac_update()
 */
psa_status_t psa_algorithm_dispatch_mac_update(psa_mac_operation_t *operation,
                                               const uint8_t *input,
                                               size_t input_length);

/**
 * @brief   Dispatch call of a mac sign finish function to a specific backend.
 *          See @ref psa_mac_sign_finish()
 */
psa_status_t psa_algorithm_dispatch_mac_sign_finish(psa_mac_operation_t *operation,
                                                    uint8_t *mac,
                                                    size_t mac_size,
                                                    size_t *mac_length);

/**
 * @brief   Dispatch call of a mac verify finish function to a specific backend.
 *          See @ref psa_mac_verify_finish()
 */
psa_status_t psa_algorithm_dispatch_mac_verify_finish(psa_mac_operation_t *operation,
                                                      const uint8_t *mac,
                                                      size_t mac_length);

/**
 * @brief   Dispatch call of a mac abort function to a specific backend.
 *          See @ref psa_mac_abort()
 */
psa_status_t psa_algorithm_dispatch_mac_abort(psa_mac_operation_t *operation);
#endif /* MODULE_PSA_MAC */

#ifdef __cplusplus
}
#endif

/** @} */
