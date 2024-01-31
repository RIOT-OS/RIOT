/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto sys_psa_crypto_alg_disp
 * @{
 *
 * @file
 * @brief       Dispatch calls from the PSA Crypto API to an available backend.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include "kernel_defines.h"
#include "psa/crypto.h"

#if IS_USED(MODULE_PSA_MAC)
#include "psa_mac.h"
#endif

#if IS_USED(MODULE_PSA_HASH)
#include "psa_hashes.h"
#endif

#if IS_USED(MODULE_PSA_ASYMMETRIC)
#include "psa_ecc.h"
#endif

#if IS_USED(MODULE_PSA_CIPHER)
#include "psa_ciphers.h"
#endif

#if IS_USED(MODULE_PSA_KEY_MANAGEMENT)
#include "psa_crypto_operation_encoder.h"
#endif

#if IS_USED(MODULE_PSA_HASH)
psa_status_t psa_algorithm_dispatch_hash_setup(psa_hash_operation_t *operation,
                                               psa_algorithm_t alg)
{
    psa_status_t status = PSA_ERROR_NOT_SUPPORTED;

    switch (alg) {
    #if (IS_USED(MODULE_PSA_HASH_MD5))
    case PSA_ALG_MD5:
        status = psa_hashes_md5_setup(&operation->ctx.md5);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_1))
    case PSA_ALG_SHA_1:
        status = psa_hashes_sha1_setup(&operation->ctx.sha1);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_224))
    case PSA_ALG_SHA_224:
        status = psa_hashes_sha224_setup(&operation->ctx.sha224);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_256))
    case PSA_ALG_SHA_256:
        status = psa_hashes_sha256_setup(&operation->ctx.sha256);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_512))
    case PSA_ALG_SHA_512:
        status = psa_hashes_sha512_setup(&operation->ctx.sha512);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
    #endif
    default:
        (void)status;
        (void)operation;
        return PSA_ERROR_NOT_SUPPORTED;
    }

    operation->alg = alg;
    return PSA_SUCCESS;
}

psa_status_t psa_algorithm_dispatch_hash_update(psa_hash_operation_t *operation,
                                                const uint8_t *input,
                                                size_t input_length)
{
    switch (operation->alg) {
    #if (IS_USED(MODULE_PSA_HASH_MD5))
    case PSA_ALG_MD5:
        return psa_hashes_md5_update(&operation->ctx.md5, input, input_length);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_1))
    case PSA_ALG_SHA_1:
        return psa_hashes_sha1_update(&operation->ctx.sha1, input, input_length);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_224))
    case PSA_ALG_SHA_224:
        return psa_hashes_sha224_update(&operation->ctx.sha224, input, input_length);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_256))
    case PSA_ALG_SHA_256:
        return psa_hashes_sha256_update(&operation->ctx.sha256, input, input_length);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_512))
    case PSA_ALG_SHA_512:
        return psa_hashes_sha512_update(&operation->ctx.sha512, input, input_length);
    #endif
    default:
        (void)operation;
        (void)input;
        (void)input_length;
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

psa_status_t psa_algorithm_dispatch_hash_finish(psa_hash_operation_t *operation,
                                                uint8_t *hash,
                                                size_t hash_size,
                                                size_t *hash_length)
{
    switch (operation->alg) {
    #if (IS_USED(MODULE_PSA_HASH_MD5))
    case PSA_ALG_MD5:
        return psa_hashes_md5_finish(&operation->ctx.md5, hash, hash_size, hash_length);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_1))
    case PSA_ALG_SHA_1:
        return psa_hashes_sha1_finish(&operation->ctx.sha1, hash, hash_size, hash_length);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_224))
    case PSA_ALG_SHA_224:
        return psa_hashes_sha224_finish(&operation->ctx.sha224, hash, hash_size, hash_length);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_256))
    case PSA_ALG_SHA_256:
        return psa_hashes_sha256_finish(&operation->ctx.sha256, hash, hash_size, hash_length);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_512))
    case PSA_ALG_SHA_512:
        return psa_hashes_sha512_finish(&operation->ctx.sha512, hash, hash_size, hash_length);
    #endif
    default:
        (void)operation;
        (void)hash;
        (void)hash_size;
        (void)hash_length;
        return PSA_ERROR_NOT_SUPPORTED;
    }
}
#endif /* MODULE_PSA_HASH */

