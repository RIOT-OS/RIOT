#include "psa/error.h"
#include <stdio.h>
#include "psa/crypto.h"
#include "crypto/chacha20poly1305.h"

psa_status_t psa_aead_chacha20_poly1305_encrypt(const psa_key_attributes_t *attributes,
                                                uint8_t *key_buffer, size_t key_buffer_length,
                                                uint8_t tag_length, const uint8_t *nonce,
                                                size_t nonce_length, const uint8_t *additional_data,
                                                size_t additional_data_length, const uint8_t *plaintext,
                                                size_t plaintext_length, uint8_t *ciphertext,
                                                size_t ciphertext_size, size_t *ciphertext_length)
{
    (void)attributes;
    (void)key_buffer_length;
    (void)tag_length;
    (void)ciphertext_size;
    (void)ciphertext_length;

    /* Only 12 and 8 byte nonces are supported. */
    switch (nonce_length) {
    case 12:
    case 8:
        break;
    default:
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    chacha20poly1305_encrypt(ciphertext, plaintext, plaintext_length,
                             additional_data, additional_data_length,
                             key_buffer, nonce);

    return PSA_SUCCESS;
}

psa_status_t psa_aead_chacha20_poly1305_decrypt(const psa_key_attributes_t *attributes,
                                                uint8_t *key_buffer, size_t key_buffer_length,
                                                uint8_t tag_length, const uint8_t *nonce,
                                                size_t nonce_length, const uint8_t *additional_data,
                                                size_t additional_data_length, const uint8_t *ciphertext,
                                                size_t ciphertext_length, uint8_t *plaintext,
                                                size_t plaintext_size, size_t *plaintext_length)
{
    (void)attributes;
    (void)key_buffer_length;
    (void)tag_length;
    (void)plaintext_size;

    /* Only 12 and 8 byte nonces are supported. */
    switch (nonce_length) {
    case 12:
    case 8:
        break;
    default:
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    chacha20poly1305_decrypt(ciphertext, ciphertext_length,
                             plaintext, plaintext_length,
                             additional_data, additional_data_length,
                             key_buffer, nonce);

    return PSA_SUCCESS;
}

psa_status_t psa_aead_chacha20_poly1305_encrypt_setup(psa_aead_chacha20_poly1305_ctx_t *ctx,
                                                      const uint8_t *key,
                                                      size_t key_length)
{
    if (key_length != CHACHA20POLY1305_KEY_BYTES) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    chacha20_poly1305_setup(&ctx->ctx, key);

    return PSA_SUCCESS;
}

psa_status_t psa_aead_chacha20_poly1305_decrypt_setup(psa_aead_chacha20_poly1305_ctx_t *ctx,
                                                      const uint8_t *key,
                                                      size_t key_length)
{
    if (key_length != CHACHA20POLY1305_KEY_BYTES) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    chacha20_poly1305_setup(&ctx->ctx, key);

    return PSA_SUCCESS;
}

psa_status_t psa_aead_chacha20_poly1305_set_nonce(psa_aead_chacha20_poly1305_ctx_t *ctx,
                                                  const uint8_t *nonce,
                                                  size_t nonce_length)
{
    if (nonce_length != CHACHA20POLY1305_NONCE_BYTES) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    chacha20_poly1305_set_nonce(&ctx->ctx, nonce);

    return PSA_SUCCESS;
}

psa_status_t psa_aead_chacha20_poly1305_update_ad(psa_aead_chacha20_poly1305_ctx_t *ctx,
                                                  const uint8_t *ad,
                                                  size_t ad_length)
{
    chacha20_poly1305_update_ad(&ctx->ctx, ad, ad_length);

    return PSA_SUCCESS;
}

psa_status_t psa_aead_chacha20_poly1305_update(psa_aead_chacha20_poly1305_ctx_t *ctx,
                                               size_t setup_done,
                                               const uint8_t *input,
                                               size_t input_length,
                                               uint8_t *output,
                                               size_t output_size,
                                               size_t *output_length)
{
    if (output_size < input_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    chacha20_poly1305_update(&ctx->ctx,
                             setup_done,
                             ctx->block_idx,
                             input,
                             input_length,
                             output);
    *output_length = input_length;

    return PSA_SUCCESS;
}
