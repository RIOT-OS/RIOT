/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto pkg_driver_cryptocell
 * @{
 *
 * @brief       Glue code translating between PSA Crypto and the CryptoCell 310 driver APIs
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "psa_error.h"
#include "psa_periph_aes_common.h"
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
    DEBUG("Periph AES 128 Cipher");
    if ((alg != PSA_ALG_CBC_PKCS7) && (alg != PSA_ALG_CBC_NO_PADDING)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    size_t iv_length = 0;

    psa_cipher_operation_t operation = psa_cipher_operation_init();
    operation.iv_required = 1;
    operation.default_iv_length = PSA_CIPHER_IV_LENGTH(attributes->type, alg);

    SaSiAesPaddingType_t padding =
        (alg == PSA_ALG_CBC_PKCS7) ? SASI_AES_PADDING_PKCS7 : SASI_AES_PADDING_NONE;

    status = psa_cipher_generate_iv(&operation, output, operation.default_iv_length, &iv_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = common_aes_setup((SaSiAesUserContext_t *)&operation.backend_ctx.cipher_ctx.aes_128,
                              SASI_AES_ENCRYPT, SASI_AES_MODE_CBC, padding, output, key_buffer,
                              key_buffer_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = common_aes_encrypt((SaSiAesUserContext_t *)&operation.backend_ctx.cipher_ctx.aes_128,
                                input, input_length, output + operation.default_iv_length,
                                output_size, output_length);
    if (status != PSA_SUCCESS) {
        return status;
    }
    return PSA_SUCCESS;
}
