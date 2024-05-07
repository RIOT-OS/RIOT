/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_driver_cryptocell_310
 * @{
 *
 * @brief       Glue code translating between PSA Crypto and the CryptoCell 310 AES 128 CBC APIs
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "psa_error.h"
#include "psa_cryptocell_310_aes_common.h"
#include "ssi_aes.h"

#define ENABLE_DEBUG    0
#include "debug.h"

psa_status_t psa_cipher_cbc_aes_128_encrypt(const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            psa_algorithm_t alg,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *output,
                                            size_t output_size,
                                            size_t *output_length)
{
    if ((alg != PSA_ALG_CBC_PKCS7) && (alg != PSA_ALG_CBC_NO_PADDING)) {
        DEBUG("%s : Invalid CBC padding type\n", __FILE__);
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_buffer_size < PSA_BITS_TO_BYTES(attributes->bits)) {
        DEBUG("%s : Key buffer too small\n", __FILE__);
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    size_t iv_length = 0;
    size_t key_size = PSA_BITS_TO_BYTES(attributes->bits);

    psa_cipher_operation_t operation = psa_cipher_operation_init();
    operation.iv_required = 1;
    operation.default_iv_length = PSA_CIPHER_IV_LENGTH(attributes->type, alg);

    SaSiAesPaddingType_t padding =
        (alg == PSA_ALG_CBC_PKCS7) ? SASI_AES_PADDING_PKCS7 : SASI_AES_PADDING_NONE;

    status = psa_cipher_generate_iv(&operation, output, operation.default_iv_length, &iv_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = cryptocell_310_common_aes_setup((SaSiAesUserContext_t *)&operation.backend_ctx.cipher_ctx.aes_128,
                              SASI_AES_ENCRYPT, SASI_AES_MODE_CBC, padding, output, key_buffer,
                              key_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = cryptocell_310_common_aes_encrypt_decrypt(
                                (SaSiAesUserContext_t *)&operation.backend_ctx.cipher_ctx.aes_128,
                                input, input_length, output + iv_length, output_size - iv_length,
                                output_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    *output_length = output_size;

    return PSA_SUCCESS;
}

psa_status_t psa_cipher_cbc_aes_128_decrypt(const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            psa_algorithm_t alg,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *output,
                                            size_t output_size,
                                            size_t *output_length)
{
    if ((alg != PSA_ALG_CBC_PKCS7) && (alg != PSA_ALG_CBC_NO_PADDING)) {
        DEBUG("%s : Invalid CBC padding type\n", __FILE__);
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_buffer_size < PSA_BITS_TO_BYTES(attributes->bits)) {
        DEBUG("%s : Key buffer too small\n", __FILE__);
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    size_t key_size = PSA_BITS_TO_BYTES(attributes->bits);

    psa_cipher_operation_t operation = psa_cipher_operation_init();
    operation.iv_required = 1;
    operation.default_iv_length = PSA_CIPHER_IV_LENGTH(attributes->type, alg);

    SaSiAesPaddingType_t padding =
        (alg == PSA_ALG_CBC_PKCS7) ? SASI_AES_PADDING_PKCS7 : SASI_AES_PADDING_NONE;

    status = cryptocell_310_common_aes_setup((SaSiAesUserContext_t *)&operation.backend_ctx.cipher_ctx.aes_128,
                              SASI_AES_DECRYPT, SASI_AES_MODE_CBC, padding, input, key_buffer,
                              key_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = cryptocell_310_common_aes_encrypt_decrypt(
                                (SaSiAesUserContext_t *)&operation.backend_ctx.cipher_ctx.aes_128,
                                input + operation.default_iv_length, input_length - operation.
                                default_iv_length, output, output_size, output_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    *output_length = PSA_CIPHER_DECRYPT_OUTPUT_SIZE(attributes->type, alg, input_length);

    return PSA_SUCCESS;
}
