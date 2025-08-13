/*
 * Copyright (C) 2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto pkg_tinycrypt
 * @{
 *
 * @brief       Glue code translating between PSA Crypto and the tinycrypt APIs
 *
 * @author      Lukas Luger <lukas.luger@mailbox.tu-dresden.de>
 *
 * @}
 */

#include "psa/crypto.h"
#include "tinycrypt/aes.h"
#include "tinycrypt/ccm_mode.h"
#include <assert.h>

psa_status_t psa_aead_aes_128_ccm_encrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *plaintext,
                                          size_t plaintext_length, uint8_t *ciphertext,
                                          size_t ciphertext_size, size_t *ciphertext_length)
{
    (void)attributes;
    (void)key_buffer_length;
    /* This should already have been checked by PSA. */
    assert(ciphertext_size >= plaintext_length + tag_length);

    int ret;
    struct tc_ccm_mode_struct c;
    struct tc_aes_key_sched_struct sched;
    /* tinycrypt only supports a nonce_length of 13 */
    if (nonce_length != 13 || additional_data_length >= TC_CCM_AAD_MAX_BYTES ||
        plaintext_length >= TC_CCM_PAYLOAD_MAX_BYTES) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    tc_aes128_set_encrypt_key(&sched, key_buffer);

    ret = tc_ccm_config(&c, &sched, (uint8_t *)nonce, nonce_length, tag_length);
    if (ret != 1) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    ret = tc_ccm_generation_encryption(ciphertext, ciphertext_size, additional_data,
                                       additional_data_length, plaintext,
                                       plaintext_length, &c);
    if (ret != 1) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    *ciphertext_length = ciphertext_size;

    return PSA_SUCCESS;
}

psa_status_t psa_aead_aes_128_ccm_decrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *ciphertext,
                                          size_t ciphertext_length, uint8_t *plaintext,
                                          size_t plaintext_size, size_t *plaintext_length)
{
    (void)attributes;
    (void)key_buffer_length;
    /* This should already have been checked by PSA. */
    assert(plaintext_size >= ciphertext_length - tag_length);

    int ret;

    struct tc_ccm_mode_struct c;
    struct tc_aes_key_sched_struct sched;
    /* tinycrypt only supports a nonce_length of 13 */
    if (nonce_length != 13 || additional_data_length >= TC_CCM_AAD_MAX_BYTES ||
        ciphertext_length >= TC_CCM_PAYLOAD_MAX_BYTES) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    tc_aes128_set_decrypt_key(&sched, key_buffer);

    ret = tc_ccm_config(&c, &sched, (uint8_t *)nonce, nonce_length, tag_length);
    if (ret != 1) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    ret = tc_ccm_decryption_verification(plaintext, plaintext_size, additional_data,
                                         additional_data_length, ciphertext,
                                         ciphertext_length, &c);
    if (ret != 1) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    *plaintext_length = plaintext_size;

    return PSA_SUCCESS;
}
