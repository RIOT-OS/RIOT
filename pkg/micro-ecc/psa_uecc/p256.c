/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto pkg_micro-ecc
 * @{
 *
 * @brief       Glue code translating between PSA Crypto and the Micro-ECC APIs
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "psa/crypto.h"
#include "uECC.h"

psa_status_t psa_generate_ecc_p256r1_key_pair(  const psa_key_attributes_t *attributes,
                                                uint8_t *priv_key_buffer, uint8_t *pub_key_buffer,
                                                size_t *priv_key_buffer_length,
                                                size_t *pub_key_buffer_length)
{
    int ret = 0;

    *priv_key_buffer_length = PSA_BITS_TO_BYTES(attributes->bits);
    *pub_key_buffer_length = PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(attributes->type, attributes->bits);

    const struct uECC_Curve_t *curve = uECC_secp256r1();

    ret = uECC_make_key(pub_key_buffer, priv_key_buffer, curve);
    if (!ret) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    return PSA_SUCCESS;
}

psa_status_t psa_ecc_p256r1_sign_hash(  const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg, const uint8_t *key_buffer,
                                        size_t key_buffer_size, const uint8_t *hash,
                                        size_t hash_length, uint8_t *signature,
                                        size_t signature_size, size_t *signature_length)
{
    int ret = 0;
    const struct uECC_Curve_t *curve = uECC_secp256r1();

    ret = uECC_sign(key_buffer, hash, hash_length, signature, curve);
    if (!ret) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    *signature_length = signature_size;

    (void)alg;
    (void)attributes;
    (void)key_buffer_size;
    return PSA_SUCCESS;
}

psa_status_t psa_ecc_p256r1_verify_hash(const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg, const uint8_t *key_buffer,
                                        size_t key_buffer_size, const uint8_t *hash,
                                        size_t hash_length, const uint8_t *signature,
                                        size_t signature_length)
{
    int ret = 0;
    const struct uECC_Curve_t *curve = uECC_secp256r1();

    ret = uECC_verify(key_buffer, hash, hash_length, signature, curve);
    if (!ret) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    (void)alg;
    (void)attributes;
    (void)key_buffer_size;
    (void)signature_length;
    return PSA_SUCCESS;
}
