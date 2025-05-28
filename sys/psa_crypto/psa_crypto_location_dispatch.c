/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto sys_psa_crypto_loc_disp
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
#include "psa_crypto_algorithm_dispatch.h"
#include "psa_crypto_se_management.h"
#include "psa_crypto_se_driver.h"

#if IS_USED(MODULE_PSA_KEY_MANAGEMENT)
#include "psa_crypto_slot_management.h"

psa_status_t psa_location_dispatch_generate_key(const psa_key_attributes_t *attributes,
                                                psa_key_slot_t *slot)
{
#if IS_USED(MODULE_PSA_SECURE_ELEMENT)
    psa_status_t status;
    const psa_drv_se_t *drv;
    psa_drv_se_context_t *drv_context;
    psa_key_slot_number_t *slot_number = psa_key_slot_get_slot_number(slot);

    uint8_t *pubkey_data = NULL;
    size_t *pubkey_data_len = NULL;

    psa_get_public_key_data_from_key_slot(slot, &pubkey_data, &pubkey_data_len);

    if (psa_get_se_driver(attributes->lifetime, &drv, &drv_context)) {
        if (drv->key_management == NULL || drv->key_management->p_generate == NULL) {
            return PSA_ERROR_NOT_SUPPORTED;
        }

        status = drv->key_management->p_generate(drv_context, *slot_number, attributes, pubkey_data,
                                               *pubkey_data_len, pubkey_data_len);
        if (status != PSA_SUCCESS) {
            /* In case anything goes wrong, free the key slot for reuse. */
            psa_se_drv_data_t *driver = psa_get_se_driver_data(attributes->lifetime);
            psa_status_t abort_status =
              drv->key_management->p_destroy(drv_context,
                                             driver->ctx.internal.persistent_data,
                                             *slot_number);
            return abort_status == PSA_SUCCESS ? status : abort_status;
        }
        return PSA_SUCCESS;
    }
#endif /* MODULE_PSA_SECURE_ELEMENT */

    return psa_algorithm_dispatch_generate_key(attributes, slot);
}

psa_status_t psa_location_dispatch_import_key( const psa_key_attributes_t *attributes,
                                               const uint8_t *data, size_t data_length,
                                               psa_key_slot_t *slot, size_t *bits)
{
    psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(attributes->lifetime);

#if IS_USED(MODULE_PSA_SECURE_ELEMENT)
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    const psa_drv_se_t *drv;
    psa_drv_se_context_t *drv_context;
    psa_key_slot_number_t *slot_number = psa_key_slot_get_slot_number(slot);

    if (psa_get_se_driver(attributes->lifetime, &drv, &drv_context)) {
        if (drv->key_management == NULL || drv->key_management->p_import == NULL) {
            return PSA_ERROR_NOT_SUPPORTED;
        }
        *bits = 0;

        status = drv->key_management->p_import(drv_context, *slot_number,
                                               attributes, data,
                                               data_length, bits);
        if (status != PSA_SUCCESS) {
            /* In case anything goes wrong, free the key slot for reuse. */
            psa_se_drv_data_t *driver = psa_get_se_driver_data(attributes->lifetime);
            psa_status_t abort_status =
              drv->key_management->p_destroy(drv_context,
                                             driver->ctx.internal.persistent_data,
                                             *slot_number);
            return abort_status == PSA_SUCCESS ? status : abort_status;
        }
        return PSA_SUCCESS;
    }
#endif /* MODULE_PSA_SECURE_ELEMENT */

    switch (location) {
    case PSA_KEY_LOCATION_LOCAL_STORAGE:
        return psa_algorithm_dispatch_import_key(attributes, data, data_length, slot, bits);
    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }
}
#endif /* MODULE_PSA_KEY_MANAGEMENT */