#if IS_USED(MODULE_PSA_ASYMMETRIC)
psa_status_t psa_algorithm_dispatch_sign_hash(  const psa_key_attributes_t *attributes,
                                                psa_algorithm_t alg,
                                                const psa_key_slot_t *slot,
                                                const uint8_t *hash,
                                                size_t hash_length,
                                                uint8_t *signature,
                                                size_t signature_size,
                                                size_t *signature_length)
{
    psa_asym_key_t asym_key = PSA_INVALID_OPERATION;
    uint8_t *key_data = NULL;
    size_t *key_bytes = NULL;
    uint8_t *pub_key_data = NULL;
    size_t *pub_key_bytes = NULL;

    if (PSA_KEY_TYPE_IS_ECC_KEY_PAIR(attributes->type)) {
        asym_key =
            PSA_ENCODE_ECC_KEY_TYPE(attributes->bits, PSA_KEY_TYPE_ECC_GET_FAMILY(attributes->type));

        if (asym_key == PSA_INVALID_OPERATION) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
    }

    psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

    switch (asym_key) {
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P192R1)
    case PSA_ECC_P192_R1:
        return psa_ecc_p192r1_sign_hash(attributes, alg, key_data, *key_bytes, hash, hash_length,
                                        signature, signature_size, signature_length);
#endif
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P256R1)
    case PSA_ECC_P256_R1:
        return psa_ecc_p256r1_sign_hash(attributes, alg, key_data, *key_bytes, hash, hash_length,
                                        signature, signature_size, signature_length);
#endif
    default:
        (void)alg;
        (void)slot;
        (void)hash;
        (void)hash_length;
        (void)signature;
        (void)signature_size;
        (void)signature_length;
        (void)pub_key_data;
        (void)pub_key_bytes;
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

psa_status_t psa_algorithm_dispatch_sign_message(const psa_key_attributes_t *attributes,
                                                psa_algorithm_t alg,
                                                const psa_key_slot_t *slot,
                                                const uint8_t *input,
                                                size_t input_length,
                                                uint8_t *signature,
                                                size_t signature_size,
                                                size_t *signature_length)
{
    psa_asym_key_t asym_key = PSA_INVALID_OPERATION;
    uint8_t *key_data = NULL;
    size_t *key_bytes = NULL;
    uint8_t *pub_key_data = NULL;
    size_t *pub_key_bytes = NULL;

    if (PSA_KEY_TYPE_IS_ECC_KEY_PAIR(attributes->type)) {
        asym_key =
            PSA_ENCODE_ECC_KEY_TYPE(attributes->bits, PSA_KEY_TYPE_ECC_GET_FAMILY(attributes->type));

        if (asym_key == PSA_INVALID_OPERATION) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
    }

    psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

    switch (asym_key) {
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P192R1)
    case PSA_ECC_P192_R1:
        return psa_ecc_p192r1_sign_message(attributes, alg, key_data, *key_bytes, input, input_length,
                                        signature, signature_size, signature_length);
#endif
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P256R1)
    case PSA_ECC_P256_R1:
        return psa_ecc_p256r1_sign_message(attributes, alg, key_data, *key_bytes, input, input_length,
                                        signature, signature_size, signature_length);
#endif
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_ED25519)
    case PSA_ECC_ED25519:
        psa_get_public_key_data_from_key_slot(slot, &pub_key_data, &pub_key_bytes);
        return psa_ecc_ed25519_sign_message(key_data, *key_bytes, pub_key_data, *pub_key_bytes, input, input_length,
                                    signature, signature_size, signature_length);
#endif
    default:
        (void)alg;
        (void)slot;
        (void)input;
        (void)input_length;
        (void)signature;
        (void)signature_size;
        (void)signature_length;
        (void)pub_key_data;
        (void)pub_key_bytes;
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

psa_status_t psa_algorithm_dispatch_verify_hash(  const psa_key_attributes_t *attributes,
                                                  psa_algorithm_t alg,
                                                  const psa_key_slot_t *slot,
                                                  const uint8_t *hash,
                                                  size_t hash_length,
                                                  const uint8_t *signature,
                                                  size_t signature_length)
{
    psa_asym_key_t asym_key = PSA_INVALID_OPERATION;
    uint8_t *pubkey_data = NULL;
    size_t *pubkey_data_len = NULL;

    if (PSA_KEY_TYPE_IS_ECC(attributes->type)) {
        asym_key =
            PSA_ENCODE_ECC_KEY_TYPE(attributes->bits, PSA_KEY_TYPE_ECC_GET_FAMILY(attributes->type));

        if (asym_key == PSA_INVALID_OPERATION) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
    }

    psa_get_public_key_data_from_key_slot(slot, &pubkey_data, &pubkey_data_len);

    switch (asym_key) {
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P192R1)
    case PSA_ECC_P192_R1:
        return psa_ecc_p192r1_verify_hash(attributes, alg, pubkey_data, *pubkey_data_len, hash,
                                          hash_length, signature, signature_length);
#endif
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P256R1)
    case PSA_ECC_P256_R1:
        return psa_ecc_p256r1_verify_hash(attributes, alg, pubkey_data, *pubkey_data_len, hash,
                                          hash_length, signature, signature_length);
