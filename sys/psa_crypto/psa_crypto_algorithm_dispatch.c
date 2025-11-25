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

#if IS_USED(MODULE_PSA_AEAD)
#include "psa_aead.h"
#endif

#if IS_USED(MODULE_PSA_KEY_MANAGEMENT)
#include "psa_crypto_operation_encoder.h"
#include "psa_crypto_algorithm_dispatch.h"
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
    #if (IS_USED(MODULE_PSA_HASH_SHA_384))
    case PSA_ALG_SHA_384:
        status = psa_hashes_sha384_setup(&operation->ctx.sha384);
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
    #if (IS_USED(MODULE_PSA_HASH_SHA3_256))
    case PSA_ALG_SHA3_256:
        status = psa_hashes_sha3_256_setup(&operation->ctx.sha3);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA3_384))
    case PSA_ALG_SHA3_384:
        status = psa_hashes_sha3_384_setup(&operation->ctx.sha3);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA3_512))
    case PSA_ALG_SHA3_512:
        status = psa_hashes_sha3_512_setup(&operation->ctx.sha3);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_512_224))
    case PSA_ALG_SHA_512_224:
        status = psa_hashes_sha512_224_setup(&operation->ctx.sha512_224);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_512_256))
    case PSA_ALG_SHA_512_256:
        status = psa_hashes_sha512_256_setup(&operation->ctx.sha512_256);
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
    #if (IS_USED(MODULE_PSA_HASH_SHA_384))
    case PSA_ALG_SHA_384:
        return psa_hashes_sha384_update(&operation->ctx.sha384, input, input_length);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_512))
    case PSA_ALG_SHA_512:
        return psa_hashes_sha512_update(&operation->ctx.sha512, input, input_length);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA3_256))
    case PSA_ALG_SHA3_256:
        return psa_hashes_sha3_256_update(&operation->ctx.sha3, input, input_length);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA3_384))
    case PSA_ALG_SHA3_384:
        return psa_hashes_sha3_384_update(&operation->ctx.sha3, input, input_length);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA3_512))
    case PSA_ALG_SHA3_512:
        return psa_hashes_sha3_512_update(&operation->ctx.sha3, input, input_length);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_512_224))
    case PSA_ALG_SHA_512_224:
        return psa_hashes_sha512_224_update(&operation->ctx.sha512_224, input, input_length);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_512_256))
    case PSA_ALG_SHA_512_256:
        return psa_hashes_sha512_256_update(&operation->ctx.sha512_256, input, input_length);
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
    #if (IS_USED(MODULE_PSA_HASH_SHA_384))
    case PSA_ALG_SHA_384:
        return psa_hashes_sha384_finish(&operation->ctx.sha384, hash, hash_size, hash_length);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_512))
    case PSA_ALG_SHA_512:
        return psa_hashes_sha512_finish(&operation->ctx.sha512, hash, hash_size, hash_length);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_512_224))
    case PSA_ALG_SHA_512_224:
        return psa_hashes_sha512_224_finish(&operation->ctx.sha512_224, hash,
                                            hash_size, hash_length);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA_512_256))
    case PSA_ALG_SHA_512_256:
        return psa_hashes_sha512_256_finish(&operation->ctx.sha512_256, hash,
                                            hash_size, hash_length);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA3_256))
    case PSA_ALG_SHA3_256:
        return psa_hashes_sha3_256_finish(&operation->ctx.sha3, hash);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA3_384))
    case PSA_ALG_SHA3_384:
        return psa_hashes_sha3_384_finish(&operation->ctx.sha3, hash);
    #endif
    #if (IS_USED(MODULE_PSA_HASH_SHA3_512))
    case PSA_ALG_SHA3_512:
        return psa_hashes_sha3_512_finish(&operation->ctx.sha3, hash);
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
            PSA_ENCODE_ECC_KEY_TYPE(attributes->bits,
                                    PSA_KEY_TYPE_ECC_GET_FAMILY(attributes->type));

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
            PSA_ENCODE_ECC_KEY_TYPE(attributes->bits,
                                    PSA_KEY_TYPE_ECC_GET_FAMILY(attributes->type));

        if (asym_key == PSA_INVALID_OPERATION) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
    }

    psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

    switch (asym_key) {
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P192R1)
    case PSA_ECC_P192_R1:
        return psa_ecc_p192r1_sign_message(attributes, alg, key_data, *key_bytes, input,
                                           input_length,
                                           signature, signature_size, signature_length);
