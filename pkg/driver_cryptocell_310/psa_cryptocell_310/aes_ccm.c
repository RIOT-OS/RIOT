/*
 * Copyright (C) 2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto pkg_driver_cryptocell_310
 * @{
 *
 * @brief       Glue code translating between PSA Crypto and the PSA Cryptocell 310 APIs
 *
 * @author      Lukas Luger <lukas.luger@mailbox.tu-dresden.de>
 *
 * @}
 */

#include "psa/crypto.h"
#include "crys_aesccm.h"
#include "crys_aesccm_error.h"
#include "psa_error.h"
#include "cryptocell_310_util.h"

#define ENABLE_DEBUG 0
#include "debug.h"

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

    if (!cryptocell_310_data_within_ram(nonce) ||
        !cryptocell_310_data_within_ram(key_buffer) ||
        !cryptocell_310_data_within_ram(additional_data) ||
        !cryptocell_310_data_within_ram(plaintext)) {
        DEBUG("%s : cryptocell_310 data required to be in RAM.\n", __FILE__);
        return PSA_ERROR_DATA_INVALID;
    }

    uint8_t tag[PSA_AES_CCM_TAG_MAX_SIZE];

    CRYSError_t ret = CC_AESCCM(SASI_AES_ENCRYPT, (uint8_t *)key_buffer, CRYS_AES_Key128BitSize,
                                (uint8_t *)nonce, nonce_length, (uint8_t *)additional_data,
                                additional_data_length, (uint8_t *)plaintext, plaintext_length,
                                ciphertext, tag_length, tag, CRYS_AESCCM_MODE_CCM);

    if (ret != CRYS_OK) {
        DEBUG("%s : cryptocell_310 failed to encrypt with %s.\n", __FILE__,
              cryptocell310_status_to_humanly_readable(ret));
        return CRYS_to_psa_error(ret);
    }

    memcpy(&ciphertext[plaintext_length], tag, tag_length);

    *ciphertext_length = plaintext_length + tag_length;

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

    if (!cryptocell_310_data_within_ram(nonce) ||
        !cryptocell_310_data_within_ram(key_buffer) ||
        !cryptocell_310_data_within_ram(additional_data) ||
        !cryptocell_310_data_within_ram(ciphertext)) {
        DEBUG("%s : cryptocell_310 data required to be in RAM.\n", __FILE__);
        return PSA_ERROR_DATA_INVALID;
    }

    uint8_t tag[PSA_AES_CCM_TAG_MAX_SIZE];
    memcpy(tag, &ciphertext[plaintext_size], tag_length);

    CRYSError_t ret = CC_AESCCM(SASI_AES_DECRYPT, (uint8_t *)key_buffer, CRYS_AES_Key128BitSize,
                                (uint8_t *)nonce, nonce_length, (uint8_t *)additional_data,
                                additional_data_length, (uint8_t *)ciphertext,
                                ciphertext_length - tag_length, plaintext, tag_length,
                                tag, CRYS_AESCCM_MODE_CCM);

    if (ret != CRYS_OK) {
        DEBUG("%s : cryptocell_310 failed to decrypt with %s.\n", __FILE__,
              cryptocell310_status_to_humanly_readable(ret));
        return CRYS_to_psa_error(ret);
    }

    *plaintext_length = ciphertext_length - tag_length;

    return PSA_SUCCESS;
}
