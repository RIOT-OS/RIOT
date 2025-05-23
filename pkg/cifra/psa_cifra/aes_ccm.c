/*
 * Copyright (C) 2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto pkg_cifra
 * @{
 *
 * @brief       Glue code translating between PSA Crypto and the Cifra APIs
 *
 * @author      Lukas Luger <lukas.luger@mailbox.tu-dresden.de>
 *
 * @}
 */

#include "psa/crypto.h"
#include "aes.h"
#include "modes.h"
#include <assert.h>

psa_status_t psa_aead_encrypt_aes_ccm(const psa_key_attributes_t *attributes,
                                      uint8_t *key_buffer, size_t key_buffer_length,
                                      uint8_t tag_length, const uint8_t *nonce,
                                      size_t nonce_length, const uint8_t *additional_data,
                                      size_t additional_data_length, const uint8_t *plaintext,
                                      size_t plaintext_length, uint8_t *ciphertext,
                                      size_t ciphertext_size, size_t *ciphertext_length)
{
    (void)attributes;
    /* This should already have been checked by PSA. */
    assert(ciphertext_size >= plaintext_length + tag_length);

    uint8_t tag[PSA_AES_CCM_TAG_MAX_SIZE];

    cf_aes_context ctx;
    cf_aes_init(&ctx, key_buffer, key_buffer_length);

    cf_ccm_encrypt(&cf_aes, &ctx, plaintext, plaintext_length,
                   15 - nonce_length, additional_data, additional_data_length,
                   nonce, nonce_length, ciphertext, tag, tag_length);

    memcpy(&ciphertext[plaintext_length], tag, tag_length);

    *ciphertext_length = plaintext_length + tag_length;

    return PSA_SUCCESS;
}

psa_status_t psa_aead_decrypt_aes_ccm(const psa_key_attributes_t *attributes,
                                      uint8_t *key_buffer, size_t key_buffer_length,
                                      uint8_t tag_length, const uint8_t *nonce,
                                      size_t nonce_length, const uint8_t *additional_data,
                                      size_t additional_data_length, const uint8_t *ciphertext,
                                      size_t ciphertext_length, uint8_t *plaintext,
                                      size_t plaintext_size, size_t *plaintext_length)
{
    (void)attributes;
    /* This should already have been checked by PSA. */
    assert(plaintext_size >= ciphertext_length - tag_length);

    int ret;
    uint8_t tag[PSA_AES_CCM_TAG_MAX_SIZE];
    memcpy(tag, &ciphertext[plaintext_size], tag_length);

    cf_aes_context ctx;
    cf_aes_init(&ctx, key_buffer, key_buffer_length);

    ret = cf_ccm_decrypt(&cf_aes, &ctx, ciphertext, plaintext_size,
                         15 - nonce_length, additional_data, additional_data_length,
                         nonce, nonce_length, tag, tag_length, plaintext);

    if (ret != 0) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    *plaintext_length = ciphertext_length - tag_length;

    return PSA_SUCCESS;
}

psa_status_t psa_aead_aes_128_ccm_encrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *plaintext,
                                          size_t plaintext_length, uint8_t *ciphertext,
                                          size_t ciphertext_size, size_t *ciphertext_length)
{
    return psa_aead_encrypt_aes_ccm(attributes, key_buffer, key_buffer_length,
                                    tag_length, nonce, nonce_length, additional_data,
                                    additional_data_length, plaintext, plaintext_length,
                                    ciphertext, ciphertext_size, ciphertext_length);
}

psa_status_t psa_aead_aes_128_ccm_decrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *ciphertext,
                                          size_t ciphertext_length, uint8_t *plaintext,
                                          size_t plaintext_size, size_t *plaintext_length)
{
    return psa_aead_decrypt_aes_ccm(attributes, key_buffer, key_buffer_length,
                                    tag_length, nonce, nonce_length, additional_data,
                                    additional_data_length, ciphertext, ciphertext_length,
                                    plaintext, plaintext_size, plaintext_length);
}
psa_status_t psa_aead_aes_192_ccm_encrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *plaintext,
                                          size_t plaintext_length, uint8_t *ciphertext,
                                          size_t ciphertext_size, size_t *ciphertext_length)
{
    return psa_aead_encrypt_aes_ccm(attributes, key_buffer, key_buffer_length,
                                    tag_length, nonce, nonce_length, additional_data,
                                    additional_data_length, plaintext, plaintext_length,
                                    ciphertext, ciphertext_size, ciphertext_length);
}
psa_status_t psa_aead_aes_192_ccm_decrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *ciphertext,
                                          size_t ciphertext_length, uint8_t *plaintext,
                                          size_t plaintext_size, size_t *plaintext_length)
{
    return psa_aead_decrypt_aes_ccm(attributes, key_buffer, key_buffer_length,
                                    tag_length, nonce, nonce_length, additional_data,
                                    additional_data_length, ciphertext, ciphertext_length,
                                    plaintext, plaintext_size, plaintext_length);
}
psa_status_t psa_aead_aes_256_ccm_encrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *plaintext,
                                          size_t plaintext_length, uint8_t *ciphertext,
                                          size_t ciphertext_size, size_t *ciphertext_length)
{
    return psa_aead_encrypt_aes_ccm(attributes, key_buffer, key_buffer_length,
                                    tag_length, nonce, nonce_length, additional_data,
                                    additional_data_length, plaintext, plaintext_length,
                                    ciphertext, ciphertext_size, ciphertext_length);
}
psa_status_t psa_aead_aes_256_ccm_decrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *ciphertext,
                                          size_t ciphertext_length, uint8_t *plaintext,
                                          size_t plaintext_size, size_t *plaintext_length)
{
    return psa_aead_decrypt_aes_ccm(attributes, key_buffer, key_buffer_length,
                                    tag_length, nonce, nonce_length, additional_data,
                                    additional_data_length, ciphertext, ciphertext_length,
                                    plaintext, plaintext_size, plaintext_length);
}
