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

psa_status_t psa_generate_ecc_p192r1_key_pair(  const psa_key_attributes_t *attributes,
                                                uint8_t *priv_key_buffer, uint8_t *pub_key_buffer,
                                                size_t *priv_key_buffer_length,
                                                size_t *pub_key_buffer_length)
{
    int ret = 0;

    *priv_key_buffer_length = PSA_BITS_TO_BYTES(attributes->bits);
    *pub_key_buffer_length = PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(attributes->type, attributes->bits);

    const struct uECC_Curve_t *curve = uECC_secp192r1();

    /**
     * Add 0x04 prefix for SEC 1 encoded uncompressed elliptic curve points.
     * Micro-ECC represents public keys in SEC 1 uncompressed format without the 0x04 prefix [1].
     * PSA Crypto uses a standard SEC 1 uncompressed representation [2].
     * [1] https://github.com/kmackay/micro-ecc/blob/master/README.md#point-representation
     * [2] https://arm-software.github.io/psa-api/crypto/1.1/api/keys/management.html#key-formats
     */
    pub_key_buffer[0] = 0x04;
    ret = uECC_make_key(pub_key_buffer+1, priv_key_buffer, curve);
    if (!ret) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    return PSA_SUCCESS;
}

psa_status_t psa_ecc_p192r1_sign_hash(  const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg, const uint8_t *key_buffer,
                                        size_t key_buffer_size, const uint8_t *hash,
                                        size_t hash_length, uint8_t *signature,
                                        size_t signature_size, size_t *signature_length)
{
    int ret = 0;
    const struct uECC_Curve_t *curve = uECC_secp192r1();

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

psa_status_t psa_ecc_p192r1_sign_message(  const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg, const uint8_t *key_buffer,
                                        size_t key_buffer_size, const uint8_t *input,
                                        size_t input_length, uint8_t *signature,
                                        size_t signature_size, size_t *signature_length)
{
    psa_status_t status;

    uint8_t hash[PSA_HASH_LENGTH(PSA_ALG_GET_HASH(alg))];
    size_t hash_length;

    status = psa_hash_compute(PSA_ALG_GET_HASH(alg), input, input_length, hash, sizeof(hash), &hash_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_ecc_p192r1_sign_hash(attributes, alg, key_buffer, key_buffer_size, hash, hash_length, signature, signature_size, signature_length);
}

psa_status_t psa_ecc_p192r1_verify_hash(const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg, const uint8_t *key_buffer,
                                        size_t key_buffer_size, const uint8_t *hash,
                                        size_t hash_length, const uint8_t *signature,
                                        size_t signature_length)
{
    int ret = 0;
    const struct uECC_Curve_t *curve = uECC_secp192r1();

    /* Micro-ECC expects uncompressed public key without 0x04 prefix */
    ret = uECC_verify(key_buffer+1, hash, hash_length, signature, curve);
    if (!ret) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    (void)alg;
    (void)attributes;
    (void)key_buffer_size;
    (void)signature_length;
    return PSA_SUCCESS;
}

psa_status_t psa_ecc_p192r1_verify_message(const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg, const uint8_t *key_buffer,
                                        size_t key_buffer_size, const uint8_t *input,
                                        size_t input_length, const uint8_t *signature,
                                        size_t signature_length)
{
    psa_status_t status;

    uint8_t hash[PSA_HASH_LENGTH(PSA_ALG_GET_HASH(alg))];
    size_t hash_length;

    status = psa_hash_compute(PSA_ALG_GET_HASH(alg), input, input_length, hash, sizeof(hash), &hash_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_ecc_p192r1_verify_hash(attributes, alg, key_buffer, key_buffer_size, hash, hash_length, signature, signature_length);
}