#endif
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P256R1)
    case PSA_ECC_P256_R1:
        return psa_ecc_p256r1_sign_message(attributes, alg, key_data, *key_bytes, input,
                                           input_length,
                                           signature, signature_size, signature_length);
#endif
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_ED25519)
    case PSA_ECC_ED25519:
        psa_get_public_key_data_from_key_slot(slot, &pub_key_data, &pub_key_bytes);
        assert(*key_bytes == 32);
        assert(*pub_key_bytes == 32);
        if (signature_size < 64) {
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }
        *signature_length = 64;
        return psa_ecc_ed25519_sign_message(key_data, pub_key_data,
                                            input, input_length, signature);
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
            PSA_ENCODE_ECC_KEY_TYPE(attributes->bits,
                                    PSA_KEY_TYPE_ECC_GET_FAMILY(attributes->type));

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
            PSA_ENCODE_ECC_KEY_TYPE(attributes->bits,
                                    PSA_KEY_TYPE_ECC_GET_FAMILY(attributes->type));

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
        assert(*pubkey_data_len == 32);
        if (signature_length < 64) {
            return PSA_ERROR_INVALID_SIGNATURE;
        }
        return psa_ecc_ed25519_verify_message(pubkey_data, input, input_length, signature);
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
            *key_bytes = 32;
            *pubkey_data_len = 32;
            return psa_generate_ecc_ed25519_key_pair(key_data, pubkey_data);
#endif
        default:
            (void)status;
            (void)slot;
            return PSA_ERROR_NOT_SUPPORTED;
        }
    }

    return psa_builtin_generate_key(attributes, key_data, *key_bytes, key_bytes);
}

psa_status_t psa_algorithm_dispatch_import_key(const psa_key_attributes_t *attributes,
                                               const uint8_t *data, size_t data_length,
                                               psa_key_slot_t *slot, size_t *bits)
{
    uint8_t *key_data = NULL;
    size_t *key_bytes = NULL;
    size_t key_data_size;

    key_data_size = psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

    /**
     * Asymmetric keys needs special import handling:
     * The public key needs to be derived from the imported private key.
     */
    if (PSA_KEY_TYPE_IS_KEY_PAIR(attributes->type)) {
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

        // derive and save public from private key
        psa_status_t ret = PSA_ERROR_NOT_SUPPORTED;
        switch (asym_key) {
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P192R1)
        case PSA_ECC_P192_R1:
            // todo: support for Weierstrass curves
            (void)slot;
            ret = PSA_ERROR_NOT_SUPPORTED;
            break;
#endif
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P256R1)
        case PSA_ECC_P256_R1:
            ret = psa_derive_ecc_p256r1_public_key(data, pubkey_data, data_length, pubkey_data_len);
            break;
#endif
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_ED25519)
        case PSA_ECC_ED25519:
            if (data_length < 32) {
                return PSA_ERROR_INVALID_ARGUMENT;
            }
            ret = psa_derive_ecc_ed25519_public_key(data, pubkey_data);
            *pubkey_data_len = 32;
            break;
#endif
        default:
            (void)slot;
            ret = PSA_ERROR_NOT_SUPPORTED;
            break;
        }
        if (ret == PSA_SUCCESS) {
            /* save private key data */
            memcpy(key_data, data, data_length);
            *key_bytes = data_length;
        }
        return ret;
    }
    else if (attributes->type == PSA_KEY_TYPE_HMAC && PSA_ALG_IS_HMAC(attributes->policy.alg) &&
            data_length > PSA_HASH_BLOCK_LENGTH(attributes->policy.alg)){
        psa_status_t ret = PSA_ERROR_NOT_SUPPORTED;
#if IS_USED(MODULE_PSA_HASH)
        /* must compute hash beforehand if key is too long */
        ret = psa_hash_compute(PSA_ALG_HMAC_GET_HASH(attributes->policy.alg), data, data_length, key_data,
                                PSA_HASH_MAX_BLOCK_SIZE, key_bytes);
        if (ret != PSA_SUCCESS) {
            return ret;
        }
        *bits = PSA_BYTES_TO_BITS(*key_bytes);
        slot->attr.bits = *bits;
#endif /* MODULE_PSA_HASH */
        return ret;
    }
    return psa_builtin_import_key(attributes, data, data_length, key_data, key_data_size,
                                  key_bytes, bits);
}

#endif /* MODULE_PSA_KEY_MANAGEMENT */