#endif
    default:
        (void)alg;
        (void)slot;
        (void)hash;
        (void)hash_length;
        (void)signature;
        (void)signature_length;
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

psa_status_t psa_algorithm_dispatch_verify_message(const psa_key_attributes_t *attributes,
                                                  psa_algorithm_t alg,
                                                  const psa_key_slot_t *slot,
                                                  const uint8_t *input,
                                                  size_t input_length,
                                                  const uint8_t *signature,
                                                  size_t signature_length)
{
    psa_asym_key_t asym_key = PSA_INVALID_OPERATION;
    uint8_t *pubkey_data = NULL;
    size_t *pubkey_data_len = NULL;

    if (PSA_KEY_TYPE_IS_ECC(attributes->type)) {
        asym_key =
            PSA_ENCODE_ECC_KEY_TYPE(attributes->bits, PSA_KEY_TYPE_ECC_GET_FAMILY(attributes->type));

        if (asym_key == PSA_INVALID_OPERATION) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
    }

    psa_get_public_key_data_from_key_slot(slot, &pubkey_data, &pubkey_data_len);

    switch (asym_key) {
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P192R1)
    case PSA_ECC_P192_R1:
        return psa_ecc_p192r1_verify_message(attributes, alg, pubkey_data, *pubkey_data_len, input,
                                        input_length, signature, signature_length);
#endif
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P256R1)
    case PSA_ECC_P256_R1:
        return psa_ecc_p256r1_verify_message(attributes, alg, pubkey_data, *pubkey_data_len, input,
                                        input_length, signature, signature_length);
#endif
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_ED25519)
    case PSA_ECC_ED25519:
        return psa_ecc_ed25519_verify_message(pubkey_data, *pubkey_data_len, input,
                                        input_length, signature, signature_length);
#endif
    default:
        (void)alg;
        (void)slot;
        (void)input;
        (void)input_length;
        (void)signature;
        (void)signature_length;
        return PSA_ERROR_NOT_SUPPORTED;
    }
}
#endif /* MODULE_PSA_ASYMMETRIC */

#if IS_USED(MODULE_PSA_KEY_MANAGEMENT)
psa_status_t psa_algorithm_dispatch_generate_key(   const psa_key_attributes_t *attributes,
                                                    psa_key_slot_t *slot)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    uint8_t *key_data = NULL;
    size_t *key_bytes = NULL;

    psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

    /**
     * Only asymmetric key generation needs special key generation algorithms. Symmetric keys can
     * be created by generating random bytes.
     */
    if (PSA_KEY_TYPE_IS_ASYMMETRIC(attributes->type)) {
        psa_asym_key_t asym_key = PSA_INVALID_OPERATION;
        uint8_t *pubkey_data = NULL;
        size_t *pubkey_data_len = NULL;
        psa_get_public_key_data_from_key_slot(slot, &pubkey_data, &pubkey_data_len);

        if (PSA_KEY_TYPE_IS_ECC_KEY_PAIR(attributes->type)) {
            asym_key =
                PSA_ENCODE_ECC_KEY_TYPE(attributes->bits,
                                        PSA_KEY_TYPE_ECC_GET_FAMILY(attributes->type));

            if (asym_key == PSA_INVALID_OPERATION) {
                return PSA_ERROR_INVALID_ARGUMENT;
            }
        }

        switch (asym_key) {
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P192R1)
        case PSA_ECC_P192_R1:
            return psa_generate_ecc_p192r1_key_pair(attributes, key_data, pubkey_data, key_bytes,
                                                    pubkey_data_len);
#endif
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P256R1)
        case PSA_ECC_P256_R1:
            return psa_generate_ecc_p256r1_key_pair(attributes, key_data, pubkey_data, key_bytes,
                                                    pubkey_data_len);
#endif
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_ED25519)
        case PSA_ECC_ED25519:
            return psa_generate_ecc_ed25519_key_pair(key_data, pubkey_data, key_bytes, pubkey_data_len);
#endif
        default:
            (void)status;
            (void)slot;
            return PSA_ERROR_NOT_SUPPORTED;
        }
    }

    return psa_builtin_generate_key(attributes, key_data, *key_bytes, key_bytes);
}
#endif /* MODULE_PSA_KEY_MANAGEMENT */