#if IS_USED(MODULE_PSA_CIPHER)
psa_status_t psa_location_dispatch_cipher_encrypt_setup(   psa_cipher_operation_t *operation,
                                                           const psa_key_attributes_t *attributes,
                                                           const psa_key_slot_t *slot,
                                                           psa_algorithm_t alg)
{
#if IS_USED(MODULE_PSA_SECURE_ELEMENT)
    psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(attributes->lifetime);
    if (location != PSA_KEY_LOCATION_LOCAL_STORAGE) {
        const psa_drv_se_t *drv;
        psa_drv_se_context_t *drv_context;
        psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
        psa_key_slot_number_t *slot_number = psa_key_slot_get_slot_number(slot);

        uint8_t *key_data = NULL;
        size_t *key_bytes = NULL;
        psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

        if (psa_get_se_driver(attributes->lifetime, &drv, &drv_context)) {
            if (drv->cipher == NULL || drv->cipher->p_setup == NULL) {
                return PSA_ERROR_NOT_SUPPORTED;
            }

            status = drv->cipher->p_setup(drv_context,
                                          &operation->backend_ctx.se_ctx, *slot_number,
                                          attributes->policy.alg, PSA_CRYPTO_DRIVER_ENCRYPT);
            if (status != PSA_SUCCESS) {
                return status;
            }
            return PSA_SUCCESS;
        }
    }
#endif /* MODULE_PSA_SECURE_ELEMENT */
    (void)operation;
    (void)attributes;
    (void)slot;
    (void)alg;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_location_dispatch_cipher_decrypt_setup(psa_cipher_operation_t *operation,
                                                        const psa_key_attributes_t *attributes,
                                                        const psa_key_slot_t *slot,
                                                        psa_algorithm_t alg)
{
    (void)operation;
    (void)attributes;
    (void)slot;
    (void)alg;
    return PSA_ERROR_NOT_SUPPORTED;
}

#if IS_USED(MODULE_PSA_SECURE_ELEMENT)
/**
 * @brief   Single part function for cipher encryption and decryption on a secure element
 *
 *          Some secure elements don't provide single part operations for cipher encryption.
 *          This is a wrapper function, to support those.
 */
static psa_status_t psa_se_cipher_encrypt_decrypt(  const psa_drv_se_t *drv,
                                                    psa_drv_se_context_t *drv_context,
                                                    const psa_key_attributes_t *attributes,
                                                    psa_algorithm_t alg,
                                                    psa_encrypt_or_decrypt_t direction,
                                                    psa_key_slot_number_t slot,
                                                    const uint8_t *input,
                                                    size_t input_length,
                                                    uint8_t *output,
                                                    size_t output_size,
                                                    size_t *output_length)
{
    psa_status_t status;
    psa_cipher_operation_t operation = psa_cipher_operation_init();
    psa_se_cipher_context_t *se_ctx = &operation.backend_ctx.se_ctx;
    size_t input_offset = 0;
    size_t output_offset = 0;

    *output_length = 0;

    if (drv->cipher == NULL ||
        drv->cipher->p_setup == NULL ||
        drv->cipher->p_set_iv == NULL ||
        drv->cipher->p_update == NULL ||
        drv->cipher->p_finish == NULL) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    status = drv->cipher->p_setup(drv_context, se_ctx, slot, alg, direction);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if (alg == PSA_ALG_CBC_NO_PADDING) {
        operation.iv_required = 1;
        operation.default_iv_length = PSA_CIPHER_IV_LENGTH(psa_get_key_type(attributes), alg);

        if (direction == PSA_CRYPTO_DRIVER_ENCRYPT) {
            /* In case of encryption, we need to generate and set an IV. The IV will be written
            into the first 16 bytes of the output buffer. */
            size_t iv_length = 0;
            status = psa_cipher_generate_iv(&operation, output, operation.default_iv_length,
                                            &iv_length);

            status = drv->cipher->p_set_iv(se_ctx, output, iv_length);
            if (status != PSA_SUCCESS) {
                return status;
            }
            /* Increase output buffer offset to IV length to write ciphertext to buffer after IV */
            output_offset += iv_length;
            *output_length += iv_length;
        }
        else {
            /* In case of decryption the IV to be used must be provided by the caller and is
            contained in the first 16 Bytes of the input buffer.  */
            status = drv->cipher->p_set_iv(se_ctx, input, operation.default_iv_length);

            /* Increase input buffer offset to IV length to start decryption
               with actual cipher text */
            input_offset += operation.default_iv_length;
        }
    }

    status = drv->cipher->p_update(se_ctx, input + input_offset, input_length - input_offset,
                                   output + output_offset, output_size - output_offset,
                                   output_length);
    if (status != PSA_SUCCESS) {
        psa_cipher_abort(&operation);
        return status;
    }

    status = drv->cipher->p_finish(se_ctx, output, output_size, output_length);
    if (status != PSA_SUCCESS) {
        return status;
    }
    return PSA_SUCCESS;
}
#endif /* CONFIG_PSA_SECURE_ELEMENT */

psa_status_t psa_location_dispatch_cipher_encrypt(  const psa_key_attributes_t *attributes,
                                                    psa_algorithm_t alg,
                                                    const psa_key_slot_t *slot,
                                                    const uint8_t *input,
                                                    size_t input_length,
                                                    uint8_t *output,
                                                    size_t output_size,
                                                    size_t *output_length)
{

#if IS_USED(MODULE_PSA_SECURE_ELEMENT)
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    const psa_drv_se_t *drv;
    psa_drv_se_context_t *drv_context;
    psa_key_slot_number_t *slot_number = psa_key_slot_get_slot_number(slot);

    if (psa_get_se_driver(attributes->lifetime, &drv, &drv_context)) {
        if (alg == PSA_ALG_ECB_NO_PADDING) {
            if (drv->cipher == NULL || drv->cipher->p_ecb == NULL) {
                return PSA_ERROR_NOT_SUPPORTED;
            }
            status = drv->cipher->p_ecb(drv_context, *slot_number, alg, PSA_CRYPTO_DRIVER_ENCRYPT,
                                        input, input_length, output, output_size);
            if (status != PSA_SUCCESS) {
                return status;
            }
        }

        /* The SE interface does not support single part functions for other algorithms than ECB,
           so we need to build one ourselves */
        status = psa_se_cipher_encrypt_decrypt(drv, drv_context, attributes, alg,
                                               PSA_CRYPTO_DRIVER_ENCRYPT, *slot_number, input,
                                               input_length, output, output_size, output_length);

        return status;
    }

#endif /* CONFIG_PSA_SECURE_ELEMENT */
    return psa_algorithm_dispatch_cipher_encrypt(attributes, alg, slot, input, input_length, output,
                                                 output_size, output_length);
}

psa_status_t psa_location_dispatch_cipher_decrypt(  const psa_key_attributes_t *attributes,
                                                    psa_algorithm_t alg,
                                                    const psa_key_slot_t *slot,
                                                    const uint8_t *input,
                                                    size_t input_length,
                                                    uint8_t *output,
                                                    size_t output_size,
                                                    size_t *output_length)
{

#if IS_USED(MODULE_PSA_SECURE_ELEMENT)
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    const psa_drv_se_t *drv;
    psa_drv_se_context_t *drv_context;
    psa_key_slot_number_t *slot_number = psa_key_slot_get_slot_number(slot);

    if (psa_get_se_driver(attributes->lifetime, &drv, &drv_context)) {
        if (alg == PSA_ALG_ECB_NO_PADDING) {
            if (drv->cipher == NULL || drv->cipher->p_ecb == NULL) {
                return PSA_ERROR_NOT_SUPPORTED;
            }
            status = drv->cipher->p_ecb(drv_context, *slot_number, alg, PSA_CRYPTO_DRIVER_DECRYPT,
                                        input, input_length, output, output_size);
            if (status != PSA_SUCCESS) {
                return status;
            }
        }

        status = psa_se_cipher_encrypt_decrypt(drv, drv_context, attributes, alg,
                                               PSA_CRYPTO_DRIVER_DECRYPT, *slot_number, input,
                                               input_length, output, output_size, output_length);

        return status;
    }
#endif /* CONFIG_PSA_SECURE_ELEMENT */
    return psa_algorithm_dispatch_cipher_decrypt(attributes, alg, slot, input, input_length,
                                                 output, output_size, output_length);
}

#endif /* MODULE_PSA_CIPHER */

#if IS_USED(MODULE_PSA_AEAD)
psa_status_t psa_location_dispatch_aead_encrypt(const psa_key_attributes_t *attributes,
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

    /* TODO: implement MODULE_PSA_SECURE_ELEMENT support */

    return psa_algorithm_dispatch_aead_encrypt(attributes, alg, slot, nonce,
                                                nonce_length, additional_data,
                                                additional_data_length, plaintext,
                                                plaintext_length, ciphertext,
                                                ciphertext_size, ciphertext_length);
}

psa_status_t psa_location_dispatch_aead_decrypt(const psa_key_attributes_t *attributes,
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
    /* TODO: implement MODULE_PSA_SECURE_ELEMENT support */

    return psa_algorithm_dispatch_aead_decrypt( attributes, alg, slot, nonce, nonce_length,
                                                additional_data, additional_data_length,
                                                ciphertext, ciphertext_length, plaintext,
                                                plaintext_size, plaintext_length);
}
#endif /* MODULE_PSA_AEAD */

#if IS_USED(MODULE_PSA_ASYMMETRIC)
psa_status_t psa_location_dispatch_sign_hash(  const psa_key_attributes_t *attributes,
                                               psa_algorithm_t alg,
                                               const psa_key_slot_t *slot,
                                               const uint8_t *hash,
                                               size_t hash_length,
                                               uint8_t *signature,
                                               size_t signature_size,
                                               size_t *signature_length)
{
#if IS_USED(MODULE_PSA_SECURE_ELEMENT)
    const psa_drv_se_t *drv;
    psa_drv_se_context_t *drv_context;
    psa_key_slot_number_t *slot_number = psa_key_slot_get_slot_number(slot);

    uint8_t *key_data = NULL;
    size_t *key_bytes = NULL;
    psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

    if (psa_get_se_driver(attributes->lifetime, &drv, &drv_context)) {
        if (drv->asymmetric == NULL || drv->asymmetric->p_sign == NULL) {
            return PSA_ERROR_NOT_SUPPORTED;
        }
        return drv->asymmetric->p_sign(drv_context, *slot_number, alg, hash, hash_length, signature,
                                       signature_size, signature_length);
    }

    (void)key_bytes;
#endif /* CONFIG_PSA_SECURE_ELEMENT */

    return psa_algorithm_dispatch_sign_hash(attributes, alg, slot, hash, hash_length, signature,
                                            signature_size, signature_length);
}

psa_status_t psa_location_dispatch_sign_message(const psa_key_attributes_t *attributes,
                                               psa_algorithm_t alg,
                                               const psa_key_slot_t *slot,
                                               const uint8_t *input,
                                               size_t input_length,
                                               uint8_t *signature,
                                               size_t signature_size,
                                               size_t *signature_length)
{
    /* TODO: implement MODULE_PSA_SECURE_ELEMENT support */

    return psa_algorithm_dispatch_sign_message(attributes, alg, slot, input,
                                               input_length, signature,
                                               signature_size, signature_length);
}

psa_status_t psa_location_dispatch_verify_hash(const psa_key_attributes_t *attributes,
                                               psa_algorithm_t alg,
                                               const psa_key_slot_t *slot,
                                               const uint8_t *hash,
                                               size_t hash_length,
                                               const uint8_t *signature,
                                               size_t signature_length)
{
#if IS_USED(MODULE_PSA_SECURE_ELEMENT)
    const psa_drv_se_t *drv;
    psa_drv_se_context_t *drv_context;
    psa_key_slot_number_t *slot_number = psa_key_slot_get_slot_number(slot);

    uint8_t *key_data = NULL;
    size_t *key_bytes = NULL;
    psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

    if (psa_get_se_driver(attributes->lifetime, &drv, &drv_context)) {
        if (drv->asymmetric == NULL || drv->asymmetric->p_verify == NULL) {
            return PSA_ERROR_NOT_SUPPORTED;
        }
        return drv->asymmetric->p_verify(drv_context, *slot_number, alg, hash, hash_length,
                                         signature, signature_length);
    }

    (void)key_bytes;
#endif /* CONFIG_PSA_SECURE_ELEMENT */

    return psa_algorithm_dispatch_verify_hash(attributes, alg, slot, hash, hash_length, signature,
                                              signature_length);
}

psa_status_t psa_location_dispatch_verify_message(  const psa_key_attributes_t *attributes,
                                                    psa_algorithm_t alg,
                                                    const psa_key_slot_t *slot,
                                                    const uint8_t *input,
                                                    size_t input_length,
                                                    const uint8_t *signature,
                                                    size_t signature_length)
{
    /* TODO: implement MODULE_PSA_SECURE_ELEMENT support */

    return psa_algorithm_dispatch_verify_message(attributes, alg, slot, input, input_length,
                                                 signature, signature_length);
}
#endif /* MODULE_PSA_ASYMMETRIC */

#if IS_USED(MODULE_PSA_MAC)
psa_status_t psa_location_dispatch_mac_compute(const psa_key_attributes_t *attributes,
                                               psa_algorithm_t alg,
                                               const psa_key_slot_t *slot,
                                               const uint8_t *input,
                                               size_t input_length,
                                               uint8_t *mac,
                                               size_t mac_size,
                                               size_t *mac_length)
{
#if IS_USED(MODULE_PSA_SECURE_ELEMENT)
    const psa_drv_se_t *drv;
    psa_drv_se_context_t *drv_context;
    psa_key_slot_number_t *slot_number = psa_key_slot_get_slot_number(slot);

    uint8_t *key_data = NULL;
    size_t *key_bytes = NULL;
    psa_get_key_data_from_key_slot(slot, &key_data, &key_bytes);

    if (psa_get_se_driver(attributes->lifetime, &drv, &drv_context)) {
        if (drv->mac == NULL || drv->mac->p_mac == NULL) {
            return PSA_ERROR_NOT_SUPPORTED;
        }

        return drv->mac->p_mac(drv_context, input, input_length, *slot_number, alg, mac, mac_size,
                               mac_length);
    }

    (void)key_bytes;
#endif /* CONFIG_PSA_SECURE_ELEMENT */

    return psa_algorithm_dispatch_mac_compute(attributes, alg, slot, input, input_length, mac,
                                              mac_size, mac_length);
}
#endif /* MODULE_PSA_MAC */

psa_status_t psa_location_dispatch_generate_random(uint8_t *output,
                                                   size_t output_size)
{
    return psa_builtin_generate_random(output, output_size);
}
