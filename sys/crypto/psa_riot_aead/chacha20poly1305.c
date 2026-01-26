#include "psa/cipher/types.h"
#include "psa/error.h"
#include "crypto/chacha20poly1305.h"
#include "psa_ciphers.h"

psa_status_t psa_aead_chacha20_poly1305_encrypt(const psa_key_attributes_t *attributes,
                                                uint8_t *key_buffer, size_t key_buffer_length,
                                                uint8_t tag_length, const uint8_t *nonce,
                                                size_t nonce_length, const uint8_t *additional_data,
                                                size_t additional_data_length, const uint8_t *plaintext,
                                                size_t plaintext_length, uint8_t *ciphertext,
                                                size_t ciphertext_size, size_t *ciphertext_length)
{
    (void)attributes;

    if (nonce_length != CHACHA20POLY1305_NONCE_BYTES ||
        tag_length != CHACHA20POLY1305_TAG_BYTES ||
        key_buffer_length != CHACHA20POLY1305_KEY_BYTES) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (ciphertext_size < plaintext_length + CHACHA20POLY1305_TAG_BYTES) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    chacha20poly1305_encrypt(ciphertext, plaintext, plaintext_length,
                             additional_data, additional_data_length,
                             key_buffer, nonce);

    *ciphertext_length = plaintext_length + CHACHA20POLY1305_TAG_BYTES;

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

    if (nonce_length != CHACHA20POLY1305_NONCE_BYTES ||
        tag_length != CHACHA20POLY1305_TAG_BYTES ||
        key_buffer_length != CHACHA20POLY1305_KEY_BYTES) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (plaintext_size < ciphertext_length - CHACHA20POLY1305_TAG_BYTES) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    int status = chacha20poly1305_decrypt(ciphertext, ciphertext_length,
                                          plaintext, plaintext_length,
                                          additional_data, additional_data_length,
                                          key_buffer, nonce);
    if (status == 0) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    return PSA_SUCCESS;
}

psa_status_t psa_aead_chacha20_poly1305_encrypt_setup(psa_aead_chacha20_poly1305_ctx_t *ctx,
                                                      uint8_t *key,
                                                      size_t key_length)
{
    if (key_length != CHACHA20POLY1305_KEY_BYTES) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Setup Chacha20 context, Poly key cannot be set yet as it
     * is derived from the full Chacha20 state, including nonce */
    psa_cipher_chacha20_encrypt_setup(&ctx->chacha, key, key_length);

    return PSA_SUCCESS;
}

psa_status_t psa_aead_chacha20_poly1305_decrypt_setup(psa_aead_chacha20_poly1305_ctx_t *ctx,
                                                      uint8_t *key,
                                                      size_t key_length)
{
    if (key_length != CHACHA20POLY1305_KEY_BYTES) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Setup Chacha20 context, Poly key cannot be set yet as it
     * is derived from the full Chacha20 state, including nonce */
    psa_cipher_chacha20_decrypt_setup(&ctx->chacha, key, key_length);

    return PSA_SUCCESS;
}

psa_status_t psa_aead_chacha20_poly1305_set_nonce(psa_aead_chacha20_poly1305_ctx_t *ctx,
                                                  const uint8_t *nonce,
                                                  size_t nonce_length)
{
    if (nonce_length != CHACHA20POLY1305_NONCE_BYTES) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Since the iv_length is 12 here, this effectively sets the nonce. */
    psa_cipher_chacha20_set_iv(&ctx->chacha, nonce, nonce_length);

#if IS_USED(MODULE_PSA_RIOT_CIPHER_CHACHA20)
    poly1305_init_from_chacha(&ctx->chacha.ctx, &ctx->poly);
#endif
    
#if IS_USED(MODULE_PERIPH_CIPHER_CHACHA20)
    static const uint8_t input[CRYS_CHACHA_BLOCK_SIZE_IN_BYTES] = { 0 };
    uint8_t output[CRYS_CHACHA_BLOCK_SIZE_IN_BYTES];
    size_t output_length;
    psa_cipher_chacha20_update(&ctx->chacha, input, sizeof(input), output, sizeof(output), &output_length);

    /* ciphertext = keystream xor plaintext --> keystream = plaintext xor ciphertext
     * Also increments counter of Chacha ctx to 1 for further encryption. */
    uint8_t chacha_keystream[32];
    for (int i = 0; i < 32; i++) {
        chacha_keystream[i] = input[i] ^ output[i];
    }

    poly1305_init(&ctx->poly, chacha_keystream);
#endif

    return PSA_SUCCESS;
}

