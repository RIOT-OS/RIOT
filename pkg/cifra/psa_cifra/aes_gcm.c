/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_psa_crypto pkg_cifra
 * @{
 *
 * @brief       Glue code translating between PSA Crypto and the Cifra APIs
 *
 * @author      Oliver Fritz <oliver.fritz-default@protonmail.com>
 * @author      Sergey Kreis <sergey.kreis@mailbox.tu-dresden.de>
 *
 * @}
 */

#include "modes.h"
#include "aes.h"
#include "psa/aead/sizes.h"
#include "psa/error.h"
#include "psa/key/attributes.h"

static psa_status_t _encrypt(const psa_key_attributes_t *attributes,
                                      uint8_t *key_buffer, size_t key_buffer_length,
                                      uint8_t tag_length, const uint8_t *nonce,
                                      size_t nonce_length, const uint8_t *additional_data,
                                      size_t additional_data_length, const uint8_t *plaintext,
                                      size_t plaintext_length, uint8_t *ciphertext,
                                      size_t ciphertext_size, size_t *ciphertext_length)
{
    (void)attributes;

    if (ciphertext_size < plaintext_length + tag_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    /* Note: tag_length must be between 1 and 16B. 16B is a full length
     * tag and anything smaller will result in a truncated tag. */
    if (tag_length == 0 || tag_length > PSA_AES_GCM_TAG_MAX_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    /* Note: The given nonce can be any size, but 12B are strongly
     * recommended according to the cifra documentation. This is
     * also the default for PSA_AEAD_NONCE_LENGTH with GCM! */

    cf_aes_context ctx;
    cf_aes_init(&ctx, key_buffer, key_buffer_length);

    cf_gcm_encrypt(&cf_aes,
                   &ctx,
                   plaintext,
                   plaintext_length,
                   additional_data,
                   additional_data_length,
                   nonce,
                   nonce_length,
                   ciphertext,
                   &ciphertext[plaintext_length],
                   tag_length);

    *ciphertext_length = plaintext_length + tag_length;

    return PSA_SUCCESS;
}

static psa_status_t _decrypt(const psa_key_attributes_t *attributes,
                                      uint8_t *key_buffer, size_t key_buffer_length,
                                      uint8_t tag_length, const uint8_t *nonce,
                                      size_t nonce_length, const uint8_t *additional_data,
                                      size_t additional_data_length, const uint8_t *ciphertext,
                                      size_t ciphertext_length, uint8_t *plaintext,
                                      size_t plaintext_size, size_t *plaintext_length)
{
    (void)attributes;

    if (plaintext_size < ciphertext_length - tag_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    if (tag_length == 0 || tag_length > PSA_AES_GCM_TAG_MAX_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    cf_aes_context ctx;
    cf_aes_init(&ctx, key_buffer, key_buffer_length);

    int ret = cf_gcm_decrypt(&cf_aes,
                             &ctx,
                             ciphertext,
                             ciphertext_length - tag_length,
                             additional_data,
                             additional_data_length,
                             nonce,
                             nonce_length,
                             &ciphertext[ciphertext_length - tag_length],
                             tag_length,
                             plaintext);

    if (ret != 0) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    *plaintext_length = ciphertext_length - tag_length;

    return PSA_SUCCESS;
}

psa_status_t psa_aead_aes_128_gcm_encrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *plaintext,
                                          size_t plaintext_length, uint8_t *ciphertext,
                                          size_t ciphertext_size, size_t *ciphertext_length)
{
    return _encrypt(attributes, key_buffer, key_buffer_length,
                                    tag_length, nonce, nonce_length, additional_data,
                                    additional_data_length, plaintext, plaintext_length,
                                    ciphertext, ciphertext_size, ciphertext_length);
}

psa_status_t psa_aead_aes_128_gcm_decrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *ciphertext,
                                          size_t ciphertext_length, uint8_t *plaintext,
                                          size_t plaintext_size, size_t *plaintext_length)
{
    return _decrypt(attributes, key_buffer, key_buffer_length,
                                    tag_length, nonce, nonce_length, additional_data,
                                    additional_data_length, ciphertext, ciphertext_length,
                                    plaintext, plaintext_size, plaintext_length);
}

psa_status_t psa_aead_aes_192_gcm_encrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *plaintext,
                                          size_t plaintext_length, uint8_t *ciphertext,
                                          size_t ciphertext_size, size_t *ciphertext_length)
{
    return _encrypt(attributes, key_buffer, key_buffer_length,
                                    tag_length, nonce, nonce_length, additional_data,
                                    additional_data_length, plaintext, plaintext_length,
                                    ciphertext, ciphertext_size, ciphertext_length);
}

psa_status_t psa_aead_aes_192_gcm_decrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *ciphertext,
                                          size_t ciphertext_length, uint8_t *plaintext,
                                          size_t plaintext_size, size_t *plaintext_length)
{
    return _decrypt(attributes, key_buffer, key_buffer_length,
                                    tag_length, nonce, nonce_length, additional_data,
                                    additional_data_length, ciphertext, ciphertext_length,
                                    plaintext, plaintext_size, plaintext_length);
}

psa_status_t psa_aead_aes_256_gcm_encrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *plaintext,
                                          size_t plaintext_length, uint8_t *ciphertext,
                                          size_t ciphertext_size, size_t *ciphertext_length)
{
    return _encrypt(attributes, key_buffer, key_buffer_length,
                                    tag_length, nonce, nonce_length, additional_data,
                                    additional_data_length, plaintext, plaintext_length,
                                    ciphertext, ciphertext_size, ciphertext_length);
}

psa_status_t psa_aead_aes_256_gcm_decrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *ciphertext,
                                          size_t ciphertext_length, uint8_t *plaintext,
                                          size_t plaintext_size, size_t *plaintext_length)
{
    return _decrypt(attributes, key_buffer, key_buffer_length,
                                    tag_length, nonce, nonce_length, additional_data,
                                    additional_data_length, ciphertext, ciphertext_length,
                                    plaintext, plaintext_size, plaintext_length);
}
