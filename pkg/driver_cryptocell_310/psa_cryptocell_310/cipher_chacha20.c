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
#include "unaligned.h"
#include "string_utils.h"

#define ENABLE_DEBUG    0
#include "debug.h"

static psa_status_t _setup(psa_cipher_chacha20_ctx_t *ctx,
                    uint8_t *key_data,
                    size_t key_length,
                    CRYS_CHACHA_EncryptMode_t mode)
{
    DEBUG("Peripheral ChaCha20 Cipher setup");

    if (key_length != CRYS_CHACHA_KEY_MAX_SIZE_IN_BYTES) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    ctx->buffer_length = 0;
    memset(ctx->buffer, 0, sizeof(ctx->buffer));
    /* Copy keydata into buffer for setup in psa_cipher_chacha20_set_iv() */
    memcpy(ctx->buffer, key_data, key_length);
    ctx->ctx.mode = mode;
    return PSA_SUCCESS;
}

psa_status_t psa_cipher_chacha20_encrypt_setup( psa_cipher_chacha20_ctx_t *ctx,
                                                uint8_t *key_data,
                                                size_t key_length)
{
    return _setup(ctx, key_data, key_length, CRYS_CHACHA_Encrypt);
}

psa_status_t psa_cipher_chacha20_decrypt_setup( psa_cipher_chacha20_ctx_t *ctx,
                                                uint8_t *key_data,
                                                size_t key_length)
{
    return _setup(ctx, key_data, key_length, CRYS_CHACHA_Decrypt);
}

psa_status_t psa_cipher_chacha20_set_iv(psa_cipher_chacha20_ctx_t *ctx,
                                        const uint8_t *iv,
                                        size_t iv_length)
{
    DEBUG("Peripheral ChaCha20 Cipher set IV");
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    CRYSError_t periph_status = CRYS_CHACHA_GENERAL_ERROR;

    /* See PSA Specification */
    switch (iv_length) {
        case 8:
            /* 8 bytes: the cipher operation uses the original [CHACHA20] definition
            of ChaCha20: the provided IV is used as the 64-bit nonce, and the 64-bit
            counter value is set to zero. */
            status = PSA_ERROR_NOT_SUPPORTED;
            break;
        case 12:
            /* 12 bytes: the provided IV is used as the nonce, and the counter value
            is set to zero. */
            cryptocell_310_enable();
            periph_status = CRYS_CHACHA_Init(   &ctx->ctx.post_setup,
                                                (uint8_t*)iv,
                                                CRYS_CHACHA_Nonce96BitSize,
                                                ctx->buffer,
                                                0,
                                                ctx->ctx.mode);
            cryptocell_310_disable();
            status = CRYS_to_psa_error(periph_status);
            break;
        case 16:
            /* 16 bytes: the first four bytes of the IV are used as the counter value
            (encoded as little-endian), and the remaining 12 bytes are used as the nonce. */
            periph_status = CRYS_CHACHA_Init(   &ctx->ctx.post_setup,
                                                (uint8_t*)&iv[4],
                                                CRYS_CHACHA_Nonce96BitSize,
                                                ctx->buffer,
                                                unaligned_get_u32(iv),
                                                ctx->ctx.mode);
            cryptocell_310_disable();
            status = CRYS_to_psa_error(periph_status);
            break;
        default:
            /* It is recommended that implementations do not support other sizes of IV. */
            status = PSA_ERROR_INVALID_ARGUMENT;
    }
    /* Clear key from buffer */
    explicit_bzero(ctx->buffer, sizeof(ctx->buffer));
    return status;
}

psa_status_t psa_cipher_chacha20_update(psa_cipher_chacha20_ctx_t *ctx,
                                        const uint8_t *input,
                                        size_t input_length,
                                        uint8_t *output,
                                        size_t output_size,
                                        size_t *output_length)
{
    DEBUG("Peripheral ChaCha20 Cipher update");
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    /* We return full blocks, so we check that we have enough output buffer
        to fit all full blocks in the combined context and input buffer */
    if (output_size < (((ctx->buffer_length + input_length) / \
        CRYS_CHACHA_BLOCK_SIZE_IN_BYTES) * CRYS_CHACHA_BLOCK_SIZE_IN_BYTES)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    size_t input_index = 0;
    size_t output_index = 0;
    CRYSError_t periph_status;
    /* check if buffer contains data */
    cryptocell_310_enable();
    if (ctx->buffer_length > 0) {
        /* if yes, fill buffer up and update this block of data */
        input_index = sizeof(ctx->buffer) - ctx->buffer_length;
        memcpy(&ctx->buffer[ctx->buffer_length], input, input_index);
        periph_status = CRYS_CHACHA_Block(  &ctx->ctx.post_setup,
                                            ctx->buffer,
                                            64,
                                            output);
        status = CRYS_to_psa_error(periph_status);
        if (status != PSA_SUCCESS) {
            return status;
        }
        memset(ctx->buffer, 0, sizeof(ctx->buffer));
        ctx->buffer_length = 0;
        output_index = 64;
    }

    /* if not, update blocks of input */
    const uint32_t blocks_length = ((input_length - input_index) / \
        CRYS_CHACHA_BLOCK_SIZE_IN_BYTES) * CRYS_CHACHA_BLOCK_SIZE_IN_BYTES;
    /* xcrypt full blocks */
    periph_status = CRYS_CHACHA_Block(  &ctx->ctx.post_setup,
                                        (uint8_t *)&input[input_index],
                                        blocks_length,
                                        &output[output_index]);
    cryptocell_310_disable();
    status = CRYS_to_psa_error(periph_status);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* put remaining data into buffer */
    ctx->buffer_length = input_length - (input_index + blocks_length);
    memcpy(ctx->buffer, &input[input_index + blocks_length], ctx->buffer_length);
    *output_length = output_index + blocks_length;

    return status;
}

psa_status_t psa_cipher_chacha20_finish(psa_cipher_chacha20_ctx_t *ctx,
                                        uint8_t *output,
                                        size_t output_size,
                                        size_t *output_length)
{
    DEBUG("Peripheral ChaCha20 Cipher finish.");
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    if (output_size < ctx->buffer_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    /* xcrypt remaining bytes */
    cryptocell_310_enable();
    CRYSError_t periph_status = CRYS_CHACHA_Finish( &ctx->ctx.post_setup,
                                                    ctx->buffer,
                                                    ctx->buffer_length,
                                                    output);
    cryptocell_310_disable();

    status = CRYS_to_psa_error(periph_status);
    if (status == PSA_SUCCESS) {
        *output_length = ctx->buffer_length;
    }
    return status;
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
