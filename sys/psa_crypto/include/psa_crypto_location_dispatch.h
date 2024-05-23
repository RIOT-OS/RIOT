/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @defgroup    sys_psa_crypto_loc_disp PSA Crypto Location Dispatcher
 * @{
 *
 * @file        psa_crypto_location_dispatch.h
 * @brief       Function declarations for the PSA Crypto location dispatcher.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_CRYPTO_LOCATION_DISPATCH_H
#define PSA_CRYPTO_LOCATION_DISPATCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "kernel_defines.h"
#include "psa/crypto.h"

#if IS_USED(MODULE_PSA_ASYMMETRIC)
/**
 * @brief   Dispatch call of a hash signature function to a location specific backend.
 *          See psa_sign_hash()
 *
 * @param attributes
 * @param alg
 * @param slot
 * @param hash
 * @param hash_length
 * @param signature
 * @param signature_size
 * @param signature_length
 * @return psa_status_t
 */
psa_status_t psa_location_dispatch_sign_hash(  const psa_key_attributes_t *attributes,
                                               psa_algorithm_t alg,
                                               const psa_key_slot_t *slot,
                                               const uint8_t *hash,
                                               size_t hash_length,
                                               uint8_t *signature,
                                               size_t signature_size,
                                               size_t *signature_length);

/**
 * @brief   Dispatch call of a message signature function to a location specific backend.
 *          See psa_sign_message()
 *
 * @param attributes
 * @param alg
 * @param slot
 * @param input
 * @param input_length
 * @param signature
 * @param signature_size
 * @param signature_length
 * @return psa_status_t
 */
psa_status_t psa_location_dispatch_sign_message(const psa_key_attributes_t *attributes,
                                               psa_algorithm_t alg,
                                               const psa_key_slot_t *slot,
                                               const uint8_t *input,
                                               size_t input_length,
                                               uint8_t *signature,
                                               size_t signature_size,
                                               size_t *signature_length);

/**
 * @brief   Dispatch call of a hash verification function to a location specific backend.
 *          See psa_verify_hash()
 *
 * @param attributes
 * @param alg
 * @param slot
 * @param hash
 * @param hash_length
 * @param signature
 * @param signature_length
 * @return psa_status_t
 */
psa_status_t psa_location_dispatch_verify_hash(  const psa_key_attributes_t *attributes,
                                                 psa_algorithm_t alg,
                                                 const psa_key_slot_t *slot,
                                                 const uint8_t *hash,
                                                 size_t hash_length,
                                                 const uint8_t *signature,
                                                 size_t signature_length);

/**
 * @brief   Dispatch call of a message verification function to a location specific backend.
 *          See psa_verify_message()
 *
 * @param attributes
 * @param alg
 * @param slot
 * @param input
 * @param input_length
 * @param signature
 * @param signature_length
 * @return psa_status_t
 */
psa_status_t psa_location_dispatch_verify_message(const psa_key_attributes_t *attributes,
                                                 psa_algorithm_t alg,
                                                 const psa_key_slot_t *slot,
                                                 const uint8_t *input,
                                                 size_t input_length,
                                                 const uint8_t *signature,
                                                 size_t signature_length);
#endif /* MODULE_PSA_ASYMMETRIC */

#if IS_USED(MODULE_PSA_MAC)
/**
 * @brief   Dispatch call of a mac computation function to a location-specific backend.
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
 * @return psa_status_t
 */
psa_status_t psa_location_dispatch_mac_compute(const psa_key_attributes_t *attributes,
                                               psa_algorithm_t alg,
                                               const psa_key_slot_t *slot,
                                               const uint8_t *input,
                                               size_t input_length,
                                               uint8_t *mac,
                                               size_t mac_size,
                                               size_t *mac_length);

/**
 * @brief   Dispatch call of a mac sign setup function to a location-specific backend.
 *          See @ref psa_mac_sign_setup()
 *
 * @param operation
 * @param attributes
 * @param slot
 * @param alg
 * @return psa_status_t
 */
psa_status_t psa_location_dispatch_mac_sign_setup(psa_mac_operation_t *operation,
                                                  const psa_key_attributes_t *attributes,
                                                  const psa_key_slot_t *slot,
                                                  psa_algorithm_t alg);

/**
 * @brief   Dispatch call of a mac verify setup function to a location-specific backend.
 *          See @ref psa_mac_verify_setup()
 *
 * @param operation
 * @param attributes
 * @param slot
 * @param alg
 * @return psa_status_t
 */
psa_status_t psa_location_dispatch_mac_verify_setup(psa_mac_operation_t *operation,
                                                    const psa_key_attributes_t *attributes,
                                                    const psa_key_slot_t *slot,
                                                    psa_algorithm_t alg);

/**
 * @brief   Dispatch call of a mac update function to a location-specific backend.
 *          See @ref psa_mac_update()
 *
 * @param operation
 * @param attributes
 * @param slot
 * @param alg
 * @return psa_status_t
 */
psa_status_t psa_location_dispatch_mac_update(psa_mac_operation_t *operation,
                                              const uint8_t *input,
                                              size_t input_length);

/**
 * @brief   Dispatch call of a mac sign finish function to a location-specific backend.
 *          See @ref psa_mac_sign_finish()
 *
 * @param operation
 * @param attributes
 * @param slot
 * @param alg
 * @return psa_status_t
 */
psa_status_t psa_location_dispatch_mac_sign_finish(psa_mac_operation_t *operation,
                                                   uint8_t *mac,
                                                   size_t mac_size,
                                                   size_t *mac_length);

/**
 * @brief   Dispatch call of a mac verify finish function to a location-specific backend.
 *          See @ref psa_mac_verify_finish()
 *
 * @param operation
 * @param attributes
 * @param slot
 * @param alg
 * @return psa_status_t
 */
psa_status_t psa_location_dispatch_mac_verify_finish(psa_mac_operation_t *operation,
                                                     const uint8_t *mac,
                                                     size_t mac_length);

/**
 * @brief   Dispatch call of a mac abort function to a location-specific backend.
 *          See @ref psa_mac_abort()
 *
 * @param operation
 * @param attributes
 * @param slot
 * @param alg
 * @return psa_status_t
 */
psa_status_t psa_location_dispatch_mac_abort(psa_mac_operation_t *operation);
#endif

#if IS_USED(MODULE_PSA_KEY_MANAGEMENT)
/**
 * @brief   Dispatch call of the key generation function to a location specific backend.
 *          See psa_generate_key()
 *
 * @param attributes
 * @param slot
 * @return psa_status_t
 */
psa_status_t psa_location_dispatch_generate_key(const psa_key_attributes_t *attributes,
                                                psa_key_slot_t *slot);

/**
 * @brief   Dispatch call of the key import function to a location specific backend.
 *          See psa_import_key()
 *
 * @param attributes
 * @param data
 * @param data_length
 * @param slot
 * @param bits
 * @return psa_status_t
 */
psa_status_t psa_location_dispatch_import_key( const psa_key_attributes_t *attributes,
                                               const uint8_t *data, size_t data_length,
                                               psa_key_slot_t *slot, size_t *bits);
#endif /* MODULE_PSA_KEY_MANAGEMENT */

#if IS_USED(MODULE_PSA_CIPHER)
/**
 * @brief   Dispatch call of a cipher encrypt setup function to a location specific backend.
 *          See psa_cipher_setup()
 *
 * @param operation
 * @param attributes
 * @param slot
 * @param alg
 * @return psa_status_t
 */
psa_status_t psa_location_dispatch_cipher_encrypt_setup(   psa_cipher_operation_t *operation,
                                                           const psa_key_attributes_t *attributes,
                                                           const psa_key_slot_t *slot,
                                                           psa_algorithm_t alg);

/**
 * @brief   Dispatch call of a cipher decrypt setup function to a location specific backend.
 *          See psa_cipher_setup()
 *
 * @param operation
 * @param attributes
 * @param slot
 * @param alg
 * @return psa_status_t
 */
psa_status_t psa_location_dispatch_cipher_decrypt_setup(psa_cipher_operation_t *operation,
                                                        const psa_key_attributes_t *attributes,
                                                        const psa_key_slot_t *slot,
                                                        psa_algorithm_t alg);

/**
 * @brief   Dispatch call of a function to set a cipher IV to a location specific backend.
 *          See psa_cipher_set_iv()
 *
 * @param operation
 * @param iv
 * @param iv_length
 * @return psa_status_t
 */
psa_status_t psa_location_dispatch_cipher_set_iv(  psa_cipher_operation_t *operation,
                                                   const uint8_t *iv,
                                                   size_t iv_length);

/**
 * @brief   Dispatch call of a cipher encrypt function to a location specific backend.
 *          See psa_cipher_encrypt()
 *
 * @param attributes
 * @param alg
 * @param slot
 * @param input
 * @param input_length
 * @param output
 * @param output_size
 * @param output_length
 * @return psa_status_t
 */
psa_status_t psa_location_dispatch_cipher_encrypt(  const psa_key_attributes_t *attributes,
                                                    psa_algorithm_t alg,
                                                    const psa_key_slot_t *slot,
                                                    const uint8_t *input,
                                                    size_t input_length,
                                                    uint8_t *output,
                                                    size_t output_size,
                                                    size_t *output_length);

/**
 * @brief   Dispatch call of a cipher decrypt function to a location specific backend.
 *          See psa_cipher_decrypt()
 *
 * @param attributes
 * @param alg
 * @param slot
 * @param input
 * @param input_length
 * @param output
 * @param output_size
 * @param output_length
 * @return psa_status_t
 */
psa_status_t psa_location_dispatch_cipher_decrypt(  const psa_key_attributes_t *attributes,
                                                    psa_algorithm_t alg,
                                                    const psa_key_slot_t *slot,
                                                    const uint8_t *input,
                                                    size_t input_length,
                                                    uint8_t *output,
                                                    size_t output_size,
                                                    size_t *output_length);
#endif /* MODULE_PSA_CIPHER */

/**
 * @brief   Dispatch call of a random number generator to a specific backend.
 *          See psa_generate_random()
 *
 * @param output
 * @param output_size
 * @return psa_status_t
 */
psa_status_t psa_location_dispatch_generate_random(uint8_t *output,
                                                   size_t output_size);

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTO_LOCATION_DISPATCH_H */
/** @} */