#if IS_USED(MODULE_PSA_CIPHER)

psa_status_t psa_algorithm_dispatch_cipher_encrypt_setup(   psa_cipher_operation_t *operation,
                                                            const psa_key_attributes_t *attributes,
                                                            const psa_key_slot_t *slot,
                                                            psa_algorithm_t alg)
{
    operation->cipher_instance = PSA_ENCODE_CIPHER_OPERATION(alg, attributes->type, attributes->bits);

    uint8_t *key_data = NULL;
    size_t *key_bytes = NULL;

    psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

    if (operation->cipher_instance == PSA_INVALID_OPERATION) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    switch (operation->cipher_instance) {
    #if IS_USED(MODULE_PSA_CIPHER_CHACHA20)
        case PSA_STREAM_CIPHER_CHACHA20:
            return psa_cipher_chacha20_encrypt_setup(&operation->backend_ctx.cipher_ctx.chacha20, key_data, *key_bytes);
    #endif
    default:
        (void)operation;
        (void)attributes;
        (void)slot;
        (void)alg;
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

psa_status_t psa_algorithm_dispatch_cipher_decrypt_setup(   psa_cipher_operation_t *operation,
                                                            const psa_key_attributes_t *attributes,
                                                            const psa_key_slot_t *slot,
                                                            psa_algorithm_t alg)
{
    operation->cipher_instance = PSA_ENCODE_CIPHER_OPERATION(alg, attributes->type, attributes->bits);

    uint8_t *key_data = NULL;
    size_t *key_bytes = NULL;

    psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

    if (operation->cipher_instance == PSA_INVALID_OPERATION) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    switch (operation->cipher_instance) {
    #if IS_USED(MODULE_PSA_CIPHER_CHACHA20)
        case PSA_STREAM_CIPHER_CHACHA20:
            return psa_cipher_chacha20_decrypt_setup(&operation->backend_ctx.cipher_ctx.chacha20, key_data, *key_bytes);
    #endif
    default:
        (void)operation;
        (void)attributes;
        (void)slot;
        (void)alg;
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

psa_status_t psa_algorithm_dispatch_cipher_finish(  psa_cipher_operation_t *operation,
                                                    uint8_t *output,
                                                    size_t output_size,
                                                    size_t *output_length)
{
    switch (operation->cipher_instance) {
    #if IS_USED(MODULE_PSA_CIPHER_CHACHA20)
        case PSA_STREAM_CIPHER_CHACHA20:
            return psa_cipher_chacha20_finish(  &operation->backend_ctx.cipher_ctx.chacha20,
                                                output, output_size, output_length);
    #endif
    default:
        (void)operation;
        (void)output;
        (void)output_size;
        (void)output_length;
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

psa_status_t psa_algorithm_dispatch_cipher_update(  psa_cipher_operation_t *operation,
                                                    const uint8_t *input,
                                                    size_t input_length,
                                                    uint8_t *output,
                                                    size_t output_size,
                                                    size_t *output_length)
{
    switch (operation->cipher_instance) {
    #if IS_USED(MODULE_PSA_CIPHER_CHACHA20)
        case PSA_STREAM_CIPHER_CHACHA20:
            return psa_cipher_chacha20_update(  &operation->backend_ctx.cipher_ctx.chacha20,
                                                input, input_length,
                                                output, output_size, output_length);
    #endif
    default:
        (void)operation;
        (void)input;
        (void)input_length;
        (void)output;
        (void)output_size;
        (void)output_length;
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

psa_status_t psa_algorithm_dispatch_cipher_set_iv(  psa_cipher_operation_t *operation,
                                                    const uint8_t *iv,
                                                    size_t iv_length)
{
    switch (operation->cipher_instance) {
    #if IS_USED(MODULE_PSA_CIPHER_CHACHA20)
        case PSA_STREAM_CIPHER_CHACHA20:
            return psa_cipher_chacha20_set_iv(  &operation->backend_ctx.cipher_ctx.chacha20,
                                                iv, iv_length);
    #endif
    default:
        (void)operation;
        (void)iv;
        (void)iv_length;
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

psa_status_t psa_algorithm_dispatch_cipher_encrypt(const psa_key_attributes_t *attributes,
                                          psa_algorithm_t alg,
                                          const psa_key_slot_t *slot,
                                          const uint8_t *input,
                                          size_t input_length,
                                          uint8_t *output,
                                          size_t output_size,
                                          size_t *output_length)
{
    psa_cipher_op_t op = PSA_ENCODE_CIPHER_OPERATION(alg, attributes->type, attributes->bits);

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
    #if IS_USED(MODULE_PSA_CIPHER_CHACHA20)
        case PSA_STREAM_CIPHER_CHACHA20:
            return psa_cipher_chacha20_encrypt(key_data, *key_bytes,
                                               input, input_length,
                                               output, output_size,
                                               output_length);
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
    psa_cipher_op_t op = PSA_ENCODE_CIPHER_OPERATION(alg, attributes->type, attributes->bits);

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
    #if IS_USED(MODULE_PSA_CIPHER_CHACHA20)
        case PSA_STREAM_CIPHER_CHACHA20:
            return psa_cipher_chacha20_decrypt(key_data, *key_bytes,
                                               input, input_length,
                                               output, output_size,
                                               output_length);
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

#if IS_USED(MODULE_PSA_AEAD)
psa_status_t psa_algorithm_dispatch_aead_encrypt(   const psa_key_attributes_t *attributes,
                                                    psa_algorithm_t alg,
                                                    const psa_key_slot_t *slot,
                                                    const uint8_t *nonce,
                                                    size_t nonce_length,
                                                    const uint8_t *additional_data,
                                                    size_t additional_data_length,
                                                    const uint8_t *plaintext,
                                                    size_t plaintext_length,
                                                    uint8_t *ciphertext,
                                                    size_t ciphertext_size,
                                                    size_t *ciphertext_length)
{
    psa_aead_op_t op = PSA_ENCODE_AEAD_OPERATION(alg, attributes->type, attributes->bits);

    uint8_t *key_data = NULL;
    size_t *key_bytes = NULL;

    psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

    if (attributes->type != PSA_KEY_TYPE_AES) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (op == PSA_INVALID_OPERATION) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    uint8_t tag_len = PSA_AEAD_TAG_LENGTH(attributes->type, attributes->bits, alg);

    switch (op) {
    #if IS_USED(MODULE_PSA_AEAD_AES_128_CCM)
        case PSA_CCM_AES_128:
            return psa_aead_aes_128_ccm_encrypt(attributes, key_data, *key_bytes, tag_len,
                                            nonce, nonce_length, additional_data,
                                            additional_data_length, plaintext,
                                            plaintext_length, ciphertext,
                                            ciphertext_size, ciphertext_length);
    #endif
    #if IS_USED(MODULE_PSA_AEAD_AES_192_CCM)
        case PSA_CCM_AES_192:
            return psa_aead_aes_192_ccm_encrypt(attributes, key_data, *key_bytes, tag_len,
                                            nonce, nonce_length, additional_data,
                                            additional_data_length, plaintext,
                                            plaintext_length, ciphertext,
                                            ciphertext_size, ciphertext_length);
    #endif
    #if IS_USED(MODULE_PSA_AEAD_AES_256_CCM)
        case PSA_CCM_AES_256:
            return psa_aead_aes_256_ccm_encrypt(attributes, key_data, *key_bytes, tag_len,
                                            nonce, nonce_length, additional_data,
                                            additional_data_length, plaintext,
                                            plaintext_length, ciphertext,
                                            ciphertext_size, ciphertext_length);
    #endif
        default:
            (void)attributes;
            (void)alg;
            (void)slot;
            (void)nonce;
            (void)nonce_length;
            (void)additional_data;
            (void)additional_data_length;
            (void)plaintext;
            (void)plaintext_length;
            (void)ciphertext;
            (void)ciphertext_size;
            (void)ciphertext_length;
            return PSA_ERROR_NOT_SUPPORTED;
    }
}

psa_status_t psa_algorithm_dispatch_aead_decrypt(   const psa_key_attributes_t *attributes,
                                                    psa_algorithm_t alg,
                                                    const psa_key_slot_t *slot,
                                                    const uint8_t *nonce,
                                                    size_t nonce_length,
                                                    const uint8_t *additional_data,
                                                    size_t additional_data_length,
                                                    const uint8_t *ciphertext,
                                                    size_t ciphertext_length,
                                                    uint8_t *plaintext,
                                                    size_t plaintext_size,
                                                    size_t *plaintext_length)
{
    psa_aead_op_t op = PSA_ENCODE_AEAD_OPERATION(alg, attributes->type, attributes->bits);

    uint8_t *key_data = NULL;
    size_t *key_bytes = NULL;

    psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

    if (attributes->type != PSA_KEY_TYPE_AES) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (op == PSA_INVALID_OPERATION) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    uint8_t tag_len = PSA_AEAD_TAG_LENGTH(attributes->type, attributes->bits, alg);

    switch (op) {
    #if IS_USED(MODULE_PSA_AEAD_AES_128_CCM)
        case PSA_CCM_AES_128:
            return psa_aead_aes_128_ccm_decrypt(attributes, key_data, *key_bytes, tag_len,
                                            nonce, nonce_length, additional_data,
                                            additional_data_length, ciphertext,
                                            ciphertext_length, plaintext,
                                            plaintext_size, plaintext_length);
    #endif
    #if IS_USED(MODULE_PSA_AEAD_AES_192_CCM)
        case PSA_CCM_AES_192:
            return psa_aead_aes_192_ccm_decrypt(attributes, key_data, *key_bytes, tag_len,
                                            nonce, nonce_length, additional_data,
                                            additional_data_length, ciphertext,
                                            ciphertext_length, plaintext,
                                            plaintext_size, plaintext_length);
    #endif
    #if IS_USED(MODULE_PSA_AEAD_AES_256_CCM)
        case PSA_CCM_AES_256:
            return psa_aead_aes_256_ccm_decrypt(attributes, key_data, *key_bytes, tag_len,
                                            nonce, nonce_length, additional_data,
                                            additional_data_length, ciphertext,
                                            ciphertext_length, plaintext,
                                            plaintext_size, plaintext_length);
    #endif
        default:
            (void)attributes;
            (void)alg;
            (void)slot;
            (void)nonce;
            (void)nonce_length;
            (void)additional_data;
            (void)additional_data_length;
            (void)ciphertext;
            (void)ciphertext_length;
            (void)plaintext;
            (void)plaintext_size;
            (void)plaintext_length;
            return PSA_ERROR_NOT_SUPPORTED;
    }
}
#endif /* MODULE_PSA_AEAD */

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
    uint8_t *key_data = NULL;
    size_t *key_bytes = NULL;

    psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

    if (!key_data || !key_bytes) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    switch (alg) {
#if IS_USED(MODULE_PSA_MAC_HMAC_MD5)
    case PSA_ALG_HMAC(PSA_ALG_MD5):
        return psa_mac_compute_hmac_md5(attributes, key_data, *key_bytes, input, input_length,
                                        mac, mac_size, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_1)
    case PSA_ALG_HMAC(PSA_ALG_SHA_1):
        return psa_mac_compute_hmac_sha1(attributes, key_data, *key_bytes, input, input_length,
                                         mac, mac_size, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_224)
    case PSA_ALG_HMAC(PSA_ALG_SHA_224):
        return psa_mac_compute_hmac_sha224(attributes, key_data, *key_bytes, input, input_length,
                                           mac, mac_size, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_256)
    case PSA_ALG_HMAC(PSA_ALG_SHA_256):
        return psa_mac_compute_hmac_sha256(attributes, key_data, *key_bytes, input, input_length,
                                           mac, mac_size, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_384)
    case PSA_ALG_HMAC(PSA_ALG_SHA_384):
        return psa_mac_compute_hmac_sha384(attributes, key_data, *key_bytes, input, input_length,
                                           mac, mac_size, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_512)
    case PSA_ALG_HMAC(PSA_ALG_SHA_512):
        return psa_mac_compute_hmac_sha512(attributes, key_data, *key_bytes, input, input_length,
                                           mac, mac_size, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_256)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_256):
        return psa_mac_compute_hmac_sha3_256(attributes, key_data, *key_bytes, input, input_length,
                                             mac, mac_size, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_384)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_384):
        return psa_mac_compute_hmac_sha3_384(attributes, key_data, *key_bytes, input, input_length,
                                             mac, mac_size, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_512)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_512):
        return psa_mac_compute_hmac_sha3_512(attributes, key_data, *key_bytes, input, input_length,
                                             mac, mac_size, mac_length);
#endif
    default:
        (void)attributes;
        (void)slot;
        (void)input;
        (void)input_length;
        (void)mac;
        (void)mac_size;
        (void)mac_length;
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

psa_status_t psa_algorithm_dispatch_mac_verify(const psa_key_attributes_t *attributes,
                                               psa_algorithm_t alg,
                                               const psa_key_slot_t *slot,
                                               const uint8_t *input,
                                               size_t input_length,
                                               const uint8_t *mac,
                                               size_t mac_length)
{
    uint8_t *key_data = NULL;
    size_t *key_bytes = NULL;

    psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

    if (!key_data || !key_bytes) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    switch (alg) {
#if IS_USED(MODULE_PSA_MAC_HMAC_MD5)
    case PSA_ALG_HMAC(PSA_ALG_MD5):
        return psa_mac_verify_hmac_md5(attributes, key_data, *key_bytes, input, input_length,
                                       mac, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_1)
    case PSA_ALG_HMAC(PSA_ALG_SHA_1):
        return psa_mac_verify_hmac_sha1(attributes, key_data, *key_bytes, input, input_length,
                                        mac, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_224)
    case PSA_ALG_HMAC(PSA_ALG_SHA_224):
        return psa_mac_verify_hmac_sha224(attributes, key_data, *key_bytes, input, input_length,
                                          mac, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_256)
    case PSA_ALG_HMAC(PSA_ALG_SHA_256):
        return psa_mac_verify_hmac_sha256(attributes, key_data, *key_bytes, input, input_length,
                                          mac, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_384)
    case PSA_ALG_HMAC(PSA_ALG_SHA_384):
        return psa_mac_verify_hmac_sha384(attributes, key_data, *key_bytes, input, input_length,
                                          mac, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_512)
    case PSA_ALG_HMAC(PSA_ALG_SHA_512):
        return psa_mac_verify_hmac_sha512(attributes, key_data, *key_bytes, input, input_length,
                                          mac, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_256)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_256):
        return psa_mac_verify_hmac_sha3_256(attributes, key_data, *key_bytes, input, input_length,
                                            mac, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_384)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_384):
        return psa_mac_verify_hmac_sha3_384(attributes, key_data, *key_bytes, input, input_length,
                                            mac, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_512)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_512):
        return psa_mac_verify_hmac_sha3_512(attributes, key_data, *key_bytes, input, input_length,
                                            mac, mac_length);
#endif
    default:
        (void)attributes;
        (void)slot;
        (void)input;
        (void)input_length;
        (void)mac;
        (void)mac_length;
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

psa_status_t psa_algorithm_dispatch_mac_sign_setup(psa_mac_operation_t *operation,
                                                   const psa_key_attributes_t *attributes,
                                                   const psa_key_slot_t *slot,
                                                   psa_algorithm_t alg)
{
    psa_status_t status = PSA_ERROR_NOT_SUPPORTED;
    uint8_t *key_data = NULL;
    size_t *key_bytes = NULL;

    psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

    if (!key_data || !key_bytes) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    switch (alg) {
#if IS_USED(MODULE_PSA_MAC_HMAC_MD5)
    case PSA_ALG_HMAC(PSA_ALG_MD5):
        status = psa_mac_sign_setup_hmac_md5(operation, attributes, key_data, *key_bytes);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_1)
    case PSA_ALG_HMAC(PSA_ALG_SHA_1):
        status = psa_mac_sign_setup_hmac_sha1(operation, attributes, key_data, *key_bytes);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_224)
    case PSA_ALG_HMAC(PSA_ALG_SHA_224):
        status = psa_mac_sign_setup_hmac_sha224(operation, attributes, key_data, *key_bytes);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_256)
    case PSA_ALG_HMAC(PSA_ALG_SHA_256):
        status = psa_mac_sign_setup_hmac_sha256(operation, attributes, key_data, *key_bytes);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_384)
    case PSA_ALG_HMAC(PSA_ALG_SHA_384):
        status = psa_mac_sign_setup_hmac_sha384(operation, attributes, key_data, *key_bytes);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_512)
    case PSA_ALG_HMAC(PSA_ALG_SHA_512):
        status = psa_mac_sign_setup_hmac_sha512(operation, attributes, key_data, *key_bytes);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_256)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_256):
        status = psa_mac_sign_setup_hmac_sha3_256(operation, attributes, key_data, *key_bytes);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_384)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_384):
        status = psa_mac_sign_setup_hmac_sha3_384(operation, attributes, key_data, *key_bytes);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_512)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_512):
        status = psa_mac_sign_setup_hmac_sha3_512(operation, attributes, key_data, *key_bytes);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
