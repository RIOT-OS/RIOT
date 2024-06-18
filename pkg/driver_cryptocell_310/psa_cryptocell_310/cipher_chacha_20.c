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
//#include "psa_ciphers.h" //TODO: warum kein include in den anderen gluecodes
#include "psa_error.h"

#define ENABLE_DEBUG    0
#include "debug.h"

psa_status_t psa_cipher_chacha20_setup(psa_cipher_chacha_20_ctx_t *ctx,
                                       psa_cipher_chacha_20_nonce_t nonce,
                                       uint8_t *key,
                                       uint32_t initial_counter,
                                       psa_encrypt_or_decrypt_t mode)
{
    
    cryptocell_310_enable();
    CRYSError_t periph_status = CRYS_CHACHA_Init((CRYS_CHACHAUserContext_t *) ctx,
                                                 nonce, 
                                                 CRYS_CHACHA_Nonce96BitSize,
                                                 key,
                                                 initial_counter,
                                                 (CRYS_CHACHA_EncryptMode_t) mode);
    cryptocell_310_disable();
    
    if (periph_status != CRYS_OK) {
        
        DEBUG("CRYS_CHACHA_Init failed with %s\n", cryptocell310_status_to_humanly_readable(periph_status));
        CRYS_CHACHA_Free((CRYS_CHACHAUserContext_t *)ctx);
        return CRYS_to_psa_error(periph_status);
    }
    return PSA_SUCCESS;
}

psa_status_t psa_cipher_chacha20_update(psa_cipher_chacha_20_ctx_t *ctx,
                                        uint8_t *input,
                                        uint32_t input_length,
                                        uint8_t *output,
                                        uint32_t output_length)
{
    if (output_length < input_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if ((input_length % CRYS_CHACHA_BLOCK_SIZE_IN_BYTES) != 0) {
        return PSA_ERROR_INSUFFICIENT_DATA;
    }
    cryptocell_310_enable();
    CRYSError_t periph_status = CRYS_CHACHA_Block((CRYS_CHACHAUserContext_t *) ctx,
                                                  input, input_length, output);
    cryptocell_310_disable();

    if (periph_status != CRYS_OK) {
        DEBUG("CRYS_CHACHA_Block failed with %s\n", cryptocell310_status_to_humanly_readable(periph_status));
        CRYS_CHACHA_Free((CRYS_CHACHAUserContext_t *)ctx);
        return CRYS_to_psa_error(periph_status);
    }
    return PSA_SUCCESS;
}

psa_status_t psa_cipher_chacha20_finish(psa_cipher_chacha_20_ctx_t *ctx,
                                        uint8_t *input,
                                        uint32_t input_length,
                                        uint8_t *output,
                                        uint32_t output_length)
{
    if (output_length < input_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    if (input_length < CRYS_CHACHA_BLOCK_SIZE_IN_BYTES) {
        return PSA_ERROR_BAD_STATE;
    }

    cryptocell_310_enable();
    CRYSError_t periph_status = CRYS_CHACHA_Finish((CRYS_CHACHAUserContext_t *) ctx,
                                                  input, input_length, output);
    cryptocell_310_disable();    

    if (periph_status != CRYS_OK) {
        DEBUG("CRYS_CHACHA_Finish failed with %s\n", cryptocell310_status_to_humanly_readable(periph_status));
        CRYS_CHACHA_Free((CRYS_CHACHAUserContext_t *)ctx);
        return CRYS_to_psa_error(periph_status);
    }
    return PSA_SUCCESS;
}

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

    if (output_size < (input_length + CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (key_buffer_size != CRYS_CHACHA_KEY_MAX_SIZE_IN_BYTES) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    CRYS_CHACHA_Nonce_t nonce = {0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x01};
    /* status = psa_generate_random(nonce, CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES);
    if (status != PSA_SUCCESS)
        return status;
 */
    /* TODO: avoid this and somehow get the const pointer to work
    you cannot simply cast the const pointer input, as then the CRYS function will assume an
    array of 0x00. This only happens with the encrypt function.*/
    uint8_t *in = malloc(input_length);
    memcpy(in, input, input_length);

    cryptocell_310_enable();
    CRYSError_t periph_status = CRYS_CHACHA(nonce, CRYS_CHACHA_Nonce96BitSize,
                                            key_buffer, 0UL,
                                            CRYS_CHACHA_Encrypt, in,
                                            input_length, output);
    cryptocell_310_disable();
    
    status = CRYS_to_psa_error(periph_status);
    if (status != PSA_SUCCESS) {
        return status;
    }

    memcpy(&output[input_length], nonce, CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES);

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

    if ((key_buffer_size != CRYS_CHACHA_KEY_MAX_SIZE_IN_BYTES) || 
        (input_length < CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }


    if (output_size < (input_length - CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    CRYS_CHACHA_Nonce_t nonce;
    memcpy(nonce, &input[input_length - CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES], CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES);

    cryptocell_310_enable();
    CRYSError_t periph_status = CRYS_CHACHA(nonce, CRYS_CHACHA_Nonce96BitSize,
                                            key_buffer, 0UL,
                                            CRYS_CHACHA_Decrypt, (uint8_t *)input,
                                            input_length - CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES,
                                            output);
    cryptocell_310_disable();
    status = CRYS_to_psa_error(periph_status);
    if (status != PSA_SUCCESS) {
        return status;
    }

    *output_length = input_length - CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES;
    return PSA_SUCCESS;
}

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTOCELL_310_CHACHA_H */
/** @} */
