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
 * @file
 * @brief       Glue code translating between PSA Crypto and the RIOT Cipher module
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "psa/crypto.h"
#include "crypto/modes/cbc.h"

#define ENABLE_DEBUG    0
#include "debug.h"

psa_status_t cipher_to_psa_error(int error)
{
    switch (error) {
    case CIPHER_ERR_INVALID_KEY_SIZE:
    case CIPHER_ERR_INVALID_LENGTH:
    case CIPHER_ERR_BAD_CONTEXT_SIZE:
        return PSA_ERROR_INVALID_ARGUMENT;
    default:
        return PSA_ERROR_GENERIC_ERROR;
    }
}

psa_status_t cbc_aes_common_encrypt_decrypt(cipher_t *ctx,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            const uint8_t *iv,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *output,
                                            size_t *output_length,
                                            psa_encrypt_or_decrypt_t direction)
{
    int ret = 0;

    ret = cipher_init(ctx, CIPHER_AES, key_buffer, key_buffer_size);
    if (ret != CIPHER_INIT_SUCCESS) {
        return cipher_to_psa_error(ret);
    }

    if (direction == PSA_CRYPTO_DRIVER_ENCRYPT) {
        ret = cipher_encrypt_cbc(ctx, (uint8_t *)iv, input, input_length, output);
    }
    else {
        ret = cipher_decrypt_cbc(ctx, (uint8_t *)iv, input, input_length, output);
    }
    if (ret <= 0) {
        return cipher_to_psa_error(ret);
    }

    *output_length = ret;
    return PSA_SUCCESS;
}