#endif
    default:
        (void)attributes;
        (void)operation;
        (void)alg;
        return status;
    }

    operation->alg = alg;

    return PSA_SUCCESS;
}

psa_status_t psa_algorithm_dispatch_mac_verify_setup(psa_mac_operation_t *operation,
                                                     const psa_key_attributes_t *attributes,
                                                     const psa_key_slot_t *slot,
                                                     psa_algorithm_t alg)
{
    psa_status_t status = PSA_ERROR_NOT_SUPPORTED;
    uint8_t *key_data = NULL;
    size_t *key_bytes = NULL;

    psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

    if (!key_data || !key_bytes) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    switch (alg) {
#if IS_USED(MODULE_PSA_MAC_HMAC_MD5)
    case PSA_ALG_HMAC(PSA_ALG_MD5):
        status = psa_mac_verify_setup_hmac_md5(operation, attributes, key_data, *key_bytes);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_1)
    case PSA_ALG_HMAC(PSA_ALG_SHA_1):
        status = psa_mac_verify_setup_hmac_sha1(operation, attributes, key_data, *key_bytes);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_224)
    case PSA_ALG_HMAC(PSA_ALG_SHA_224):
        status = psa_mac_verify_setup_hmac_sha224(operation, attributes, key_data, *key_bytes);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_256)
    case PSA_ALG_HMAC(PSA_ALG_SHA_256):
        status = psa_mac_verify_setup_hmac_sha256(operation, attributes, key_data, *key_bytes);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_384)
    case PSA_ALG_HMAC(PSA_ALG_SHA_384):
        status = psa_mac_verify_setup_hmac_sha384(operation, attributes, key_data, *key_bytes);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_512)
    case PSA_ALG_HMAC(PSA_ALG_SHA_512):
        status = psa_mac_verify_setup_hmac_sha512(operation, attributes, key_data, *key_bytes);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_256)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_256):
        status = psa_mac_verify_setup_hmac_sha3_256(operation, attributes, key_data, *key_bytes);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_384)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_384):
        status = psa_mac_verify_setup_hmac_sha3_384(operation, attributes, key_data, *key_bytes);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_512)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_512):
        status = psa_mac_verify_setup_hmac_sha3_512(operation, attributes, key_data, *key_bytes);
        if (status != PSA_SUCCESS) {
            return status;
        }
        break;
