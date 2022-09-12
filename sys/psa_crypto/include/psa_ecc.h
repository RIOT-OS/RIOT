/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @defgroup    sys_psa_crypto_ecc  PSA Wrapper Functions: ECC
 * @{
 *
 * @file        psa_ecc.h
 * @brief       Function declarations for low level wrapper functions for ECC operations.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_ECC_H
#define PSA_ECC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"
#include "kernel_defines.h"

/**
 * @brief   Low level wrapper function to call a driver for an ECC key generation
 *          with a SECP 192 R1 key.
 *
 * @details See @ref psa_generate_key()
 *
 * @param   attributes
 * @param   priv_key_buffer
 * @param   pub_key_buffer
 * @param   priv_key_buffer_length
 * @param   pub_key_buffer_length
 * @return  @ref psa_status_t
 */
psa_status_t psa_generate_ecc_p192r1_key_pair(  const psa_key_attributes_t *attributes,
                                                uint8_t *priv_key_buffer, uint8_t *pub_key_buffer,
                                                size_t *priv_key_buffer_length,
                                                size_t *pub_key_buffer_length);

/**
 * @brief   Low level wrapper function to call a driver for an ECC public key export
 *          of a SECP 192 R1 key.
 *
 * @details See @ref psa_export_public_key()
 *
 * @param attributes
 * @param key_buffer
 * @param key_buffer_size
 * @param data
 * @param data_size
 * @param data_length
 * @return psa_status_t
 */
psa_status_t psa_ecc_p192r1_export_public_key(  const psa_key_attributes_t *attributes,
                                                uint8_t *key_buffer,
                                                size_t key_buffer_size,
                                                uint8_t *data,
                                                size_t data_size,
                                                size_t *data_length);

/**
 * @brief   Low level wrapper function to call a driver for an ECC hash signature
 *          with a SECP 192 R1 key.
 *
 * @details See @ref psa_sign_hash()
 *
 * @param attributes
 * @param alg
 * @param key_buffer
 * @param key_buffer_size
 * @param hash
 * @param hash_length
 * @param signature
 * @param signature_size
 * @param signature_length
 * @return psa_status_t
 */
psa_status_t psa_ecc_p192r1_sign_hash(  const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg,
                                        const uint8_t *key_buffer, size_t key_buffer_size,
                                        const uint8_t *hash, size_t hash_length,
                                        uint8_t *signature, size_t signature_size,
                                        size_t *signature_length);

/**
 * @brief   Low level wrapper function to call a driver for an ECC hash verification
 *          with a SECP 192 R1 key.
 *
 * @details See @ref psa_verify_hash()
 *
 * @param attributes
 * @param alg
 * @param key_buffer
 * @param key_buffer_size
 * @param hash
 * @param hash_length
 * @param signature
 * @param signature_length
 * @return psa_status_t
 */
psa_status_t psa_ecc_p192r1_verify_hash(const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg,
                                        const uint8_t *key_buffer, size_t key_buffer_size,
                                        const uint8_t *hash, size_t hash_length,
                                        const uint8_t *signature, size_t signature_length);

/**
 * @brief   Low level wrapper function to call a driver for an ECC key generation
 *          with a SECP 192 R1 key.
 *
 * @details See @ref psa_generate_key()
 *
 * @param   attributes
 * @param   priv_key_buffer
 * @param   pub_key_buffer
 * @param   priv_key_buffer_length
 * @param   pub_key_buffer_length
 * @return  @ref psa_status_t
 */
psa_status_t psa_generate_ecc_p256r1_key_pair(  const psa_key_attributes_t *attributes,
                                                uint8_t *priv_key_buffer, uint8_t *pub_key_buffer,
                                                size_t *priv_key_buffer_length,
                                                size_t *pub_key_buffer_length);

/**
 * @brief   Low level wrapper function to call a driver for an ECC public key export
 *          of a SECP 256 R1 key.
 *
 * @details See @ref psa_export_public_key()
 *
 * @param attributes
 * @param key_buffer
 * @param key_buffer_size
 * @param data
 * @param data_size
 * @param data_length
 * @return psa_status_t
 */
psa_status_t psa_ecc_p256r1_export_public_key(  const psa_key_attributes_t *attributes,
                                                uint8_t *key_buffer,
                                                size_t key_buffer_size,
                                                uint8_t *data,
                                                size_t data_size,
                                                size_t *data_length);

/**
 * @brief   Low level wrapper function to call a driver for an ECC hash signature
 *          with a SECP 256 R1 key.
 *
 * @details See @ref psa_sign_hash()
 *
 * @param attributes
 * @param alg
 * @param key_buffer
 * @param key_buffer_size
 * @param hash
 * @param hash_length
 * @param signature
 * @param signature_size
 * @param signature_length
 * @return psa_status_t
 */
psa_status_t psa_ecc_p256r1_sign_hash(  const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg,
                                        const uint8_t *key_buffer, size_t key_buffer_size,
                                        const uint8_t *hash, size_t hash_length,
                                        uint8_t *signature, size_t signature_size,
                                        size_t *signature_length);

/**
 * @brief   Low level wrapper function to call a driver for an ECC hash verification
 *          with a SECP 256 R1 key.
 *
 * @details See @ref psa_verify_hash()
 *
 * @param attributes
 * @param alg
 * @param key_buffer
 * @param key_buffer_size
 * @param hash
 * @param hash_length
 * @param signature
 * @param signature_length
 * @return psa_status_t
 */
psa_status_t psa_ecc_p256r1_verify_hash(const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg,
                                        const uint8_t *key_buffer, size_t key_buffer_size,
                                        const uint8_t *hash, size_t hash_length,
                                        const uint8_t *signature, size_t signature_length);

#ifdef __cplusplus
}
#endif

#endif /* PSA_ECC_H */
/**@}*/