psa_status_t psa_aead_chacha20_poly1305_update_ad(psa_aead_chacha20_poly1305_ctx_t *ctx,
                                                  const uint8_t *ad,
                                                  size_t ad_length)
{
    poly1305_update(&ctx->poly, ad, ad_length);

    return PSA_SUCCESS;
}

psa_status_t psa_aead_chacha20_poly1305_update(psa_aead_chacha20_poly1305_ctx_t *ctx,
                                               size_t setup_done,
                                               psa_encrypt_or_decrypt_t direction,
                                               const uint8_t *input,
                                               size_t input_length,
                                               uint8_t *output,
                                               size_t output_size,
                                               size_t *output_length)
{
    if (!setup_done) {
        /* When this function is called, no more additional data can be added.
         * So we pad the given ad to a full 16 byte (Poly) block. */
        static const uint8_t padding[15] = { 0 };
        const size_t padlen = (16 - ctx->poly.c_idx) & 0xF;
        poly1305_update(&ctx->poly, padding, padlen);
    }

    psa_status_t status = psa_cipher_chacha20_update(&ctx->chacha, input, input_length, output, output_size, output_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* No matter which direction, Poly always uses the ciphertext as input. */
    if (direction == PSA_CRYPTO_DRIVER_ENCRYPT) {
        poly1305_update(&ctx->poly, output, *output_length);
    }
    else {
        poly1305_update(&ctx->poly, input, input_length);
    }

    return PSA_SUCCESS;
}

psa_status_t psa_aead_chacha20_poly1305_finish(psa_aead_chacha20_poly1305_ctx_t *ctx,
                                               size_t total_ad_length,
                                               size_t current_ciphertext_length,
                                               uint8_t *ciphertext,
                                               size_t ciphertext_size,
                                               size_t *ciphertext_length,
                                               uint8_t *tag,
                                               size_t tag_size,
                                               size_t *tag_length)
{
    if (tag_size < CHACHA20POLY1305_TAG_BYTES) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    psa_status_t status = psa_cipher_chacha20_finish(&ctx->chacha, ciphertext, ciphertext_size, ciphertext_length);
    if (status != PSA_SUCCESS) {
        return status;
    }
    poly1305_update(&ctx->poly, ciphertext, *ciphertext_length);

    /* Now the last bit of ciphertext was calculated. We pad again! */
    static const uint8_t padding[15] = { 0 };
    const size_t padlen = (16 - ctx->poly.c_idx) & 0xF;
    poly1305_update(&ctx->poly, padding, padlen);

    const uint64_t lengths[2] = { total_ad_length, current_ciphertext_length };
    poly1305_update(&ctx->poly, (uint8_t *)lengths, sizeof(lengths));

    poly1305_finish(&ctx->poly, tag);
    *tag_length = CHACHA20POLY1305_TAG_BYTES;

    return PSA_SUCCESS;
}

psa_status_t psa_aead_chacha20_poly1305_verify(psa_aead_chacha20_poly1305_ctx_t *ctx,
                                               size_t total_ad_length,
                                               size_t current_ciphertext_length,
                                               uint8_t *plaintext,
                                               size_t plaintext_size,
                                               size_t *plaintext_length,
                                               const uint8_t *tag,
                                               size_t tag_length)
{
    psa_status_t status = psa_cipher_chacha20_finish(&ctx->chacha, plaintext, plaintext_size, plaintext_length);
    if (status != PSA_SUCCESS) {
        return status;
    }
    /* Since this is a decrypt operation, the last bit of ciphertext
     * was already added in psa_aead_update(). Likewise, we do not
     * need to calculate the total ciphertext length here! */
    static const uint8_t padding[15] = { 0 };
    const size_t padlen = (16 - ctx->poly.c_idx) & 0xF;
    poly1305_update(&ctx->poly, padding, padlen);

    const uint64_t lengths[2] = { total_ad_length, current_ciphertext_length };
    poly1305_update(&ctx->poly, (uint8_t *)lengths, sizeof(lengths));

    uint8_t calculated_tag[16];
    poly1305_finish(&ctx->poly, calculated_tag);

    if (memcmp(calculated_tag, tag, tag_length) != 0) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    return PSA_SUCCESS;
}
