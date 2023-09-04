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
 * @brief       Glue code translating between PSA Crypto and the CryptoCell 310 ECC P256 APIs
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "psa_cryptocell_310_ecc_common.h"

psa_status_t psa_generate_ecc_p256r1_key_pair(  const psa_key_attributes_t *attributes,
                                                uint8_t *priv_key_buffer,
                                                uint8_t *pub_key_buffer,
                                                size_t *priv_key_buffer_length,
                                                size_t *pub_key_buffer_length)
{
    *priv_key_buffer_length = PSA_BITS_TO_BYTES(attributes->bits);
    *pub_key_buffer_length = PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(attributes->type, attributes->bits);

    return cryptocell_310_common_ecc_generate_key_pair(priv_key_buffer, pub_key_buffer,
                                               (uint32_t *)priv_key_buffer_length,
                                               (uint32_t *)pub_key_buffer_length,
                                               CRYS_ECPKI_DomainID_secp256r1);
}

psa_status_t psa_ecc_p256r1_sign_hash(  const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg,
                                        const uint8_t *key_buffer,
                                        size_t key_buffer_size,
                                        const uint8_t *hash,
                                        size_t hash_length,
                                        uint8_t *signature,
                                        size_t signature_size,
                                        size_t *signature_length)
{
    (void)key_buffer_size;
    CRYS_ECPKI_HASH_OpMode_t hash_mode = MAP_PSA_HASH_TO_CRYS_HASH(PSA_ALG_GET_HASH(alg));
    *signature_length = signature_size;
    return cryptocell_310_common_ecc_sign_hash(key_buffer,
                                       PSA_BITS_TO_BYTES(attributes->bits),
                                       hash, hash_length, signature,
                                       signature_length, hash_mode,
                                       CRYS_ECPKI_DomainID_secp256r1);
}

psa_status_t psa_ecc_p256r1_verify_hash(const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg,
                                        const uint8_t *key_buffer,
                                        size_t key_buffer_size,
                                        const uint8_t *hash,
                                        size_t hash_length,
                                        const uint8_t *signature,
                                        size_t signature_length)
{
    CRYS_ECPKI_HASH_OpMode_t hash_mode = MAP_PSA_HASH_TO_CRYS_HASH(PSA_ALG_GET_HASH(alg));

    (void)attributes;
    return cryptocell_310_common_ecc_verify_hash(key_buffer, key_buffer_size,
                                         hash, hash_length, signature,
                                         signature_length, hash_mode,
                                         CRYS_ECPKI_DomainID_secp256r1);
}
