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
 *          See @ref psa_generate_key()
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
 * @brief   Low level wrapper function to call a driver for an ECC hash signature
 *          with a SECP 192 R1 key.
 *          See @ref psa_sign_hash()
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
 * @brief   Low level wrapper function to call a driver for an ECC hash signature
 *          with a SECP 192 R1 key.
 *          See @ref psa_sign_message()
 *
 * @param attributes
 * @param alg
 * @param key_buffer
 * @param key_buffer_size
 * @param input
 * @param input_length
 * @param signature
 * @param signature_size
 * @param signature_length
 * @return psa_status_t
 */
psa_status_t psa_ecc_p192r1_sign_message(  const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg,
                                        const uint8_t *key_buffer, size_t key_buffer_size,
                                        const uint8_t *input, size_t input_length,
                                        uint8_t *signature, size_t signature_size,
                                        size_t *signature_length);

/**
 * @brief   Low level wrapper function to call a driver for an ECC hash verification
 *          with a SECP 192 R1 key.
 *          See @ref psa_verify_hash()
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
 * @brief   Low level wrapper function to call a driver for an ECC hash verification
 *          with a SECP 192 R1 key.
 *          See @ref psa_verify_message()
 *
 * @param attributes
 * @param alg
 * @param key_buffer
 * @param key_buffer_size
 * @param input
 * @param input_length
 * @param signature
 * @param signature_length
 * @return psa_status_t
 */
psa_status_t psa_ecc_p192r1_verify_message(const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg,
                                        const uint8_t *key_buffer, size_t key_buffer_size,
                                        const uint8_t *input, size_t input_length,
                                        const uint8_t *signature, size_t signature_length);

/**
 * @brief   Low level wrapper function to call a driver for an ECC key generation
 *          with a SECP 192 R1 key.
 *          See @ref psa_generate_key()
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
 * @brief   Low level wrapper function to call a driver for an ECC hash signature
 *          with a SECP 256 R1 key.
 *          See @ref psa_sign_hash()
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
 * @brief   Low level wrapper function to call a driver for an ECC hash signature
 *          with a SECP 256 R1 key.
 *          See @ref psa_sign_message()
 *
 * @param attributes
 * @param alg
 * @param key_buffer
 * @param key_buffer_size
 * @param input
 * @param input_length
 * @param signature
 * @param signature_size
 * @param signature_length
 * @return psa_status_t
 */
psa_status_t psa_ecc_p256r1_sign_message(  const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg,
                                        const uint8_t *key_buffer, size_t key_buffer_size,
                                        const uint8_t *input, size_t input_length,
                                        uint8_t *signature, size_t signature_size,
                                        size_t *signature_length);

/**
 * @brief   Low level wrapper function to call a driver for an ECC hash verification
 *          with a SECP 256 R1 key.
 *          See @ref psa_verify_hash()
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

/**
 * @brief   Low level wrapper function to call a driver for an ECC hash verification
 *          with a SECP 256 R1 key.
 *          See @ref psa_verify_message()
 *
 * @param attributes
 * @param alg
 * @param key_buffer
 * @param key_buffer_size
 * @param input
 * @param input_length
 * @param signature
 * @param signature_length
 * @return psa_status_t
 */
psa_status_t psa_ecc_p256r1_verify_message(const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg,
                                        const uint8_t *key_buffer, size_t key_buffer_size,
                                        const uint8_t *input, size_t input_length,
                                        const uint8_t *signature, size_t signature_length);

/**
 * @brief   Low level wrapper function to call a driver for an ECC key generation
 *          with an ed25519 key.
 *          See @ref psa_generate_key()
 *
 * @param   priv_key_buffer
 * @param   pub_key_buffer
 * @param   priv_key_buffer_length
 * @param   pub_key_buffer_length
 * @return  @ref psa_status_t
 */
psa_status_t psa_generate_ecc_ed25519_key_pair( uint8_t *priv_key_buffer, uint8_t *pub_key_buffer,
                                                size_t *priv_key_buffer_length,
                                                size_t *pub_key_buffer_length);

/**
 * @brief   Low level wrapper function to call a driver for deriving an ed25519 public key from the private key.
 *
 * @param[in]       priv_key_buffer
 * @param[out]      pub_key_buffer
 * @param[in]       priv_key_buffer_length
 * @param[inout]    pub_key_buffer_length
 * @return          @ref psa_status_t
 */
psa_status_t psa_derive_ecc_ed25519_public_key( const uint8_t *priv_key_buffer,
                                                uint8_t *pub_key_buffer,
                                                size_t priv_key_buffer_length,
                                                size_t *pub_key_buffer_length);

/**
 * @brief   Low level wrapper function to call a driver for an ECC hash signature
 *          with an ed25519 key.
 *          See @ref psa_sign_message()
 *
 * @param priv_key_buffer
 * @param priv_key_buffer_size
 * @param pub_key_buffer
 * @param pub_key_buffer_size
 * @param input
 * @param input_length
 * @param signature
 * @param signature_size
 * @param signature_length
 * @return psa_status_t
 */
psa_status_t psa_ecc_ed25519_sign_message(const uint8_t *priv_key_buffer,
                                        size_t priv_key_buffer_size,
                                        const uint8_t *pub_key_buffer,
                                        size_t pub_key_buffer_size,
                                        const uint8_t *input, size_t input_length,
                                        uint8_t *signature, size_t signature_size,
                                        size_t *signature_length);

/**
 * @brief   Low level wrapper function to call a driver for an ECC hash verification
 *          with a ed25519 key.
 *          See @ref psa_verify_message()
 *
 * @param key_buffer
 * @param key_buffer_size
 * @param input
 * @param input_length
 * @param signature
 * @param signature_length
 * @return psa_status_t
 */
psa_status_t psa_ecc_ed25519_verify_message(const uint8_t *key_buffer, size_t key_buffer_size,
                                            const uint8_t *input, size_t input_length,
                                            const uint8_t *signature, size_t signature_length);

#ifdef __cplusplus
}
#endif

#endif /* PSA_ECC_H */
/**@}*/