#endif
    default:
        (void)attributes;
        (void)operation;
        (void)alg;
        return status;
    }

    operation->alg = alg;

    return PSA_SUCCESS;
}

psa_status_t psa_algorithm_dispatch_mac_update(psa_mac_operation_t *operation,
                                               const uint8_t *input,
                                               size_t input_length)
{
    switch (operation->alg) {
#if IS_USED(MODULE_PSA_MAC_HMAC_MD5)
    case PSA_ALG_HMAC(PSA_ALG_MD5):
        return psa_mac_update_hmac_md5(operation, input, input_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_1)
    case PSA_ALG_HMAC(PSA_ALG_SHA_1):
        return psa_mac_update_hmac_sha1(operation, input, input_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_224)
    case PSA_ALG_HMAC(PSA_ALG_SHA_224):
        return psa_mac_update_hmac_sha224(operation, input, input_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_256)
    case PSA_ALG_HMAC(PSA_ALG_SHA_256):
        return psa_mac_update_hmac_sha256(operation, input, input_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_384)
    case PSA_ALG_HMAC(PSA_ALG_SHA_384):
        return psa_mac_update_hmac_sha384(operation, input, input_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_512)
    case PSA_ALG_HMAC(PSA_ALG_SHA_512):
        return psa_mac_update_hmac_sha512(operation, input, input_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_256)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_256):
        return psa_mac_update_hmac_sha3_256(operation, input, input_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_384)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_384):
        return psa_mac_update_hmac_sha3_384(operation, input, input_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_512)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_512):
        return psa_mac_update_hmac_sha3_512(operation, input, input_length);
