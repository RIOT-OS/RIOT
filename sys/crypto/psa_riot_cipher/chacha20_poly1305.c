/*
 * Copyright (C) 2024 TU Dresden
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
 * @brief       Glue code translating between PSA Crypto and the RIOT ChaCha20-Poly1305 Cipher module
 *
 * @author      Lennard Melling <lennard.melling@msx.tu-dresden.de>
 *
 * @}
 */

#include "psa/crypto.h"
#include "crypto/chacha20poly1305.h"

#define ENABLE_DEBUG    0
#include "debug.h"

psa_status_t psa_cipher_chacha20_poly1305_encrypt(const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *output,
                                            size_t output_size,
                                            size_t *output_length)
{
    DEBUG("RIOT CHACHA20-Poly1305 Cipher");
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_cipher_operation_t operation = psa_cipher_operation_init();
    size_t iv_length = 0;
    size_t required_output_buf_size = input_length + CHACHA20POLY1305_TAG_BYTES;

    if (output_size < required_output_buf_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (key_buffer_size != CHACHA20POLY1305_KEY_BYTES) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    operation.iv_required = 1;
    operation.default_iv_length = CHACHA20POLY1305_NONCE_BYTES;
    *output_length = 0;
    uint8_t nonce[CHACHA20POLY1305_NONCE_BYTES];
    status = psa_cipher_generate_iv(&operation, nonce, operation.default_iv_length, &iv_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    chacha20poly1305_encrypt(output, input, input_length, NULL, 0, key_buffer, nonce);

    return PSA_SUCCESS;
}

psa_status_t psa_cipher_chacha20_poly1305_decrypt(const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            psa_algorithm_t alg,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *output,
                                            size_t output_size,
                                            size_t *output_length)
{
    DEBUG("RIOT CHACHA20-Poly1305 Cipher");
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_cipher_operation_t operation = psa_cipher_operation_init();
    size_t required_output_buf_size = input_length - CHACHA20POLY1305_TAG_BYTES;

    if (output_size < required_output_buf_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (key_buffer_size != CHACHA20POLY1305_KEY_BYTES) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    operation.iv_required = 1;
    operation.default_iv_length = CHACHA20POLY1305_NONCE_BYTES;
    *output_length = 0;
    uint8_t nonce[CHACHA20POLY1305_NONCE_BYTES];
    status = psa_cipher_generate_iv(&operation, nonce, operation.default_iv_length, &iv_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    chacha20poly1305_decrypt(input, input_length, output, output_length, NULL, 0, key_buffer, nonce);

    return PSA_SUCCESS;
}
