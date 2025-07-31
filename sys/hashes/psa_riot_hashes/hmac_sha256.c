/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @brief       Glue code translating between PSA Crypto and the RIOT Hash module
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "psa/crypto.h"
#include "hashes/psa/riot_hashes.h"

psa_status_t psa_mac_compute_hmac_sha256(const psa_key_attributes_t *attributes,
                                         const uint8_t *key_buffer,
                                         size_t key_buffer_size,
                                         const uint8_t *input,
                                         size_t input_length,
                                         uint8_t *mac,
                                         size_t mac_size,
                                         size_t *mac_length)
{
    hmac_sha256(key_buffer, key_buffer_size, input, input_length, mac);
    *mac_length = 32;

    (void)mac_size;
    (void)attributes;
    return PSA_SUCCESS;
}

psa_status_t psa_mac_verify_hmac_sha256(const psa_key_attributes_t *attributes,
                                        const uint8_t *key_buffer,
                                        size_t key_buffer_size,
                                        const uint8_t *input,
                                        size_t input_length,
                                        const uint8_t *mac,
                                        size_t mac_length)
{
    (void)attributes;
    (void)key_buffer;
    (void)key_buffer_size;
    (void)input;
    (void)input_length;
    (void)mac;
    (void)mac_length;

    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_mac_sign_setup_hmac_sha256(psa_mac_operation_t *operation,
                                            const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size)
{
    (void)operation;
    (void)attributes;
    (void)key_buffer;
    (void)key_buffer_size;

    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_mac_verify_setup_hmac_sha256(psa_mac_operation_t *operation,
                                              const psa_key_attributes_t *attributes,
                                              const uint8_t *key_buffer,
                                              size_t key_buffer_size)
{
    (void)operation;
    (void)attributes;
    (void)key_buffer;
    (void)key_buffer_size;

    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_mac_update_hmac_sha256(psa_mac_operation_t *operation,
                                        const uint8_t *input,
                                        size_t input_length)
{
    (void)operation;
    (void)input;
    (void)input_length;

    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_mac_sign_finish_hmac_sha256(psa_mac_operation_t *operation,
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

psa_status_t psa_mac_verify_finish_hmac_sha256(psa_mac_operation_t *operation,
                                               const uint8_t *mac,
                                               size_t mac_length)
{
    (void)operation;
    (void)mac;
    (void)mac_length;

    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_mac_abort_hmac_sha256(psa_mac_operation_t *operation)
{
    (void)operation;

    return PSA_ERROR_NOT_SUPPORTED;
}
