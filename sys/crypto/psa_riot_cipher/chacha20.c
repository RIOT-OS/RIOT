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
 * @brief       Glue code translating between PSA Crypto and the RIOT ChaCha20 Cipher from the ChaCha20-Poly1305 Cipher module
 *
 * @author      Lennard Melling <lennard.melling@msx.tu-dresden.de>
 *
 * @}
 */

#include "psa/crypto.h"
#include "crypto/chacha20poly1305.h"

#define ENABLE_DEBUG    0
#include "debug.h"

psa_status_t psa_cipher_chacha20_encrypt(uint8_t *key_buffer,
                                         size_t key_buffer_size,
                                         const uint8_t *input,
                                         size_t input_length,
                                         uint8_t *output,
                                         size_t output_size,
                                         size_t *output_length)
{
    DEBUG("RIOT ChaCha20 Cipher encryption");
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    if (output_size < (input_length + CHACHA20POLY1305_NONCE_BYTES)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (key_buffer_size != CHACHA20POLY1305_KEY_BYTES) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    uint8_t *nonce = &output[0];
    uint8_t *data_out = &output[CHACHA20POLY1305_NONCE_BYTES];
    status = psa_generate_random(nonce, CHACHA20POLY1305_NONCE_BYTES);
    if (status != PSA_SUCCESS)
        return status;

    chacha20_encrypt_decrypt(input, data_out, key_buffer, nonce, input_length);

    *output_length = input_length + CHACHA20POLY1305_NONCE_BYTES;
    return PSA_SUCCESS;
}

psa_status_t psa_cipher_chacha20_decrypt(uint8_t *key_buffer,
                                         size_t key_buffer_size,
                                         const uint8_t *input,
                                         size_t input_length,
                                         uint8_t *output,
                                         size_t output_size,
                                         size_t *output_length)
{
    DEBUG("RIOT ChaCha20 Cipher decryption");

    if ((key_buffer_size != CHACHA20POLY1305_KEY_BYTES) ||
        (input_length < CHACHA20POLY1305_NONCE_BYTES)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (output_size < (input_length - CHACHA20POLY1305_NONCE_BYTES)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    const uint8_t *nonce = &input[0];
    const uint8_t *data_in = &input[CHACHA20POLY1305_NONCE_BYTES];

    chacha20_encrypt_decrypt(data_in, output, key_buffer, nonce, input_length - CHACHA20POLY1305_NONCE_BYTES);
    *output_length = input_length;
    return PSA_SUCCESS;
}
