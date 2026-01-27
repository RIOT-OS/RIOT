/*
 * SPDX-FileCopyrightText: 2024 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "string_utils.h"
#include "unaligned.h"
#include "psa/crypto.h"
#include "crypto/chacha20poly1305.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static psa_status_t _setup(psa_cipher_chacha20_ctx_t *ctx,
                           uint8_t *key_data,
                           size_t key_length)
{
    DEBUG("RIOT ChaCha20 Cipher setup");

    if (key_length != CHACHA20POLY1305_KEY_BYTES) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    ctx->buffer_length = 0;
    memset(ctx->buffer, 0, sizeof(ctx->buffer));
    /* Copy keydata into buffer for setup in psa_cipher_chacha20_set_iv() */
    memcpy(ctx->buffer, key_data, key_length);
    return PSA_SUCCESS;
}

psa_status_t psa_cipher_chacha20_encrypt_setup(psa_cipher_chacha20_ctx_t *ctx,
                                               uint8_t *key_data,
                                               size_t key_length)
{
    return _setup(ctx, key_data, key_length);
}

psa_status_t psa_cipher_chacha20_decrypt_setup(psa_cipher_chacha20_ctx_t *ctx,
                                               uint8_t *key_data,
                                               size_t key_length)
{
    return _setup(ctx, key_data, key_length);
}

psa_status_t psa_cipher_chacha20_set_iv(psa_cipher_chacha20_ctx_t *ctx,
                                        const uint8_t *iv,
                                        size_t iv_length)
{
    DEBUG("RIOT ChaCha20 Cipher set IV");
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    /* See PSA Specification */
    switch (iv_length) {
    case 8:
        /* 8 bytes: the cipher operation uses the original [CHACHA20] definition
         * of ChaCha20: the provided IV is used as the 64-bit nonce, and the 64-bit
         * counter value is set to zero.
         * This is currently not supported, as the current implementation only handles
         * 12-byte nonces. To change this, you would need to modify chacha20_ctx_t
         * and functions that are using this type.
         */
        status = PSA_ERROR_NOT_SUPPORTED;
        break;
    case 12:
        /* 12 bytes: the provided IV is used as the nonce, and the counter value
         * is set to zero. */
        chacha20_setup(&ctx->ctx, ctx->buffer, iv, 0);
        status = PSA_SUCCESS;
        break;
    case 16:
        /* 16 bytes: the first four bytes of the IV are used as the counter value
         * (encoded as little-endian), and the remaining 12 bytes are used as the nonce. */
        chacha20_setup(&ctx->ctx, ctx->buffer, &iv[4], unaligned_get_u32(iv));
        status = PSA_SUCCESS;
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
    DEBUG("RIOT ChaCha20 Cipher update");

    if (output_size < ((ctx->buffer_length + input_length) / CHACHA20POLY1305_BLOCK_BYTES) * CHACHA20POLY1305_BLOCK_BYTES) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    size_t input_idx = 0;
    size_t output_idx = 0;
    while (true) {
        if (ctx->buffer_length + input_length < CHACHA20POLY1305_BLOCK_BYTES) {
            memcpy(&ctx->buffer[ctx->buffer_length], &input[input_idx], input_length);
            ctx->buffer_length += input_length;
            input_idx += input_length;
            break;
        }
        memcpy(&ctx->buffer[ctx->buffer_length], &input[input_idx], CHACHA20POLY1305_BLOCK_BYTES - ctx->buffer_length);
        chacha20_update(&ctx->ctx, ctx->buffer, &output[output_idx]);
        input_length -= CHACHA20POLY1305_BLOCK_BYTES - ctx->buffer_length;
        input_idx += CHACHA20POLY1305_BLOCK_BYTES - ctx->buffer_length;
        ctx->buffer_length = 0;
        output_idx += CHACHA20POLY1305_BLOCK_BYTES;
    }

    *output_length = output_idx;

    return PSA_SUCCESS;
}

psa_status_t psa_cipher_chacha20_finish(psa_cipher_chacha20_ctx_t *ctx,
                                        uint8_t *output,
                                        size_t output_size,
                                        size_t *output_length)
{
    DEBUG("RIOT ChaCha20 Cipher finish");

    if (output_size < ctx->buffer_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    /* xcrypt remaining bytes */
    chacha20_finish(&ctx->ctx, ctx->buffer, ctx->buffer_length, output);

    *output_length = ctx->buffer_length;

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

    chacha20_encrypt_decrypt(key_buffer, nonce, 0, input, input_length, data_out);

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

    chacha20_encrypt_decrypt(key_buffer, nonce, 0, data_in, input_length - CHACHA20POLY1305_NONCE_BYTES, output);
    *output_length = input_length;
    return PSA_SUCCESS;
}