#endif
    default:
        (void)operation;
        (void)input;
        (void)input_length;
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

psa_status_t psa_algorithm_dispatch_mac_sign_finish(psa_mac_operation_t *operation,
                                                    uint8_t *mac,
                                                    size_t mac_size,
                                                    size_t *mac_length)
{
    switch (operation->alg) {
#if IS_USED(MODULE_PSA_MAC_HMAC_MD5)
    case PSA_ALG_HMAC(PSA_ALG_MD5):
        return psa_mac_sign_finish_hmac_md5(operation, mac, mac_size, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_1)
    case PSA_ALG_HMAC(PSA_ALG_SHA_1):
        return psa_mac_sign_finish_hmac_sha1(operation, mac, mac_size, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_224)
    case PSA_ALG_HMAC(PSA_ALG_SHA_224):
        return psa_mac_sign_finish_hmac_sha224(operation, mac, mac_size, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_256)
    case PSA_ALG_HMAC(PSA_ALG_SHA_256):
        return psa_mac_sign_finish_hmac_sha256(operation, mac, mac_size, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_384)
    case PSA_ALG_HMAC(PSA_ALG_SHA_384):
        return psa_mac_sign_finish_hmac_sha384(operation, mac, mac_size, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_512)
    case PSA_ALG_HMAC(PSA_ALG_SHA_512):
        return psa_mac_sign_finish_hmac_sha512(operation, mac, mac_size, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_256)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_256):
        return psa_mac_sign_finish_hmac_sha3_256(operation, mac, mac_size, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_384)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_384):
        return psa_mac_sign_finish_hmac_sha3_384(operation, mac, mac_size, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_512)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_512):
        return psa_mac_sign_finish_hmac_sha3_512(operation, mac, mac_size, mac_length);
