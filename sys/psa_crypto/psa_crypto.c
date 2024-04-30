/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file
 * @brief       PSA Crypto API implementation
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include "psa/crypto.h"

#if IS_USED(MODULE_PSA_KEY_MANAGEMENT)
#include "psa_crypto_slot_management.h"
#endif

#include "psa_crypto_se_driver.h"
#include "psa_crypto_se_management.h"
#include "psa_crypto_location_dispatch.h"
#include "psa_crypto_algorithm_dispatch.h"

#if IS_USED(MODULE_PSA_PERSISTENT_STORAGE)
#include "psa_crypto_persistent_storage.h"
#endif /* MODULE_PSA_PERSISTENT_STORAGE */

#include "random.h"
#include "kernel_defines.h"

#define ENABLE_DEBUG    0
#include "debug.h"

/**
 * @brief   Set by psa_crypto_init, which is required to be called once before
 *          PSA Crypto. Must be checked by other operations first.
 */
static uint8_t lib_initialized = 0;

#if IS_USED(MODULE_PSA_HASH)
/**
 * @brief   Compares the content of two same-sized buffers while maintaining
 *          constant processing time
 *
 * @param   a Buffer A to compare with B
 * @param   b Buffer B to compare with A
 * @param   n Size of the input buffers
 *
 * @return  int
 *          0 if buffer contents are the same
 *          1 if buffer contents differ
 */
static inline int constant_time_memcmp(const uint8_t *a, const uint8_t *b, size_t n)
{
    uint8_t diff = 0;

    for (size_t i = 0; i < n; i++) {
        diff |= a[i] ^ b[i];
    }

    return diff;
}
#endif /* MODULE_PSA_HASH */

const char *psa_status_to_humanly_readable(psa_status_t status)
{
    switch (status) {
        case PSA_ERROR_GENERIC_ERROR:
            return "PSA_ERROR_GENERIC_ERROR";
        case PSA_ERROR_NOT_SUPPORTED:
            return "PSA_ERROR_NOT_SUPPORTED";
        case PSA_ERROR_NOT_PERMITTED:
            return "PSA_ERROR_NOT_PERMITTED";
        case PSA_ERROR_BUFFER_TOO_SMALL:
            return "PSA_ERROR_BUFFER_TOO_SMALL";
        case PSA_ERROR_ALREADY_EXISTS:
            return "PSA_ERROR_ALREADY_EXISTS";
        case PSA_ERROR_DOES_NOT_EXIST:
            return "PSA_ERROR_DOES_NOT_EXIST";
        case PSA_ERROR_BAD_STATE:
            return "PSA_ERROR_BAD_STATE";
        case PSA_ERROR_INVALID_ARGUMENT:
            return "PSA_ERROR_INVALID_ARGUMENT";
        case PSA_ERROR_INSUFFICIENT_MEMORY:
            return "PSA_ERROR_INSUFFICIENT_MEMORY";
        case PSA_ERROR_INSUFFICIENT_STORAGE:
            return "PSA_ERROR_INSUFFICIENT_STORAGE";
        case PSA_ERROR_COMMUNICATION_FAILURE:
            return "PSA_ERROR_COMMUNICATION_FAILURE";
        case PSA_ERROR_STORAGE_FAILURE:
            return "PSA_ERROR_STORAGE_FAILURE";
        case PSA_ERROR_DATA_CORRUPT:
            return "PSA_ERROR_DATA_CORRUPT";
        case PSA_ERROR_DATA_INVALID:
            return "PSA_ERROR_DATA_INVALID";
        case PSA_ERROR_HARDWARE_FAILURE:
            return "PSA_ERROR_HARDWARE_FAILURE";
        case PSA_ERROR_CORRUPTION_DETECTED:
            return "PSA_ERROR_CORRUPTION_DETECTED";
        case PSA_ERROR_INSUFFICIENT_ENTROPY:
            return "PSA_ERROR_INSUFFICIENT_ENTROPY";
        case PSA_ERROR_INVALID_SIGNATURE:
            return "PSA_ERROR_INVALID_SIGNATURE";
        case PSA_ERROR_INVALID_PADDING:
            return "PSA_ERROR_INVALID_PADDING";
        case PSA_ERROR_INSUFFICIENT_DATA:
            return "PSA_ERROR_INSUFFICIENT_DATA";
        case PSA_ERROR_INVALID_HANDLE:
            return "PSA_ERROR_INVALID_HANDLE";
        default:
            return "Error value not recognized";
    }
}

psa_status_t psa_crypto_init(void)
{
    if (lib_initialized) {
        return PSA_SUCCESS;
    }

    lib_initialized = 1;

#if (IS_USED(MODULE_PSA_KEY_SLOT_MGMT))
    psa_init_key_slots();
#endif

    return PSA_SUCCESS;
}

