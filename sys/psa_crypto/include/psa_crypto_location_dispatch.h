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
 * @defgroup    sys_psa_crypto_loc_disp PSA Crypto Location Dispatcher
 * @{
 *
 * @file        psa_crypto_location_dispatch.h
 * @brief       Function declarations for the PSA Crypto location dispatcher.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 */


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
 * @brief   Dispatch call of a mac computation function to a location specific backend.
 *          See psa_mac_compute()
 */
psa_status_t psa_location_dispatch_mac_compute(const psa_key_attributes_t *attributes,
                                               psa_algorithm_t alg,
                                               const psa_key_slot_t *slot,
                                               const uint8_t *input,
                                               size_t input_length,
                                               uint8_t *mac,
                                               size_t mac_size,
                                               size_t *mac_length);
#endif

#if IS_USED(MODULE_PSA_KEY_MANAGEMENT)
/**
 * @brief   Dispatch call of the key generation function to a location specific backend.
 *          See psa_generate_key()
 */
psa_status_t psa_location_dispatch_generate_key(const psa_key_attributes_t *attributes,
                                                psa_key_slot_t *slot);

/**
 * @brief   Dispatch call of the key import function to a location specific backend.
 *          See psa_import_key()
 */
psa_status_t psa_location_dispatch_import_key( const psa_key_attributes_t *attributes,
                                               const uint8_t *data, size_t data_length,
                                               psa_key_slot_t *slot, size_t *bits);
#endif /* MODULE_PSA_KEY_MANAGEMENT */

#if IS_USED(MODULE_PSA_CIPHER)
/**
 * @brief   Dispatch call of a cipher encrypt setup function to a location specific backend.
 *          See psa_cipher_setup()
 */
psa_status_t psa_location_dispatch_cipher_encrypt_setup(   psa_cipher_operation_t *operation,
                                                           const psa_key_attributes_t *attributes,
                                                           const psa_key_slot_t *slot,
                                                           psa_algorithm_t alg);

/**
 * @brief   Dispatch call of a cipher decrypt setup function to a location specific backend.
 *          See psa_cipher_setup()
 */
psa_status_t psa_location_dispatch_cipher_decrypt_setup(psa_cipher_operation_t *operation,
                                                        const psa_key_attributes_t *attributes,
                                                        const psa_key_slot_t *slot,
                                                        psa_algorithm_t alg);

/**
 * @brief   Dispatch call of a function to set a cipher IV to a location specific backend.
 *          See psa_cipher_set_iv()
 */
psa_status_t psa_location_dispatch_cipher_set_iv(  psa_cipher_operation_t *operation,
                                                   const uint8_t *iv,
                                                   size_t iv_length);

/**
 * @brief   Dispatch call of a cipher encrypt function to a location specific backend.
 *          See psa_cipher_encrypt()
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
 */
psa_status_t psa_location_dispatch_generate_random(uint8_t *output,
                                                   size_t output_size);

#ifdef __cplusplus
}
#endif

/** @} */