#endif
    default:
        (void)operation;
        (void)mac;
        (void)mac_size;
        (void)mac_length;
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

psa_status_t psa_algorithm_dispatch_mac_verify_finish(psa_mac_operation_t *operation,
                                                      const uint8_t *mac,
                                                      size_t mac_length)
{
    switch (operation->alg) {
#if IS_USED(MODULE_PSA_MAC_HMAC_MD5)
    case PSA_ALG_HMAC(PSA_ALG_MD5):
        return psa_mac_verify_finish_hmac_md5(operation, mac, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_1)
    case PSA_ALG_HMAC(PSA_ALG_SHA_1):
        return psa_mac_verify_finish_hmac_sha1(operation, mac, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_224)
    case PSA_ALG_HMAC(PSA_ALG_SHA_224):
        return psa_mac_verify_finish_hmac_sha224(operation, mac, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_256)
    case PSA_ALG_HMAC(PSA_ALG_SHA_256):
        return psa_mac_verify_finish_hmac_sha256(operation, mac, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_384)
    case PSA_ALG_HMAC(PSA_ALG_SHA_384):
        return psa_mac_verify_finish_hmac_sha384(operation, mac, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_512)
    case PSA_ALG_HMAC(PSA_ALG_SHA_512):
        return psa_mac_verify_finish_hmac_sha512(operation, mac, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_256)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_256):
        return psa_mac_verify_finish_hmac_sha3_256(operation, mac, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_384)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_384):
        return psa_mac_verify_finish_hmac_sha3_384(operation, mac, mac_length);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_512)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_512):
        return psa_mac_verify_finish_hmac_sha3_512(operation, mac, mac_length);