#if IS_USED(MODULE_PSA_CIPHER)
psa_status_t psa_algorithm_dispatch_cipher_encrypt( const psa_key_attributes_t *attributes,
                                                    psa_algorithm_t alg,
                                                    const psa_key_slot_t *slot,
                                                    const uint8_t *input,
                                                    size_t input_length,
                                                    uint8_t *output,
                                                    size_t output_size,
                                                    size_t *output_length)
{
    psa_cipher_op_t op = PSA_ENCODE_CIPHER_OPERATION(alg, attributes->bits, attributes->type);
    uint8_t *key_data = NULL;
    size_t *key_bytes = NULL;

    psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

    if (op == PSA_INVALID_OPERATION) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    switch (op) {
#if IS_USED(MODULE_PSA_CIPHER_AES_128_CBC)
    case PSA_CBC_NO_PAD_AES_128:
        return psa_cipher_cbc_aes_128_encrypt(attributes, key_data, *key_bytes, alg, input,
                                              input_length, output, output_size, output_length);
#endif
#if IS_USED(MODULE_PSA_CIPHER_AES_192_CBC)
    case PSA_CBC_NO_PAD_AES_192:
        return psa_cipher_cbc_aes_192_encrypt(attributes, key_data, *key_bytes, alg, input,
                                              input_length, output, output_size, output_length);
#endif
#if IS_USED(MODULE_PSA_CIPHER_AES_256_CBC)
    case PSA_CBC_NO_PAD_AES_256:
        return psa_cipher_cbc_aes_256_encrypt(attributes, key_data, *key_bytes, alg, input,
                                              input_length, output, output_size, output_length);
#endif
    default:
        (void)slot;
        (void)input;
        (void)input_length;
        (void)output;
        (void)output_size;
        (void)output_length;
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

psa_status_t psa_algorithm_dispatch_cipher_decrypt( const psa_key_attributes_t *attributes,
                                                    psa_algorithm_t alg,
                                                    const psa_key_slot_t *slot,
                                                    const uint8_t *input,
                                                    size_t input_length,
                                                    uint8_t *output,
                                                    size_t output_size,
                                                    size_t *output_length)
{
    psa_cipher_op_t op = PSA_ENCODE_CIPHER_OPERATION(alg, attributes->bits, attributes->type);
    uint8_t *key_data = NULL;
    size_t *key_bytes = NULL;

    psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

    if (op == PSA_INVALID_OPERATION) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    switch (op) {
#if IS_USED(MODULE_PSA_CIPHER_AES_128_CBC)
    case PSA_CBC_NO_PAD_AES_128:
        return psa_cipher_cbc_aes_128_decrypt(attributes, key_data, *key_bytes, alg, input,
                                              input_length, output, output_size, output_length);
#endif
#if IS_USED(MODULE_PSA_CIPHER_AES_192_CBC)
    case PSA_CBC_NO_PAD_AES_192:
        return psa_cipher_cbc_aes_192_decrypt(attributes, key_data, *key_bytes, alg, input,
                                              input_length, output, output_size, output_length);
#endif
#if IS_USED(MODULE_PSA_CIPHER_AES_256_CBC)
    case PSA_CBC_NO_PAD_AES_256:
        return psa_cipher_cbc_aes_256_decrypt(attributes, key_data, *key_bytes, alg, input,
                                              input_length, output, output_size, output_length);
#endif
    default:
        (void)slot;
        (void)input;
        (void)input_length;
        (void)output;
        (void)output_size;
        (void)output_length;
        return PSA_ERROR_NOT_SUPPORTED;
    }
}
#endif /* MODULE_PSA_CIPHER */

#if IS_USED(MODULE_PSA_MAC)
psa_status_t psa_algorithm_dispatch_mac_compute(const psa_key_attributes_t *attributes,
                                                psa_algorithm_t alg,
                                                const psa_key_slot_t *slot,
                                                const uint8_t *input,
                                                size_t input_length,
                                                uint8_t *mac,
                                                size_t mac_size,
                                                size_t *mac_length)
{
    psa_status_t status = PSA_ERROR_NOT_SUPPORTED;
    uint8_t *key_data = NULL;
    size_t *key_bytes = NULL;

    psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

    switch (alg) {
    #if IS_USED(MODULE_PSA_MAC_HMAC_SHA_256)
    case PSA_ALG_HMAC(PSA_ALG_SHA_256):
        status = psa_mac_compute_hmac_sha256(attributes, key_data, *key_bytes, input, input_length,
                                             mac, mac_size, mac_length);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
    #endif
    default:
        (void)status;
        return PSA_ERROR_NOT_SUPPORTED;
    }

    (void)attributes;
    (void)input;
    (void)input_length;
    (void)mac;
    (void)mac_size;
    (void)mac_length;
    return PSA_SUCCESS;
}
#endif /* MODULE_PSA_MAC */
