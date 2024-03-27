/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @defgroup    sys_psa_crypto_alg_disp PSA Crypto Algorithm Dispatcher
 * @{
 *
 * @file        psa_crypto_algorithm_dispatch.h
 * @brief       Function declarations for PSA Crypto algorithm dispatcher
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_CRYPTO_ALGORITHM_DISPATCH_H
#define PSA_CRYPTO_ALGORITHM_DISPATCH_H

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
 *
 * @param operation
 * @param alg
 * @return @ref psa_status_t
 */
psa_status_t psa_algorithm_dispatch_hash_setup(psa_hash_operation_t *operation,
                                               psa_algorithm_t alg);

/**
 * @brief   Dispatch a hash update function to a specific backend.
 *          See @ref psa_hash_update()
 *
 * @param operation
 * @param input
 * @param input_length
 * @return @ref psa_status_t
 */
psa_status_t psa_algorithm_dispatch_hash_update(psa_hash_operation_t *operation,
                                                const uint8_t *input,
                                                size_t input_length);

/**
 * @brief   Dispatch a hash finish function to a specific backend.
 *          See @ref psa_hash_finish()
 *
 * @param operation
 * @param hash
 * @param hash_size
 * @param hash_length
 * @return @ref psa_status_t
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
 *
 * @param attributes
 * @param alg
 * @param slot
 * @param hash
 * @param hash_length
 * @param signature
 * @param signature_size
 * @param signature_length
 * @return @ref psa_status_t
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
 *
 * @param attributes
 * @param alg
 * @param slot
 * @param input
 * @param input_length
 * @param signature
 * @param signature_size
 * @param signature_length
 * @return @ref psa_status_t
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
 *
 * @param attributes
 * @param alg
 * @param slot
 * @param hash
 * @param hash_length
 * @param signature
 * @param signature_length
 * @return @ref psa_status_t
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
 *
 * @param attributes
 * @param alg
 * @param slot
 * @param input
 * @param input_length
 * @param signature
 * @param signature_length
 * @return @ref psa_status_t
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
 *
 * @param attributes
 * @param slot
 * @return @ref psa_status_t
 */
psa_status_t psa_algorithm_dispatch_generate_key(   const psa_key_attributes_t *attributes,
                                                    psa_key_slot_t *slot);

/**
 * @brief   Dispatch the key import function to a specific backend.
 *          See psa_import_key()
 *
 * @param attributes
 * @param data
 * @param data_length
 * @param slot
 * @param bits
 * @return psa_status_t
 */
psa_status_t psa_algorithm_dispatch_import_key(const psa_key_attributes_t *attributes,
                                               const uint8_t *data, size_t data_length,
                                               psa_key_slot_t *slot, size_t *bits);
#endif

#if IS_USED(MODULE_PSA_CIPHER)
/**
 * @brief   Dispatch a cipher encrypt function to a specific backend.
 *          See @ref psa_cipher_encrypt()
 *
 * @param attributes
 * @param alg
 * @param slot
 * @param input
 * @param input_length
 * @param output
 * @param output_size
 * @param output_length
 * @return @ref psa_status_t
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
 *
 * @param attributes
 * @param alg
 * @param slot
 * @param input
 * @param input_length
 * @param output
 * @param output_size
 * @param output_length
 * @return @ref psa_status_t
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

#if IS_USED(MODULE_PSA_MAC)
/**
 * @brief   Dispatch a mac computation function to a specific backend.
 *          See @ref psa_mac_compute()
 *
 * @param attributes
 * @param alg
 * @param slot
 * @param input
 * @param input_length
 * @param mac
 * @param mac_size
 * @param mac_length
 * @return @ref psa_status_t
 */
psa_status_t psa_algorithm_dispatch_mac_compute(const psa_key_attributes_t *attributes,
                                                psa_algorithm_t alg,
                                                const psa_key_slot_t *slot,
                                                const uint8_t *input,
                                                size_t input_length,
                                                uint8_t *mac,
                                                size_t mac_size,
                                                size_t *mac_length);
#endif

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTO_ALGORITHM_DISPATCH_H */
/** @} */