#if IS_USED(MODULE_PSA_AEAD)
psa_status_t psa_aead_abort(psa_aead_operation_t *operation)
{
    (void)operation;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_decrypt(  psa_key_id_t key,
                                psa_algorithm_t alg,
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
    (void)key;
    (void)alg;
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

psa_status_t psa_aead_decrypt_setup(psa_aead_operation_t *operation,
                                    psa_key_id_t key,
                                    psa_algorithm_t alg)
{
    (void)operation;
    (void)key;
    (void)alg;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_encrypt(  psa_key_id_t key,
                                psa_algorithm_t alg,
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
    (void)key;
    (void)alg;
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

psa_status_t psa_aead_encrypt_setup(psa_aead_operation_t *operation,
                                    psa_key_id_t key,
                                    psa_algorithm_t alg)
{
    (void)operation;
    (void)key;
    (void)alg;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_finish(   psa_aead_operation_t *operation,
                                uint8_t *ciphertext,
                                size_t ciphertext_size,
                                size_t *ciphertext_length,
                                uint8_t *tag,
                                size_t tag_size,
                                size_t *tag_length)
{
    (void)operation;
    (void)ciphertext;
    (void)ciphertext_size;
    (void)ciphertext_length;
    (void)tag;
    (void)tag_size;
    (void)tag_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_generate_nonce(   psa_aead_operation_t *operation,
                                        uint8_t *nonce,
                                        size_t nonce_size,
                                        size_t *nonce_length)
{
    (void)operation;
    (void)nonce;
    (void)nonce_size;
    (void)nonce_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_set_lengths(  psa_aead_operation_t *operation,
                                    size_t ad_length,
                                    size_t plaintext_length)
{
    (void)operation;
    (void)ad_length;
    (void)plaintext_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_set_nonce(psa_aead_operation_t *operation,
                                const uint8_t *nonce,
                                size_t nonce_length)
{
    (void)operation;
    (void)nonce;
    (void)nonce_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_update(   psa_aead_operation_t *operation,
                                const uint8_t *input,
                                size_t input_length,
                                uint8_t *output,
                                size_t output_size,
                                size_t *output_length)
{
    (void)operation;
    (void)input;
    (void)input_length;
    (void)output;
    (void)output_size;
    (void)output_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_update_ad(psa_aead_operation_t *operation,
                                const uint8_t *input,
                                size_t input_length)
{
    (void)operation;
    (void)input;
    (void)input_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_verify(   psa_aead_operation_t *operation,
                                uint8_t *plaintext,
                                size_t plaintext_size,
                                size_t *plaintext_length,
                                const uint8_t *tag,
                                size_t tag_length)
{
    (void)operation;
    (void)plaintext;
    (void)plaintext_size;
    (void)plaintext_length;
    (void)tag;
    (void)tag_length;
    return PSA_ERROR_NOT_SUPPORTED;
}
#endif /* MODULE_PSA_AEAD */

#if IS_USED(MODULE_PSA_ASYMMETRIC)
psa_status_t psa_asymmetric_decrypt(psa_key_id_t key,
                                    psa_algorithm_t alg,
                                    const uint8_t *input,
                                    size_t input_length,
                                    const uint8_t *salt,
                                    size_t salt_length,
                                    uint8_t *output,
                                    size_t output_size,
                                    size_t *output_length)
{
    (void)key;
    (void)alg;
    (void)input;
    (void)input_length;
    (void)salt;
    (void)salt_length;
    (void)output;
    (void)output_size;
    (void)output_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_asymmetric_encrypt(psa_key_id_t key,
                                    psa_algorithm_t alg,
                                    const uint8_t *input,
                                    size_t input_length,
                                    const uint8_t *salt,
                                    size_t salt_length,
                                    uint8_t *output,
                                    size_t output_size,
                                    size_t *output_length)
{
    (void)key;
    (void)alg;
    (void)input;
    (void)input_length;
    (void)salt;
    (void)salt_length;
    (void)output;
    (void)output_size;
    (void)output_length;
    return PSA_ERROR_NOT_SUPPORTED;
}
#endif /* MODULE_PSA_ASYMMETRIC */

#if IS_USED(MODULE_PSA_KEY_MANAGEMENT)
/**
 * @brief   Checks whether a key's policy permits the usage of a given algorithm
 *
 * @param   policy          Policy of the given key
 * @param   type            Type of the given key
 * @param   requested_alg   Algorithm to be used
 *
 * @return  @ref PSA_SUCCESS
 *          @ref PSA_ERROR_NOT_PERMITTED
 *          @ref PSA_ERROR_INVALID_ARGUMENT  If @c requested_alg is not a valid algorithm
 */
static psa_status_t psa_key_policy_permits( const psa_key_policy_t *policy,
                                            psa_key_type_t type,
                                            psa_algorithm_t requested_alg)
{
    if (requested_alg == 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (policy->alg == requested_alg) {
        return PSA_SUCCESS;
    }

    (void)type;
    return PSA_ERROR_NOT_PERMITTED;
}

/**
 * @brief   Check whether the policy of the key associated with the given ID permits the requested
 *          usage and return the key slot.
 *
 * @param   id      ID of the key to be used
 * @param   p_slot  Pointer to a @c psa_key_slot_t type to return the desired key slot.
 *                  @c NULL if something went wrong.
 * @param   usage   The requested usage of the key
 * @param   alg     The requested algorithm that uses the key
 *
 * @return  @ref PSA_SUCCESS
 *          @ref PSA_ERROR_NOT_PERMITTED
 *          @ref PSA_ERROR_DOES_NOT_EXIST
 *          @ref PSA_ERROR_INVALID_ARGUMENT
 *          @ref PSA_ERROR_NOT_SUPPORTED
 *          @ref PSA_ERROR_CORRUPTION_DETECTED
 */
static psa_status_t psa_get_and_lock_key_slot_with_policy(psa_key_id_t id,
                                                          psa_key_slot_t **p_slot,
                                                          psa_key_usage_t usage,
                                                          psa_algorithm_t alg)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;

    status = psa_get_and_lock_key_slot(id, p_slot);
    if (status != PSA_SUCCESS) {
        return status;
    }
    slot = *p_slot;

    if (PSA_KEY_TYPE_IS_PUBLIC_KEY(slot->attr.type)) {
        /* Export is always permitted for asymmetric public keys */
        usage &= ~PSA_KEY_USAGE_EXPORT;
    }

    if ((slot->attr.policy.usage & usage) != usage) {
        *p_slot = NULL;
        psa_unlock_key_slot(slot);
        return PSA_ERROR_NOT_PERMITTED;
    }

    if (alg != 0) {
        status = psa_key_policy_permits( &slot->attr.policy, slot->attr.type, alg);
        if (status != PSA_SUCCESS) {
            *p_slot = NULL;
            psa_unlock_key_slot(slot);
            return status;
        }
    }
    return PSA_SUCCESS;
}
#endif /* MODULE_PSA_KEY_MANAGEMENT */

#if IS_USED(MODULE_PSA_CIPHER)
psa_status_t psa_cipher_abort(psa_cipher_operation_t *operation)
{
    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    *operation = psa_cipher_operation_init();
    return PSA_SUCCESS;
}

/**
 * @brief   Setup a cipher encrypt or decrypt operation.
 *
 *          See @ref psa_cipher_encrypt_setup(...)
 *          See @ref psa_cipher_decrypt_setup(...)
 *
 * @param   operation
 * @param   key
 * @param   alg
 * @param   direction   Whether encrypt or decrypt, see @ref psa_encrypt_or_decrypt_t
 * @return  @ref psa_status_t
 */
static psa_status_t psa_cipher_setup(   psa_cipher_operation_t *operation,
                                        psa_key_id_t key,
                                        psa_algorithm_t alg,
                                        psa_encrypt_or_decrypt_t direction)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;
    psa_key_usage_t usage = (direction == PSA_CRYPTO_DRIVER_ENCRYPT ?
                             PSA_KEY_USAGE_ENCRYPT :
                             PSA_KEY_USAGE_DECRYPT);

    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!operation) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (!PSA_ALG_IS_CIPHER(alg)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    status = psa_get_and_lock_key_slot_with_policy(key, &slot, usage, alg);
    if (status != PSA_SUCCESS) {
        psa_cipher_abort(operation);
        unlock_status = psa_unlock_key_slot(slot);
        return status;
    }

    operation->iv_set = 0;
    if (alg == PSA_ALG_ECB_NO_PADDING) {
        operation->iv_required = 0;
    }
    else {
        operation->iv_required = 1;
    }
    operation->default_iv_length = PSA_CIPHER_IV_LENGTH(slot->attr.type, alg);

    psa_key_attributes_t attr = slot->attr;

    if (direction == PSA_CRYPTO_DRIVER_ENCRYPT) {
        status = psa_location_dispatch_cipher_encrypt_setup(operation, &attr, slot, alg);
    }
    else if (direction == PSA_CRYPTO_DRIVER_DECRYPT) {
        status = psa_location_dispatch_cipher_decrypt_setup(operation, &attr, slot, alg);
    }

    if (status != PSA_SUCCESS) {
        psa_cipher_abort(operation);
    }

    unlock_status = psa_unlock_key_slot(slot);
    return ((status == PSA_SUCCESS) ? unlock_status : status);
}

/**
 * @brief   Cipher encrypt and decrypt function
 *
 *          See @ref psa_cipher_encrypt(...)
 *          See @ref psa_cipher_decrypt(...)
 *
 * @param   key
 * @param   alg
 * @param   input
 * @param   input_length
 * @param   output
 * @param   output_size
 * @param   output_length
 * @param   direction       Whether to encrypt or decrypt, see @ref psa_encrypt_or_decrypt_t
 * @return  @ref psa_status_t
 */
static psa_status_t psa_cipher_encrypt_decrypt( psa_key_id_t key,
                                                psa_algorithm_t alg,
                                                const uint8_t *input,
                                                size_t input_length,
                                                uint8_t *output,
                                                size_t output_size,
                                                size_t *output_length,
                                                psa_encrypt_or_decrypt_t direction)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;

    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!input || !output || !output_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (!PSA_ALG_IS_CIPHER(alg)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_key_usage_t usage = (direction == PSA_CRYPTO_DRIVER_ENCRYPT ?
                             PSA_KEY_USAGE_ENCRYPT :
                             PSA_KEY_USAGE_DECRYPT);

    status = psa_get_and_lock_key_slot_with_policy(key, &slot, usage, alg);
    if (status != PSA_SUCCESS) {
        unlock_status = psa_unlock_key_slot(slot);
        if (unlock_status != PSA_SUCCESS) {
            status = unlock_status;
        }
        return status;
    }

    if (((alg == PSA_ALG_CBC_NO_PADDING) || (alg == PSA_ALG_ECB_NO_PADDING)) &&
        (input_length % PSA_BLOCK_CIPHER_BLOCK_LENGTH(slot->attr.type))) {
            unlock_status = psa_unlock_key_slot(slot);
            return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (direction == PSA_CRYPTO_DRIVER_ENCRYPT) {
        if (output_size < PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(slot->attr.type, alg, input_length)) {
            unlock_status = psa_unlock_key_slot(slot);
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }
        status = psa_location_dispatch_cipher_encrypt(&slot->attr, alg, slot, input, input_length,
                                                      output, output_size, output_length);
    }
    else {
        size_t iv_length = PSA_CIPHER_IV_LENGTH(slot->attr.type, alg);
        if (output_size < (input_length - iv_length)) {
            /* Input buffer contains iv + cipher, so output must be at least input_length - IV */
            unlock_status = psa_unlock_key_slot(slot);
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }
        status = psa_location_dispatch_cipher_decrypt(&slot->attr, alg, slot, input, input_length,
                                                      output, output_size, output_length);
    }

    unlock_status = psa_unlock_key_slot(slot);
    return ((status == PSA_SUCCESS) ? unlock_status : status);
}

psa_status_t psa_cipher_decrypt(psa_key_id_t key,
                                psa_algorithm_t alg,
                                const uint8_t *input,
                                size_t input_length,
                                uint8_t *output,
                                size_t output_size,
                                size_t *output_length)
{
    return psa_cipher_encrypt_decrypt(key, alg, input, input_length, output, output_size,
                                      output_length, PSA_CRYPTO_DRIVER_DECRYPT);
}

psa_status_t psa_cipher_decrypt_setup(psa_cipher_operation_t *operation,
                                      psa_key_id_t key,
                                      psa_algorithm_t alg)
{
    return psa_cipher_setup(operation, key, alg, PSA_CRYPTO_DRIVER_DECRYPT);
}

psa_status_t psa_cipher_encrypt(psa_key_id_t key,
                                psa_algorithm_t alg,
                                const uint8_t *input,
                                size_t input_length,
                                uint8_t *output,
                                size_t output_size,
                                size_t *output_length)
{
    return psa_cipher_encrypt_decrypt(key, alg, input, input_length, output, output_size,
                                      output_length, PSA_CRYPTO_DRIVER_ENCRYPT);
}

psa_status_t psa_cipher_encrypt_setup(psa_cipher_operation_t *operation,
                                      psa_key_id_t key,
                                      psa_algorithm_t alg)
{
    return psa_cipher_setup(operation, key, alg, PSA_CRYPTO_DRIVER_ENCRYPT);
}

psa_status_t psa_cipher_finish(psa_cipher_operation_t *operation,
                               uint8_t *output,
                               size_t output_size,
                               size_t *output_length)
{
    (void)operation;
    (void)output;
    (void)output_size;
    (void)output_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_cipher_generate_iv(psa_cipher_operation_t *operation,
                                    uint8_t *iv,
                                    size_t iv_size,
                                    size_t *iv_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!operation || !iv || !iv_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    *iv_length = 0;

    if (!operation->iv_required || operation->iv_set) {
        return PSA_ERROR_BAD_STATE;
    }

    if (iv_size < operation->default_iv_length) {
        psa_cipher_abort(operation);
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    status = psa_generate_random(iv, iv_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    operation->iv_set = 1;
    *iv_length = iv_size;

    return status;
}

psa_status_t psa_cipher_set_iv(psa_cipher_operation_t *operation,
                               const uint8_t *iv,
                               size_t iv_length)
{
    (void)operation;
    (void)iv;
    (void)iv_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_cipher_update(psa_cipher_operation_t *operation,
                               const uint8_t *input,
                               size_t input_length,
                               uint8_t *output,
                               size_t output_size,
                               size_t *output_length)
{
    (void)operation;
    (void)input;
    (void)input_length;
    (void)output;
    (void)output_size;
    (void)output_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

#endif /* MODULE_PSA_CIPHER */

#if IS_USED(MODULE_PSA_HASH)
psa_status_t psa_hash_setup(psa_hash_operation_t *operation,
                            psa_algorithm_t alg)
{
    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!operation) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (operation->alg != 0) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!PSA_ALG_IS_HASH(alg)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    psa_status_t status = psa_algorithm_dispatch_hash_setup(operation, alg);
    if (status == PSA_SUCCESS) {
        operation->alg = alg;
    }

    return status;
}

psa_status_t psa_hash_update(psa_hash_operation_t *operation,
                             const uint8_t *input,
                             size_t input_length)
{
    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!operation || !input) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (operation->alg == 0) {
        return PSA_ERROR_BAD_STATE;
    }

    psa_status_t status = psa_algorithm_dispatch_hash_update(operation, input, input_length);

    if (status != PSA_SUCCESS) {
        psa_hash_abort(operation);
    }
    return status;
}

psa_status_t psa_hash_finish(psa_hash_operation_t *operation,
                             uint8_t *hash,
                             size_t hash_size,
                             size_t *hash_length)
{
    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!operation || !hash || !hash_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (operation->alg == 0) {
        return PSA_ERROR_BAD_STATE;
    }

    uint8_t actual_hash_length = PSA_HASH_LENGTH(operation->alg);

    if (hash_size < actual_hash_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    psa_status_t status =
        psa_algorithm_dispatch_hash_finish(operation, hash, hash_size, hash_length);
    if (status == PSA_SUCCESS) {
        *hash_length = actual_hash_length;
    }

    /* Make sure operation becomes inactive after successful execution */
    psa_hash_abort(operation);
    return status;
}

psa_status_t psa_hash_verify(psa_hash_operation_t *operation,
                             const uint8_t *hash,
                             size_t hash_length)
{
    int status = PSA_ERROR_CORRUPTION_DETECTED;
    uint8_t digest[PSA_HASH_MAX_SIZE];
    size_t actual_hash_length = 0;

    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!operation || !hash) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    status = psa_hash_finish(operation, digest, PSA_HASH_MAX_SIZE, &actual_hash_length);

    if (status != PSA_SUCCESS) {
        return status;
    }

    if (actual_hash_length != hash_length) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    if (constant_time_memcmp(hash, digest, hash_length) != 0) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    return PSA_SUCCESS;
}

psa_status_t psa_hash_suspend(psa_hash_operation_t *operation,
                              uint8_t *hash_state,
                              size_t hash_state_size,
                              size_t *hash_state_length)
{
    (void)operation;
    (void)hash_state;
    (void)hash_state_size;
    (void)hash_state_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_hash_resume(psa_hash_operation_t *operation,
                             const uint8_t *hash_state,
                             size_t hash_state_length)
{
    (void)operation;
    (void)hash_state;
    (void)hash_state_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_hash_abort(psa_hash_operation_t *operation)
{
    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    *operation = psa_hash_operation_init();
    return PSA_SUCCESS;
}

psa_status_t psa_hash_clone(const psa_hash_operation_t *source_operation,
                            psa_hash_operation_t *target_operation)
{
    (void)source_operation;
    (void)target_operation;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_hash_compare(psa_algorithm_t alg,
                              const uint8_t *input,
                              size_t input_length,
                              const uint8_t *hash,
                              size_t hash_length)
{
    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!input || !hash) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    status = psa_hash_setup(&operation, alg);
    if (status != PSA_SUCCESS) {
        return status;
    }
    status = psa_hash_update(&operation, input, input_length);
    if (status != PSA_SUCCESS) {
        return status;
    }
    status = psa_hash_verify(&operation, hash, hash_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return PSA_SUCCESS;
}

psa_status_t psa_hash_compute(psa_algorithm_t alg,
                              const uint8_t *input,
                              size_t input_length,
                              uint8_t *hash,
                              size_t hash_size,
                              size_t *hash_length)
{
    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!input || !hash || !hash_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (hash_size < PSA_HASH_LENGTH(alg)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    *hash_length = hash_size;
    status = psa_hash_setup(&operation, alg);
    if (status != PSA_SUCCESS) {
        return status;
    }
    status = psa_hash_update(&operation, input, input_length);
    if (status != PSA_SUCCESS) {
        return status;
    }
    status = psa_hash_finish(&operation, hash, hash_size, hash_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return PSA_SUCCESS;
}
#endif /* MODULE_PSA_HASH */

psa_status_t psa_builtin_generate_random(uint8_t *output,
                                         size_t output_size)
{
    if (!output) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* TODO: Should point to a CSPRNG API in the future */
    random_bytes(output, output_size);
    return PSA_SUCCESS;
}

psa_status_t psa_generate_random(uint8_t *output,
                                 size_t output_size)
{
    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!output) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return psa_location_dispatch_generate_random(output, output_size);
}

/* Key Management */
#if IS_USED(MODULE_PSA_KEY_MANAGEMENT)
/**
 * @brief   Check whether the key policy is valid
 *
 * @param   policy  Policy of type @ref psa_key_policy_t of the key to be used
 *
 * @return  @ref PSA_SUCCESS
 *          @ref PSA_ERROR_INVALID_ARGUMENT
 */
static psa_status_t psa_validate_key_policy(const psa_key_policy_t *policy)
{
    if ((policy->usage & ~(PSA_KEY_USAGE_EXPORT |
                           PSA_KEY_USAGE_COPY |
                           PSA_KEY_USAGE_ENCRYPT |
                           PSA_KEY_USAGE_DECRYPT |
                           PSA_KEY_USAGE_SIGN_MESSAGE |
                           PSA_KEY_USAGE_VERIFY_MESSAGE |
                           PSA_KEY_USAGE_SIGN_HASH |
                           PSA_KEY_USAGE_VERIFY_HASH |
                           PSA_KEY_USAGE_DERIVE)) != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return PSA_SUCCESS;
}

/**
 * @brief   Check whether the size of a symmetric key is supported
 *
 * @param   type    Type of the used key as @ref psa_key_type_t
 * @param   bits    Size of the key as @c size_t
 *
 * @return  @ref PSA_SUCCESS
 *          @ref PSA_ERROR_INVALID_ARGUMENT
 *          @ref PSA_ERROR_NOT_SUPPORTED
 */
static psa_status_t psa_validate_unstructured_key_size(psa_key_type_t type, size_t bits)
{
    switch (type) {
    case PSA_KEY_TYPE_AES:
        if (bits != 128 && bits != 192 && bits != 256) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
        break;
    case PSA_KEY_TYPE_HMAC:
        if (bits % 8 != 0) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
        break;
    default:
        (void)bits;
        return PSA_ERROR_NOT_SUPPORTED;
        break;
    }
    return PSA_SUCCESS;
}

/**
 * @brief   Check whether the key size is valid for key generation
 *
 * @param   type    Type of the used key as @ref psa_key_type_t
 * @param   bits    Size of the key as @c size_t
 *
 * @return  @ref PSA_SUCCESS
 *          @ref PSA_ERROR_INVALID_ARGUMENT
 *          @ref PSA_ERROR_NOT_SUPPORTED
 */
static psa_status_t psa_validate_key_for_key_generation(psa_key_type_t type, size_t bits)
{
    if (PSA_KEY_TYPE_IS_UNSTRUCTURED(type)) {
        return psa_validate_unstructured_key_size(type, bits);
    }
#if IS_USED(MODULE_PSA_ASYMMETRIC)
    else if (PSA_KEY_TYPE_IS_ECC_KEY_PAIR(type)) {
        return PSA_ECC_KEY_SIZE_IS_VALID(type, bits) ? PSA_SUCCESS : PSA_ERROR_INVALID_ARGUMENT;
    }
#endif
    /* TODO: add validation for other key types */
    return PSA_ERROR_NOT_SUPPORTED;
}

/**
 * @brief   Check validity of key attributes and get secure element driver in case the key is
 *          stored on a secure element
 *
 * @param   attributes  Key attributes that are to be checked
 * @param   p_drv       Pointer which will contain the SE driver, if one exists
 *
 * @return  @ref PSA_SUCCESS
 *          @ref PSA_ERROR_INVALID_ARGUMENT
 *          @ref PSA_ERROR_NOT_SUPPORTED
 */
static psa_status_t psa_validate_key_attributes(const psa_key_attributes_t *attributes,
                                                psa_se_drv_data_t **p_drv)
{
    psa_status_t status = PSA_ERROR_INVALID_ARGUMENT;
    psa_key_lifetime_t lifetime = psa_get_key_lifetime(attributes);
    psa_key_id_t key = psa_get_key_id(attributes);

    status = psa_validate_key_location(lifetime, p_drv);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_validate_key_persistence(lifetime);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if (PSA_KEY_LIFETIME_IS_VOLATILE(lifetime)) {
        if (key != 0) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
    }
#if IS_USED(MODULE_PSA_PERSISTENT_STORAGE)
    else {
        if (!psa_is_valid_key_id(key, 1)) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
    }
#endif /* MODULE_PSA_PERSISTENT_STORAGE */

    status = psa_validate_key_policy(&attributes->policy);
    if (status != PSA_SUCCESS) {
        return status;
    }
    return PSA_SUCCESS;
}

/**
 * @brief   Set up a key creation process.
 *
 *          Validate key attributes for key creation and find free slots and drivers if they exists.
 *
 * @param   method      Key creation method (see @ref psa_key_creation_method_t)
 * @param   attributes  Key attributes of the key that should be created
 * @param   p_slot      Pointer which will contain a key slot to store the key in
 * @param   p_drv       Pointer to a SE driver if one exists for the given key location
 *
 * @return  @ref psa_status_t
 */
static psa_status_t psa_start_key_creation(psa_key_creation_method_t method,
                                           const psa_key_attributes_t *attributes,
                                           psa_key_slot_t **p_slot, psa_se_drv_data_t **p_drv)
{
    psa_status_t status;
    psa_key_id_t key_id;
    psa_key_slot_t *slot;

    *p_drv = NULL;

    status = psa_validate_key_attributes(attributes, p_drv);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_allocate_empty_key_slot(&key_id, attributes, p_slot);
    if (status != PSA_SUCCESS) {
        return status;
    }
    slot = *p_slot;
    slot->attr = *attributes;

    if (PSA_KEY_LIFETIME_IS_VOLATILE(slot->attr.lifetime)) {
        slot->attr.id = key_id;
    }

#if IS_USED(MODULE_PSA_SECURE_ELEMENT)
    /* Find a free slot on a secure element and store SE slot number in key_data */
    if (*p_drv != NULL) {
        psa_key_slot_number_t *slot_number = psa_key_slot_get_slot_number(slot);
        status = psa_find_free_se_slot(attributes, method, *p_drv, slot_number);
        if (status != PSA_SUCCESS) {
            return status;
        }
        /* TODO: Start transaction for persistent key storage */
    }
#endif /* CONFIG_PSA_SECURE_ELEMENT */

    (void)method;
    return PSA_SUCCESS;
}

/**
 * @brief   Finish up key creation process
 *
 * @param   slot    Pointer to slot that the key is stored in
 * @param   driver  SE driver, in case the key creation took place on a secure element
 * @param   key_id  Pointer which will contain the key ID assigned to the key
 *
 * @return  @ref psa_status_t
 */
static psa_status_t psa_finish_key_creation(psa_key_slot_t *slot, psa_se_drv_data_t *driver,
                                            psa_key_id_t *key_id)
{
    psa_status_t status = PSA_SUCCESS;

    if (PSA_KEY_LIFETIME_IS_VOLATILE(slot->attr.lifetime)) {
        *key_id = slot->attr.id;
    }
#if IS_USED(MODULE_PSA_PERSISTENT_STORAGE)
    else {
        status = psa_persist_key_slot_in_storage(slot);
    }
#endif /* MODULE_PSA_PERSISTENT_STORAGE */

    (void)driver;
    psa_status_t unlock_status = psa_unlock_key_slot(slot);
    return ((status == PSA_SUCCESS) ? unlock_status : status);
}

/**
 * @brief   Abort key creation and clean up in case of failure
 *
 * @param   slot    Slot that the key has been written to
 * @param   driver  SE driver, in case the key creation took place on a secure element
 */
static void psa_fail_key_creation(psa_key_slot_t *slot, psa_se_drv_data_t *driver)
{
    (void)driver;
    if (slot == NULL) {
        return;
    }
    /* TODO: Destroy key in secure element (see mbedtls code) */
    /* TODO: Secure Element stop transaction */
    psa_wipe_key_slot(slot);
}

psa_status_t psa_copy_key(psa_key_id_t source_key,
                          const psa_key_attributes_t *attributes,
                          psa_key_id_t *target_key)
{
    (void)source_key;
    (void)attributes;
    (void)target_key;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_destroy_key(psa_key_id_t key)
{
    psa_status_t status;
    psa_key_slot_t *slot;

    status = psa_get_and_lock_key_slot(key, &slot);
    if (status != PSA_SUCCESS) {
        return status;
    }
    if (slot->lock_count > 1) {
        return PSA_ERROR_CORRUPTION_DETECTED;
    }

#if IS_USED(MODULE_PSA_PERSISTENT_STORAGE)
    if (!PSA_KEY_LIFETIME_IS_VOLATILE(slot->attr.lifetime)) {
        status = psa_destroy_persistent_key(key);
        if (status != PSA_SUCCESS) {
            DEBUG("PSA destroy key: Persistent key destruction failed: %s\n",
                                            psa_status_to_humanly_readable(status));
            return PSA_ERROR_STORAGE_FAILURE;
        }
    }
#endif /* MODULE_PSA_PERSISTENT_STORAGE */

    return psa_wipe_key_slot(slot);
}

/**
 * @brief   Export key that is stored in local memory
 *
 *          See @ref psa_export_key
 *
 * @param   key_buffer
 * @param   key_buffer_size
 * @param   data
 * @param   data_size
 * @param   data_length
 *
 * @return  @ref PSA_SUCCESS
 *          @ref PSA_ERROR_INVALID_ARGUMENT
 */
static psa_status_t psa_builtin_export_key(const uint8_t *key_buffer,
                                                size_t key_buffer_size,
                                                uint8_t *data,
                                                size_t data_size,
                                                size_t *data_length)
{
    if (!key_buffer || !data || !data_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_buffer_size == 0 || data_size == 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (data_size < key_buffer_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    memcpy(data, key_buffer, key_buffer_size);
    *data_length = key_buffer_size;

    return PSA_SUCCESS;
}

psa_status_t psa_export_key(psa_key_id_t key,
                            uint8_t *data,
                            size_t data_size,
                            size_t *data_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;
    uint8_t *privkey_data = NULL;
    size_t *privkey_data_len = NULL;

    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!data || !data_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    *data_length = 0;

    status = psa_get_and_lock_key_slot_with_policy(key, &slot, PSA_KEY_USAGE_EXPORT, 0);
    if (status != PSA_SUCCESS) {
        unlock_status = psa_unlock_key_slot(slot);
        if (unlock_status != PSA_SUCCESS) {
            status = unlock_status;
        }
        return status;
    }

    psa_key_lifetime_t lifetime = psa_get_key_lifetime(&slot->attr);
    if (psa_key_lifetime_is_external(lifetime)) {
        /* key export from an external device is currently not supported */
        status = PSA_ERROR_NOT_SUPPORTED;
        unlock_status = psa_unlock_key_slot(slot);
        if (unlock_status != PSA_SUCCESS) {
            status = unlock_status;
        }
        return status;
    }

    if (!PSA_KEY_TYPE_IS_ECC(slot->attr.type) ||
            PSA_KEY_TYPE_ECC_GET_FAMILY(slot->attr.type) != PSA_ECC_FAMILY_TWISTED_EDWARDS) {
        /* key export is currently only supported for ed25519 keys */
        status = PSA_ERROR_NOT_SUPPORTED;
        unlock_status = psa_unlock_key_slot(slot);
        if (unlock_status != PSA_SUCCESS) {
            status = unlock_status;
        }
        return status;
    }

    psa_get_key_data_from_key_slot(slot, &privkey_data, &privkey_data_len);

    status =
        psa_builtin_export_key(privkey_data, *privkey_data_len, data, data_size, data_length);

    unlock_status = psa_unlock_key_slot(slot);
    return ((status == PSA_SUCCESS) ? unlock_status : status);
}

/**
 * @brief   Export asymmetric public key that is stored in local memory
 *
 *          See @ref psa_export_public_key
 *
 * @param   key_buffer
 * @param   key_buffer_size
 * @param   data
 * @param   data_size
 * @param   data_length
 *
 * @return  @ref PSA_SUCCESS
 *          @ref PSA_ERROR_INVALID_ARGUMENT
 */
static psa_status_t psa_builtin_export_public_key( const uint8_t *key_buffer,
                                                   size_t key_buffer_size,
                                                   uint8_t *data,
                                                   size_t data_size,
                                                   size_t *data_length)
{
    if (key_buffer_size == 0 || data_size == 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (data_size < key_buffer_size) {
        DEBUG("PSA Crypto Builtin Export Key: Output buffer too small\n");
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    memcpy(data, key_buffer, key_buffer_size);
    *data_length = key_buffer_size;

    return PSA_SUCCESS;
}

psa_status_t psa_export_public_key(psa_key_id_t key,
                                   uint8_t *data,
                                   size_t data_size,
                                   size_t *data_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;
    uint8_t *pubkey_data = NULL;
    size_t *pubkey_data_len = NULL;

    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!data || !data_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    *data_length = 0;

    status = psa_get_and_lock_key_slot_with_policy(key, &slot, 0, 0);
    if (status != PSA_SUCCESS) {
        unlock_status = psa_unlock_key_slot(slot);
        if (unlock_status != PSA_SUCCESS) {
            status = unlock_status;
        }
        return status;
    }

    if ((data_size == 0) ||
        (data_size < PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(slot->attr.type, slot->attr.bits))) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (!PSA_KEY_TYPE_IS_ECC(slot->attr.type)) {
        status = PSA_ERROR_INVALID_ARGUMENT;
        unlock_status = psa_unlock_key_slot(slot);
        return status;
    }
    psa_get_public_key_data_from_key_slot(slot, &pubkey_data, &pubkey_data_len);

    status =
        psa_builtin_export_public_key(pubkey_data, *pubkey_data_len, data, data_size, data_length);

    unlock_status = psa_unlock_key_slot(slot);
    return ((status == PSA_SUCCESS) ? unlock_status : status);
}

psa_status_t psa_builtin_generate_key(const psa_key_attributes_t *attributes, uint8_t *key_buffer,
                                      size_t key_buffer_size, size_t *key_buffer_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    if (!attributes || !key_buffer || !key_buffer_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_key_type_t type = attributes->type;

    if (PSA_KEY_TYPE_IS_UNSTRUCTURED(type)) {
        status = psa_generate_random(key_buffer, key_buffer_size);
        if (status != PSA_SUCCESS) {
            return status;
        }
        *key_buffer_length = key_buffer_size;
        return PSA_SUCCESS;
    }
    (void)key_buffer;
    (void)key_buffer_size;
    (void)key_buffer_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_generate_key(const psa_key_attributes_t *attributes,
                              psa_key_id_t *key)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot = NULL;
    psa_se_drv_data_t *driver = NULL;

    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!attributes || !key) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (psa_get_key_bits(attributes) == 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (PSA_KEY_LIFETIME_IS_VOLATILE(attributes->lifetime)) {
        *key = PSA_KEY_ID_NULL;
    }

    /* Find empty slot */
    status = psa_start_key_creation(PSA_KEY_CREATION_GENERATE, attributes, &slot, &driver);
    if (status != PSA_SUCCESS) {
        psa_fail_key_creation(slot, driver);
        return status;
    }

    if (PSA_KEY_LIFETIME_GET_LOCATION(attributes->lifetime) == PSA_KEY_LOCATION_LOCAL_STORAGE) {
        status = psa_validate_key_for_key_generation(attributes->type, attributes->bits);
        if (status != PSA_SUCCESS) {
            return status;
        }
    }

    status = psa_location_dispatch_generate_key(attributes, slot);

    if (status != PSA_SUCCESS) {
        psa_fail_key_creation(slot, driver);
        return status;
    }

    status = psa_finish_key_creation(slot, driver, key);
    if (status != PSA_SUCCESS) {
        psa_fail_key_creation(slot, driver);
    }

    return status;
}

psa_status_t psa_get_key_attributes(psa_key_id_t key,
                                    psa_key_attributes_t *attributes)
{
    psa_status_t status;
    psa_key_slot_t *slot = NULL;

    if (!attributes) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_reset_key_attributes(attributes);

    status = psa_get_and_lock_key_slot(key, &slot);
    if (status != PSA_SUCCESS) {
        return status;
    }

    *attributes = slot->attr;

    status = psa_unlock_key_slot(slot);
    return status;
}

psa_status_t psa_builtin_import_key(const psa_key_attributes_t *attributes,
                                    const uint8_t *data, size_t data_length,
                                    uint8_t *key_buffer, size_t key_buffer_size,
                                    size_t *key_buffer_length, size_t *bits)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    if (!attributes || !data || !key_buffer || !key_buffer_length || !bits) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (data_length == 0) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (data_length > key_buffer_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    psa_key_type_t type = attributes->type;

    if (PSA_KEY_TYPE_IS_UNSTRUCTURED(type)) {
        *bits = PSA_BYTES_TO_BITS(data_length);

        status = psa_validate_unstructured_key_size(type, *bits);
        if (status != PSA_SUCCESS) {
            return status;
        }

        memcpy(key_buffer, data, data_length);
        *key_buffer_length = data_length;

        return PSA_SUCCESS;
    }
    else if (PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(type)) {
        if (data_length > PSA_EXPORT_PUBLIC_KEY_MAX_SIZE) {
            return PSA_ERROR_NOT_SUPPORTED;
        }

        memcpy(key_buffer, data, data_length);
        *key_buffer_length = data_length;

        return PSA_SUCCESS;
    }
    return status;
}

psa_status_t psa_import_key(const psa_key_attributes_t *attributes,
                            const uint8_t *data,
                            size_t data_length,
                            psa_key_id_t *key)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot = NULL;
    psa_se_drv_data_t *driver = NULL;
    size_t bits;

    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!attributes || !data) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (attributes->bits != 0 && attributes->bits > PSA_BYTES_TO_BITS(data_length)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (PSA_KEY_LIFETIME_IS_VOLATILE(attributes->lifetime)) {
        *key = PSA_KEY_ID_NULL;
    }

    /* Find empty slot */
    status = psa_start_key_creation(PSA_KEY_CREATION_IMPORT, attributes, &slot, &driver);
    if (status != PSA_SUCCESS) {
        psa_fail_key_creation(slot, driver);
        return status;
    }

    bits = slot->attr.bits;

    status = psa_location_dispatch_import_key(attributes, data, data_length, slot, &bits);

    if (status != PSA_SUCCESS) {
        psa_fail_key_creation(slot, driver);
        return status;
    }

    if (slot->attr.bits == 0) {
        slot->attr.bits = (psa_key_bits_t)bits;
    }
    else if (bits != slot->attr.bits) {
        psa_fail_key_creation(slot, driver);
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    status = psa_finish_key_creation(slot, driver, key);
    if (status != PSA_SUCCESS) {
        psa_fail_key_creation(slot, driver);
    }

    return status;
}
#endif /* MODULE_PSA_KEY_MANAGEMENT */

#if IS_USED(MODULE_PSA_KEY_DERIVATION)
psa_status_t psa_key_derivation_abort(psa_key_derivation_operation_t *operation)
{
    (void)operation;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_key_derivation_get_capacity(const psa_key_derivation_operation_t *operation,
                                             size_t *capacity)
{
    (void)operation;
    (void)capacity;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_key_derivation_input_bytes(psa_key_derivation_operation_t *operation,
                                            psa_key_derivation_step_t step,
                                            const uint8_t *data,
                                            size_t data_length)
{
    (void)operation;
    (void)step;
    (void)data;
    (void)data_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_key_derivation_input_key(psa_key_derivation_operation_t *operation,
                                          psa_key_derivation_step_t step,
                                          psa_key_id_t key)
{
    (void)operation;
    (void)step;
    (void)key;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_key_derivation_key_agreement(psa_key_derivation_operation_t *operation,
                                              psa_key_derivation_step_t step,
                                              psa_key_id_t private_key,
                                              const uint8_t *peer_key,
                                              size_t peer_key_length)
{
    (void)operation;
    (void)step;
    (void)private_key;
    (void)peer_key;
    (void)peer_key_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_key_derivation_output_bytes(psa_key_derivation_operation_t *operation,
                                             uint8_t *output,
                                             size_t output_length)
{
    (void)operation;
    (void)output;
    (void)output_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_key_derivation_output_key(const psa_key_attributes_t *attributes,
                                           psa_key_derivation_operation_t *operation,
                                           psa_key_id_t *key)
{
    (void)attributes;
    (void)operation;
    (void)key;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_key_derivation_set_capacity(psa_key_derivation_operation_t *operation,
                                             size_t capacity)
{
    (void)operation;
    (void)capacity;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_key_derivation_setup(psa_key_derivation_operation_t *operation,
                                      psa_algorithm_t alg)
{
    (void)operation;
    (void)alg;
    return PSA_ERROR_NOT_SUPPORTED;
}
#endif /* MODULE_PSA_KEY_DERIVATION */

#if IS_USED(MODULE_PSA_MAC)
psa_status_t psa_mac_abort(psa_mac_operation_t *operation)
{
    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    *operation = psa_mac_operation_init();

    return PSA_ERROR_NOT_SUPPORTED;
}

/**
 * @brief   Validate algorithm and key for a MAC operation
 *
 * @param   attr        Attributes of the key that is supposed to be used
 * @param   alg         Algorithm for performing the MAC operation
 * @param   mac_size    Size of the MAC that is to be generated
 *
 * @return  @ref PSA_SUCCESS
 *          @ref PSA_ERROR_NOT_SUPPORTED
 *          @ref PSA_ERROR_INVALID_ARGUMENT
 */
static psa_status_t psa_mac_validate_alg_and_key_and_size(psa_key_attributes_t *attr,
                                                          psa_algorithm_t alg,
                                                          size_t mac_size)
{
    psa_key_type_t type = psa_get_key_type(attr);
    psa_key_bits_t bits = psa_get_key_bits(attr);

    if (!PSA_ALG_IS_HMAC(alg) || (PSA_ALG_GET_HASH(alg) != PSA_ALG_SHA_256)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    size_t operation_mac_size = PSA_MAC_LENGTH(type, bits, alg);

    if (operation_mac_size < 4) {
        /**
         * A very short MAC is too short for security since it can be
         * brute-forced. Ancient protocols with 32-bit MACs do exist,
         * so we make this our minimum, even though 32 bits is still
         * too small for security.
         */
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (operation_mac_size > PSA_MAC_MAX_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (mac_size < operation_mac_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    return PSA_SUCCESS;
}

psa_status_t psa_mac_compute(psa_key_id_t key,
                             psa_algorithm_t alg,
                             const uint8_t *input,
                             size_t input_length,
                             uint8_t *mac,
                             size_t mac_size,
                             size_t *mac_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_attributes_t attr = psa_key_attributes_init();
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;
    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!input || !mac || !mac_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    status = psa_get_key_attributes(key, &attr);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_mac_validate_alg_and_key_and_size(&attr, alg, mac_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_get_and_lock_key_slot_with_policy(key, &slot, attr.policy.usage, alg);
    if (status != PSA_SUCCESS) {
        unlock_status = psa_unlock_key_slot(slot);
        if (unlock_status != PSA_SUCCESS) {
            status = unlock_status;
        }
        return status;
    }

    status = psa_location_dispatch_mac_compute(&slot->attr, alg, slot, input, input_length, mac,
                                               mac_size, mac_length);

    unlock_status = psa_unlock_key_slot(slot);
    return ((status == PSA_SUCCESS) ? unlock_status : status);

}

psa_status_t psa_mac_sign_finish(psa_mac_operation_t *operation,
                                 uint8_t *mac,
                                 size_t mac_size,
                                 size_t *mac_length)
{
    (void)operation;
    (void)mac;
    (void)mac_size;
    (void)mac_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_mac_sign_setup(psa_mac_operation_t *operation,
                                psa_key_id_t key,
                                psa_algorithm_t alg)
{
    (void)operation;
    (void)key;
    (void)alg;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_mac_update(psa_mac_operation_t *operation,
                            const uint8_t *input,
                            size_t input_length)
{
    (void)operation;
    (void)input;
    (void)input_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_mac_verify(psa_key_id_t key,
                            psa_algorithm_t alg,
                            const uint8_t *input,
                            size_t input_length,
                            const uint8_t *mac,
                            size_t mac_length)
{
    (void)key;
    (void)alg;
    (void)input;
    (void)input_length;
    (void)mac;
    (void)mac_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_mac_verify_finish(psa_mac_operation_t *operation,
                                   const uint8_t *mac,
                                   size_t mac_length)
{
    (void)operation;
    (void)mac;
    (void)mac_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_mac_verify_setup(psa_mac_operation_t *operation,
                                  psa_key_id_t key,
                                  psa_algorithm_t alg)
{
    (void)operation;
    (void)key;
    (void)alg;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_purge_key(psa_key_id_t key)
{
    (void)key;
    return PSA_ERROR_NOT_SUPPORTED;
}
#endif /* MODULE_PSA_MAC */

#if IS_USED(MODULE_PSA_KEY_AGREEMENT)
psa_status_t psa_raw_key_agreement(psa_algorithm_t alg,
                                   psa_key_id_t private_key,
                                   const uint8_t *peer_key,
                                   size_t peer_key_length,
                                   uint8_t *output,
                                   size_t output_size,
                                   size_t *output_length)
{
    (void)alg;
    (void)private_key;
    (void)peer_key;
    (void)peer_key_length;
    (void)output;
    (void)output_size;
    (void)output_length;
    return PSA_ERROR_NOT_SUPPORTED;
}
#endif /* MODULE_PSA_KEY_AGREEMENT */

#if IS_USED(MODULE_PSA_ASYMMETRIC)
psa_status_t psa_sign_hash(psa_key_id_t key,
                           psa_algorithm_t alg,
                           const uint8_t *hash,
                           size_t hash_length,
                           uint8_t *signature,
                           size_t signature_size,
                           size_t *signature_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;

    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!hash || !signature || !signature_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (!PSA_ALG_IS_ECDSA(alg)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (!PSA_ALG_IS_SIGN_HASH(alg) || hash_length != PSA_HASH_LENGTH(alg)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    status = psa_get_and_lock_key_slot_with_policy(key, &slot, PSA_KEY_USAGE_SIGN_HASH, alg);
    if (status != PSA_SUCCESS) {
        unlock_status = psa_unlock_key_slot(slot);
        return status;
    }

    if (signature_size < PSA_ECDSA_SIGNATURE_SIZE(PSA_ECC_KEY_GET_CURVE(slot->attr.type, slot->attr.bits))) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (!PSA_KEY_TYPE_IS_KEY_PAIR(slot->attr.type)) {
        unlock_status = psa_unlock_key_slot(slot);
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_key_attributes_t attributes = slot->attr;

    status = psa_location_dispatch_sign_hash(&attributes, alg, slot, hash, hash_length, signature,
                                             signature_size, signature_length);

    unlock_status = psa_unlock_key_slot(slot);
    return ((status == PSA_SUCCESS) ? unlock_status : status);
}

psa_status_t psa_sign_message(psa_key_id_t key,
                              psa_algorithm_t alg,
                              const uint8_t *input,
                              size_t input_length,
                              uint8_t *signature,
                              size_t signature_size,
                              size_t *signature_length)
{

    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;

    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!input || !signature || !signature_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (!PSA_ALG_IS_ECDSA(alg)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (!PSA_ALG_IS_SIGN_MESSAGE(alg)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    status = psa_get_and_lock_key_slot_with_policy(key, &slot, PSA_KEY_USAGE_SIGN_MESSAGE, alg);
    if (status != PSA_SUCCESS) {
        unlock_status = psa_unlock_key_slot(slot);
        return status;
    }

    if (signature_size < PSA_ECDSA_SIGNATURE_SIZE(PSA_ECC_KEY_GET_CURVE(slot->attr.type, slot->attr.bits))) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (!PSA_KEY_TYPE_IS_KEY_PAIR(slot->attr.type)) {
        unlock_status = psa_unlock_key_slot(slot);
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_key_attributes_t attributes = slot->attr;

    status = psa_location_dispatch_sign_message(&attributes, alg, slot, input, input_length, signature,
                                             signature_size, signature_length);

    unlock_status = psa_unlock_key_slot(slot);
    return ((status == PSA_SUCCESS) ? unlock_status : status);
}

psa_status_t psa_verify_hash(psa_key_id_t key,
                             psa_algorithm_t alg,
                             const uint8_t *hash,
                             size_t hash_length,
                             const uint8_t *signature,
                             size_t signature_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;

    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!hash || !signature) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (!PSA_ALG_IS_ECDSA(alg)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (!PSA_ALG_IS_SIGN_HASH(alg) || hash_length != PSA_HASH_LENGTH(alg)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    status = psa_get_and_lock_key_slot_with_policy(key, &slot, PSA_KEY_USAGE_VERIFY_HASH, alg);
    if (status != PSA_SUCCESS) {
        unlock_status = psa_unlock_key_slot(slot);
        return status;
    }

    if (signature_length != PSA_ECDSA_SIGNATURE_SIZE(PSA_ECC_KEY_GET_CURVE(slot->attr.type, slot->attr.bits))) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /**
     * When key location is a secure element, this implementation only supports
     * the use of public keys stored on the secure element, not key pairs in
     * which the public key is stored locally.
     */
    if ((PSA_KEY_LIFETIME_GET_LOCATION(slot->attr.lifetime) != PSA_KEY_LOCATION_LOCAL_STORAGE) &&
        PSA_KEY_TYPE_IS_ECC_KEY_PAIR(slot->attr.type)) {
        unlock_status = psa_unlock_key_slot(slot);
        return PSA_ERROR_NOT_SUPPORTED;
    }

    psa_key_attributes_t attributes = slot->attr;

    status = psa_location_dispatch_verify_hash(&attributes, alg, slot, hash, hash_length, signature,
                                               signature_length);

    unlock_status = psa_unlock_key_slot(slot);
    return ((status == PSA_SUCCESS) ? unlock_status : status);
}

psa_status_t psa_verify_message(psa_key_id_t key,
                                psa_algorithm_t alg,
                                const uint8_t *input,
                                size_t input_length,
                                const uint8_t *signature,
                                size_t signature_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;

    if (!lib_initialized) {
        return PSA_ERROR_BAD_STATE;
    }

    if (!input || !signature) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (!PSA_ALG_IS_ECDSA(alg)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (!PSA_ALG_IS_SIGN_MESSAGE(alg)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    status = psa_get_and_lock_key_slot_with_policy(key, &slot, PSA_KEY_USAGE_VERIFY_MESSAGE, alg);
    if (status != PSA_SUCCESS) {
        unlock_status = psa_unlock_key_slot(slot);
        return status;
    }

    if (signature_length != PSA_ECDSA_SIGNATURE_SIZE(PSA_ECC_KEY_GET_CURVE(slot->attr.type, slot->attr.bits))) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /**
     * When key location is a secure element, this implementation only supports
     * the use of public keys stored on the secure element, not key pairs in
     * which the public key is stored locally.
     */
    if ((PSA_KEY_LIFETIME_GET_LOCATION(slot->attr.lifetime) != PSA_KEY_LOCATION_LOCAL_STORAGE) &&
        PSA_KEY_TYPE_IS_ECC_KEY_PAIR(slot->attr.type)) {
        unlock_status = psa_unlock_key_slot(slot);
        return PSA_ERROR_NOT_SUPPORTED;
    }

    psa_key_attributes_t attributes = slot->attr;

    status = psa_location_dispatch_verify_message(&attributes, alg, slot, input, input_length, signature,
                                               signature_length);

    unlock_status = psa_unlock_key_slot(slot);
    return ((status == PSA_SUCCESS) ? unlock_status : status);
}
#endif /* MODULE_PSA_ASYMMETRIC */