#endif
    default:
        (void)operation;
        (void)mac;
        (void)mac_length;
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

psa_status_t psa_algorithm_dispatch_mac_abort(psa_mac_operation_t *operation)
{
    switch (operation->alg) {
#if IS_USED(MODULE_PSA_MAC_HMAC_MD5)
    case PSA_ALG_HMAC(PSA_ALG_MD5):
        return psa_mac_abort_hmac_md5(operation);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_1)
    case PSA_ALG_HMAC(PSA_ALG_SHA_1):
        return psa_mac_abort_hmac_sha1(operation);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_224)
    case PSA_ALG_HMAC(PSA_ALG_SHA_224):
        return psa_mac_abort_hmac_sha224(operation);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_256)
    case PSA_ALG_HMAC(PSA_ALG_SHA_256):
        return psa_mac_abort_hmac_sha256(operation);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_384)
    case PSA_ALG_HMAC(PSA_ALG_SHA_384):
        return psa_mac_abort_hmac_sha384(operation);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA_512)
    case PSA_ALG_HMAC(PSA_ALG_SHA_512):
        return psa_mac_abort_hmac_sha512(operation);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_256)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_256):
        return psa_mac_abort_hmac_sha3_256(operation);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_384)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_384):
        return psa_mac_abort_hmac_sha3_384(operation);
#endif
#if IS_USED(MODULE_PSA_MAC_HMAC_SHA3_512)
    case PSA_ALG_HMAC(PSA_ALG_SHA3_512):
        return psa_mac_abort_hmac_sha3_512(operation);
#endif
    case PSA_ALG_NONE:
        /* Calling psa_mac_abort() on a fresh psa_mac_operation_t is valid */
        return PSA_SUCCESS;
    default:
        (void)operation;
        return PSA_ERROR_NOT_SUPPORTED;
    }
}
#endif /* MODULE_PSA_MAC */
