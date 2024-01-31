/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup examples
 * @{
 *
 * @brief   Example functions for ECDSA with PSA Crypto
 *
 * @author  Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "psa/crypto.h"

#define ECDSA_MESSAGE_SIZE  (127)

#define ECC_KEY_SIZE    (256)
#define ECC_KEY_TYPE (PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1))
#define ECC_ALG_HASH (PSA_ALG_SHA_256)
#define ECC_ALG (PSA_ALG_ECDSA(ECC_ALG_HASH))

/**
 * @brief   Example function to perform an ECDSA operation with a NIST P256 curve
 *          with the PSA Crypto API.
 *
 * @return  psa_status_t
 */
psa_status_t example_ecdsa_p256(void)
{
    psa_key_id_t privkey_id;
    psa_key_attributes_t privkey_attr = psa_key_attributes_init();
    psa_key_id_t pubkey_id;
    psa_key_attributes_t pubkey_attr = psa_key_attributes_init();

    psa_key_usage_t usage = PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH;

    uint8_t public_key[PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(ECC_KEY_TYPE, ECC_KEY_SIZE)] = { 0 };
    size_t pubkey_length;
    uint8_t signature[PSA_SIGN_OUTPUT_SIZE(ECC_KEY_TYPE, ECC_KEY_SIZE, ECC_ALG)];
    size_t sig_length;
    uint8_t msg[ECDSA_MESSAGE_SIZE] = { 0x0b };
    uint8_t hash[PSA_HASH_LENGTH(ECC_ALG_HASH)];
    size_t hash_length;

    psa_set_key_algorithm(&privkey_attr, ECC_ALG);
    psa_set_key_usage_flags(&privkey_attr, usage);
    psa_set_key_type(&privkey_attr, ECC_KEY_TYPE);
    psa_set_key_bits(&privkey_attr, ECC_KEY_SIZE);

#ifdef SECURE_ELEMENT
    psa_key_lifetime_t lifetime = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(
        PSA_KEY_LIFETIME_VOLATILE, PSA_ATCA_LOCATION_DEV0);
    psa_set_key_lifetime(&privkey_attr, lifetime);
#endif

    psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;

    status = psa_generate_key(&privkey_attr, &privkey_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_export_public_key(privkey_id, public_key, sizeof(public_key), &pubkey_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_hash_compute(ECC_ALG_HASH, msg, sizeof(msg), hash, sizeof(hash), &hash_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

#ifdef SECURE_ELEMENT
    /* Currently there is no support for message signature and verification on secure elements */
    psa_set_key_lifetime(&pubkey_attr, lifetime);
    psa_set_key_usage_flags(&pubkey_attr, PSA_KEY_USAGE_VERIFY_HASH);
#else
    psa_set_key_usage_flags(&pubkey_attr, PSA_KEY_USAGE_VERIFY_MESSAGE);
#endif
    psa_set_key_algorithm(&pubkey_attr, ECC_ALG);
    psa_set_key_bits(&pubkey_attr, PSA_BYTES_TO_BITS(pubkey_length));
    psa_set_key_type(&pubkey_attr, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));

    status = psa_import_key(&pubkey_attr, public_key, pubkey_length, &pubkey_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_sign_hash(privkey_id, ECC_ALG, hash, sizeof(hash), signature, sizeof(signature),
                           &sig_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

#ifdef SECURE_ELEMENT
    /* Currently there is only support for hash signature and verification on secure elements,
       so we can't verify the message, but only the hash */
    return psa_verify_hash(pubkey_id, ECC_ALG, hash, sizeof(hash), signature, sig_length);
#endif

    /* verify on original message with internal hashing operation */
    return psa_verify_message(pubkey_id, ECC_ALG, msg, sizeof(msg), signature, sig_length);
}

#ifdef MULTIPLE_SE
psa_status_t example_ecdsa_p256_sec_se(void)
{
    psa_key_id_t privkey_id;
    psa_key_attributes_t privkey_attr = psa_key_attributes_init();
    psa_key_id_t pubkey_id;
    psa_key_attributes_t pubkey_attr = psa_key_attributes_init();

    psa_key_lifetime_t lifetime = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(
        PSA_KEY_LIFETIME_VOLATILE, PSA_ATCA_LOCATION_DEV1);
    psa_key_usage_t usage = PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH;

    uint8_t public_key[PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(ECC_KEY_TYPE, ECC_KEY_SIZE)] = { 0 };
    size_t pubkey_length;

    uint8_t signature[PSA_SIGN_OUTPUT_SIZE(ECC_KEY_TYPE, ECC_KEY_SIZE, ECC_ALG)];
    size_t sig_length;
    uint8_t msg[ECDSA_MESSAGE_SIZE] = { 0x0b };
    uint8_t hash[PSA_HASH_LENGTH(ECC_ALG_HASH)];
    size_t hash_length;

    psa_set_key_lifetime(&privkey_attr, lifetime);
    psa_set_key_algorithm(&privkey_attr, ECC_ALG);
    psa_set_key_usage_flags(&privkey_attr, usage);
    psa_set_key_type(&privkey_attr, ECC_KEY_TYPE);
    psa_set_key_bits(&privkey_attr, ECC_KEY_SIZE);

    psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;

    status = psa_generate_key(&privkey_attr, &privkey_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_export_public_key(privkey_id, public_key, sizeof(public_key), &pubkey_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_hash_compute(ECC_ALG_HASH, msg, sizeof(msg), hash, sizeof(hash), &hash_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    psa_set_key_lifetime(&pubkey_attr, lifetime);
    psa_set_key_algorithm(&pubkey_attr, ECC_ALG);
    psa_set_key_usage_flags(&pubkey_attr, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_bits(&pubkey_attr, PSA_BYTES_TO_BITS(pubkey_length));
    psa_set_key_type(&pubkey_attr, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));

    status = psa_import_key(&pubkey_attr, public_key, pubkey_length, &pubkey_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_sign_hash(privkey_id, ECC_ALG, hash, sizeof(hash), signature, sizeof(signature),
                           &sig_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_verify_hash(pubkey_id, ECC_ALG, hash, sizeof(hash), signature, sig_length);
}
#endif
