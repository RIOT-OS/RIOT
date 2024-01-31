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
 * @brief       Glue code translating between PSA Crypto and the RIOT Cipher module
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "psa/crypto.h"
#include "crypto/modes/cbc.h"
#include "aes_common.h"

#define ENABLE_DEBUG    0
#include "debug.h"

psa_status_t psa_cipher_cbc_aes_256_encrypt(const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            psa_algorithm_t alg,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *output,
                                            size_t output_size,
                                            size_t *output_length)
{
    DEBUG("RIOT AES 256 Cipher");
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_cipher_operation_t operation = psa_cipher_operation_init();
    size_t iv_length = 0;
    size_t required_output_buf_size = PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_AES,
                                        PSA_ALG_CBC_NO_PADDING, input_length);

    if (output_size < required_output_buf_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    operation.iv_required = 1;
    operation.default_iv_length = PSA_CIPHER_IV_LENGTH(attributes->type, alg);
    *output_length = 0;

    status = psa_cipher_generate_iv(&operation, output, operation.default_iv_length, &iv_length);
    if (status != PSA_SUCCESS) {
        return status;
    }
    status = cbc_aes_common(&operation.backend_ctx.cipher_ctx.aes_256, key_buffer, key_buffer_size,
                            output, input, input_length, output + operation.default_iv_length,
                            output_length);
    return status;
}
