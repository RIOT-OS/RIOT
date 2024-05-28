/*
 * Copyright (C) 2024 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @defgroup    sys_psa_crypto_hmac PSA Generic HMAC
 * @{
 *
 * @file        psa_generic_hmac.c
 * @brief       Generic PSA Crypto HMAC implementation, based on RFC 2104.
 *
 * @author      Armin Wolf <armin.wolf@mailbox.tu-dresden.de>
 *
 * @}
 */

#include "kernel_defines.h"
#include "psa/crypto.h"
#include "psa/crypto_contexts.h"
#include "psa_generic_hmac.h"
#include "string_utils.h"

#if IS_USED(MODULE_PSA_RIOT_MAC_HMAC_GENERIC)
static psa_status_t psa_generic_hmac_setup_key(psa_mac_operation_t *operation,
                                               const uint8_t *key_buffer,
                                               size_t key_buffer_size)
{
    psa_algorithm_t hash_alg = PSA_ALG_GET_HASH(operation->alg);
    size_t block_length = PSA_HASH_BLOCK_LENGTH(hash_alg);
    size_t dummy;

    if (key_buffer_size > block_length) {
        return psa_hash_compute(hash_alg, key_buffer, key_buffer_size, operation->block,
                                block_length, &dummy);
    }

    memcpy(operation->block, key_buffer, key_buffer_size);

    return PSA_SUCCESS;
}

static psa_status_t psa_generic_hmac_finish(psa_mac_operation_t *operation,
                                            uint8_t *buffer,
                                            size_t buffer_size)
{
    psa_algorithm_t hash_alg = PSA_ALG_GET_HASH(operation->alg);
    size_t block_length = PSA_HASH_BLOCK_LENGTH(hash_alg);
    psa_status_t status;
    size_t hash_length;

    /* See step 4 in RFC 2104 */
    status = psa_hash_finish(&operation->hash, buffer, buffer_size, &hash_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* See step 5 in RFC 2104 */
    for (size_t i = 0; i < block_length; i++) {
        operation->block[i] = operation->block[i] ^ 0x5c;
    }

    /* See step 6 and 7 in RFC 2104 */
    status = psa_hash_setup(&operation->hash, hash_alg);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_hash_update(&operation->hash, operation->block, block_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_hash_update(&operation->hash, buffer, hash_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Clear key data */
    explicit_bzero(&operation->block, block_length);

    return PSA_SUCCESS;
}

psa_status_t psa_generic_hmac_compute(const uint8_t *key_data,
                                      size_t key_length,
                                      psa_algorithm_t alg,
                                      const uint8_t *input,
                                      size_t input_length,
                                      uint8_t *mac,
                                      size_t mac_size,
                                      size_t *mac_length)
{
    psa_mac_operation_t operation = psa_mac_operation_init();
    psa_status_t status;

    status = psa_generic_hmac_setup(&operation, key_data, key_length, alg);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_generic_hmac_update(&operation, input, input_length);
    if (status != PSA_SUCCESS) {
        psa_generic_hmac_abort(&operation);
        return status;
    }

    status = psa_generic_hmac_sign_finish(&operation, mac, mac_size, mac_length);
    if (status != PSA_SUCCESS) {
        psa_generic_hmac_abort(&operation);
    }

    return status;
}

psa_status_t psa_generic_hmac_verify(const uint8_t *key_data,
                                     size_t key_length,
                                     psa_algorithm_t alg,
                                     const uint8_t *input,
                                     size_t input_length,
                                     const uint8_t *mac,
                                     size_t mac_length)
{
    psa_mac_operation_t operation = psa_mac_operation_init();
    psa_status_t status;

    status = psa_generic_hmac_setup(&operation, key_data, key_length, alg);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_generic_hmac_update(&operation, input, input_length);
    if (status != PSA_SUCCESS) {
        psa_generic_hmac_abort(&operation);
        return status;
    }

    status = psa_generic_hmac_verify_finish(&operation, mac, mac_length);
    if (status != PSA_SUCCESS) {
        psa_generic_hmac_abort(&operation);
    }

    return status;
}

psa_status_t psa_generic_hmac_setup(psa_mac_operation_t *operation,
                                    const uint8_t *key_buffer,
                                    size_t key_buffer_size,
                                    psa_algorithm_t alg)
{
    psa_algorithm_t hash_alg = PSA_ALG_GET_HASH(alg);
    psa_status_t status;
    size_t block_length;

    block_length = PSA_HASH_BLOCK_LENGTH(hash_alg);
    if (!block_length) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    operation->alg = alg;
    /* See step 1 in RFC 2104 */
    status = psa_generic_hmac_setup_key(operation, key_buffer, key_buffer_size);
    if (status != PSA_SUCCESS) {
        /* Clear key data */
        explicit_bzero(&operation->block, block_length);
        return status;
    }

    status = psa_hash_setup(&operation->hash, hash_alg);
    if (status != PSA_SUCCESS) {
        /* Clear key data */
        explicit_bzero(&operation->block, block_length);
        return status;
    }

    /* See step 2 in RFC 2104 */
    for (size_t i = 0; i < block_length; i++) {
        operation->block[i] = operation->block[i] ^ 0x36;
    }

    /* See step 3 and 4 in RFC 2104 */
    status = psa_hash_update(&operation->hash, operation->block, block_length);
    if (status != PSA_SUCCESS) {
        psa_hash_abort(&operation->hash);
        /* Clear key data */
        explicit_bzero(&operation->block, block_length);
        return status;
    }

    /* Undo the previous XOR operation to reuse the key later */
    for (size_t i = 0; i < block_length; i++) {
        operation->block[i] = operation->block[i] ^ 0x36;
    }

    return PSA_SUCCESS;
}

psa_status_t psa_generic_hmac_update(psa_mac_operation_t *operation,
                                     const uint8_t *input,
                                     size_t input_length)
{
    return psa_hash_update(&operation->hash, input, input_length);
}

psa_status_t psa_generic_hmac_sign_finish(psa_mac_operation_t *operation,
                                          uint8_t *mac,
                                          size_t mac_size,
                                          size_t *mac_length)
{
    psa_status_t status;

    status = psa_generic_hmac_finish(operation, mac, mac_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_hash_finish(&operation->hash, mac, mac_size, mac_length);
}

psa_status_t psa_generic_hmac_verify_finish(psa_mac_operation_t *operation,
                                            const uint8_t *mac,
                                            size_t mac_length)
{
    uint8_t result[PSA_MAC_MAX_SIZE];
    psa_status_t status;

    status = psa_generic_hmac_finish(operation, result, sizeof(result));
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_hash_verify(&operation->hash, mac, mac_length);
}

psa_status_t psa_generic_hmac_abort(psa_mac_operation_t *operation)
{
    psa_algorithm_t hash_alg = PSA_ALG_GET_HASH(operation->alg);
    size_t block_length = PSA_HASH_BLOCK_LENGTH(hash_alg);
    psa_status_t status;

    /* Clear key data */
    explicit_bzero(&operation->block, block_length);
    status = psa_hash_abort(&operation->hash);
    if (status != PSA_SUCCESS) {
        return status;
    }

    *operation = psa_mac_operation_init();

    return PSA_SUCCESS;
}

#endif /* MODULE_PSA_RIOT_MAC_HMAC_GENERIC */
