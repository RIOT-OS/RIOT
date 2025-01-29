/*
 * Copyright (C) 2024 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_driver_cryptocell_310
 * @{
 *
 * @file
 * @brief       PSA Crypto wrapper for the CryptoCell 310 ChaCha API.
 *
 * @author      Lennard Melling <lennard.melling@msx.tu-dresden.de>
 *
 */
#ifndef PSA_CRYPTOCELL_310_CHACHA_H
#define PSA_CRYPTOCELL_310_CHACHA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"
#include "crys_chacha.h"
#include "cryptocell_310_util.h"
#include "psa_error.h"

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
    DEBUG("Peripheral ChaCha20 Cipher encryption");
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    if (!cryptocell_310_data_within_ram(input)) {
        DEBUG("%s : cryptocell_310 data required to be in RAM.\n", RIOT_FILE_RELATIVE);
        return PSA_ERROR_DATA_INVALID;
    }

    if (output_size < (input_length + CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (key_buffer_size != CRYS_CHACHA_KEY_MAX_SIZE_IN_BYTES) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    uint8_t *nonce = &output[0];
    uint8_t *data_out = &output[CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES];
    status = psa_generate_random(nonce, CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES);
    if (status != PSA_SUCCESS) {
        return status;
    }

    cryptocell_310_enable();
    CRYSError_t periph_status = CRYS_CHACHA(nonce, CRYS_CHACHA_Nonce96BitSize,
                                            key_buffer, 0UL,
                                            CRYS_CHACHA_Encrypt,
                                            (uint8_t *) input,
                                            input_length,
                                            data_out);
    cryptocell_310_disable();
    status = CRYS_to_psa_error(periph_status);
    if (status != PSA_SUCCESS) {
        return status;
    }

    *output_length = input_length + CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES;
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
    DEBUG("Peripheral ChaCha20 Cipher decryption");
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    if (!cryptocell_310_data_within_ram(input)) {
        DEBUG("%s : cryptocell_310 data required to be in RAM.\n", RIOT_FILE_RELATIVE);
        return PSA_ERROR_DATA_INVALID;
    }

    if ((key_buffer_size != CRYS_CHACHA_KEY_MAX_SIZE_IN_BYTES) ||
        (input_length < CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (output_size < (input_length - CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    const uint8_t *nonce = &input[0];
    const uint8_t *data_in = &input[CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES];
    size_t data_size = input_length - CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES;

    cryptocell_310_enable();
    CRYSError_t periph_status = CRYS_CHACHA((uint8_t *)nonce, CRYS_CHACHA_Nonce96BitSize,
                                            key_buffer, 0UL,
                                            CRYS_CHACHA_Decrypt,
                                            (uint8_t *)data_in,
                                            data_size,
                                            output);
    cryptocell_310_disable();
    status = CRYS_to_psa_error(periph_status);
    if (status != PSA_SUCCESS) {
        return status;
    }

    *output_length = data_size;
    return PSA_SUCCESS;
}

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTOCELL_310_CHACHA_H */
/** @} */
