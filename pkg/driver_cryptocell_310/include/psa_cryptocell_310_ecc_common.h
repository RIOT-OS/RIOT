/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_driver_cryptocell_310
 * @{
 *
 * @file
 * @brief       Common ECC functions used by all PSA Crypto wrappers
 *              for the CryptoCell 310 ECC APIs.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_CRYPTOCELL_310_ECC_COMMON_H
#define PSA_CRYPTOCELL_310_ECC_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"

#include "sns_silib.h"
#include "crys_ecpki_build.h"
#include "crys_ecpki_ecdsa.h"
#include "crys_ecpki_kg.h"
#include "crys_ecpki_domain.h"

/**
 * @brief   Map PSA hash encodings to CryptoCell specific hash encodings
 *
 * @param   hash psa_algorithm_t
 */
#define MAP_PSA_HASH_TO_CRYS_HASH_AFTER(hash) \
    ((hash == PSA_ALG_SHA_1) ? CRYS_ECPKI_AFTER_HASH_SHA1_mode : \
     (hash == PSA_ALG_SHA_224) ? CRYS_ECPKI_AFTER_HASH_SHA224_mode : \
     (hash == PSA_ALG_SHA_256) ? CRYS_ECPKI_AFTER_HASH_SHA256_mode : \
     (hash == PSA_ALG_SHA_384) ? CRYS_ECPKI_AFTER_HASH_SHA384_mode : \
     (hash == PSA_ALG_SHA_512) ? CRYS_ECPKI_AFTER_HASH_SHA512_mode : \
     0)

/**
 * @brief   Map PSA hash encodings to CryptoCell specific hash encodings
 *
 * @param   hash psa_algorithm_t
 */
#define MAP_PSA_HASH_TO_CRYS_HASH_BEFORE(hash) \
    ((hash == PSA_ALG_SHA_1) ? CRYS_ECPKI_HASH_SHA1_mode : \
     (hash == PSA_ALG_SHA_224) ? CRYS_ECPKI_HASH_SHA224_mode : \
     (hash == PSA_ALG_SHA_256) ? CRYS_ECPKI_HASH_SHA256_mode : \
     (hash == PSA_ALG_SHA_384) ? CRYS_ECPKI_HASH_SHA384_mode : \
     (hash == PSA_ALG_SHA_512) ? CRYS_ECPKI_HASH_SHA512_mode : \
     0)

/**
 * @brief   Common ECC key generation function
 *
 * @param   priv_key_buffer         Output buffer to write ECC private key
 * @param   pub_key_buffer          Output buffer to write ECC public key
 * @param   priv_key_buffer_length  Output pointer to write private key length
 * @param   pub_key_buffer_length   Output pointer to write public key length
 * @param   domain                  ECC domain of type @c CRYS_ECPKI_DomainID_t
 * @return  psa_status_t
 */
psa_status_t cryptocell_310_common_ecc_generate_key_pair(uint8_t *priv_key_buffer,
                                                 uint8_t *pub_key_buffer,
                                                 uint32_t *priv_key_buffer_length,
                                                 uint32_t *pub_key_buffer_length,
                                                 CRYS_ECPKI_DomainID_t domain);

/**
 * @brief   Common ECC signature function
 *
 * @param   priv_key            Pointer to ECC private key
 * @param   priv_key_size       Size of private key
 * @param   input               Input to sign (hash or original message depending on @c hash_mode )
 * @param   input_length        Length of the input
 * @param   signature           Output buffer to write the generated signature
 * @param   signature_length    Pointer to store the actual length of the signature
 * @param   hash_mode           Mode used to hash the message of type @c CRYS_ECPKI_HASH_OpMode_t
 * @param   domain              ECC domain of type @c CRYS_ECPKI_DomainID_t
 * @return  psa_status_t
 */
psa_status_t cryptocell_310_common_ecc_sign(const uint8_t *priv_key,
                                         uint32_t priv_key_size,
                                         const uint8_t *input,
                                         size_t input_length,
                                         uint8_t *signature,
                                         size_t *signature_length,
                                         CRYS_ECPKI_HASH_OpMode_t hash_mode,
                                         CRYS_ECPKI_DomainID_t domain);

/**
 * @brief   Common ECC verification function
 *
 * @param   pub_key             Pointer to ECC public key
 * @param   pub_key_size        Size of public key
 * @param   input               Input to verify (hash or original message depending on @c hash_mode )
 * @param   input_length        Length of the input
 * @param   signature           Buffer containing the signature to be verified
 * @param   signature_length    Actual length of the signature
 * @param   hash_mode           Mode used to hash the message of type
 *                              @c CRYS_ECPKI_HASH_OpMode_t
 * @param   domain              ECC domain of type @c CRYS_ECPKI_DomainID_t
 * @return  psa_status_t
 */
psa_status_t cryptocell_310_common_ecc_verify(const uint8_t *pub_key,
                                           size_t pub_key_size,
                                           const uint8_t *input,
                                           size_t input_length,
                                           const uint8_t *signature,
                                           size_t signature_length,
                                           CRYS_ECPKI_HASH_OpMode_t hash_mode,
                                           CRYS_ECPKI_DomainID_t domain);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PSA_CRYPTOCELL_310_ECC_COMMON_H */
