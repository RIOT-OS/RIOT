/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_driver_cryptocell_310
 * @{
 *
 * @file
 * @brief       PSA Crypto wrapper for the CryptoCell 310 ChachaPoly API.
 *
 * @author      Oliver Fritz <oliver.fritz-default@protonmail.com>
 * @author      Sergey Kreis <sergey.kreis@mailbox.tu-dresden.de>
 *
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "psa_error.h"
#include "crys_chacha_poly.h"
#include "crys_chacha_poly_error.h"
#include "cryptocell_310_util.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* This is defined to double check the PSA_AEAD_TAG_LENGTH
 * macro call in psa_crypto_algorithm_dispatch.c */
#define CHACHA20POLY1305_TAG_BYTES (16)

psa_status_t psa_aead_chacha20_poly1305_encrypt(const psa_key_attributes_t *attributes,
                                                uint8_t *key_buffer, size_t key_buffer_length,
                                                uint8_t tag_length, const uint8_t *nonce,
                                                size_t nonce_length, const uint8_t *additional_data,
                                                size_t additional_data_length, const uint8_t *plaintext,
                                                size_t plaintext_length, uint8_t *ciphertext,
                                                size_t ciphertext_size, size_t *ciphertext_length)
{
    (void)attributes;

    DEBUG("Peripheral ChaCha20-Poly1305 AEAD encryption\n");

    if (nonce_length != CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES ||
        tag_length != CHACHA20POLY1305_TAG_BYTES ||
        key_buffer_length != CRYS_CHACHA_KEY_MAX_SIZE_IN_BYTES) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    /* Cryptocell310 ChachaPoly specifically requires the inputs to be not 0.
     * This is in contrast to the definition of ChachaPoly in RFC 7539. */
    if (additional_data_length == 0 ||
        plaintext_length == 0) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    if (ciphertext_size < plaintext_length + CHACHA20POLY1305_TAG_BYTES) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    if (!cryptocell_310_data_within_ram(nonce) ||
        !cryptocell_310_data_within_ram(key_buffer) ||
        !cryptocell_310_data_within_ram(additional_data) ||
        !cryptocell_310_data_within_ram(plaintext)) {
        DEBUG("%s : cryptocell_310 data required to be in RAM.\n", __FILE__);
        return PSA_ERROR_DATA_INVALID;
    }

    cryptocell_310_enable();
    CRYSError_t ret = CRYS_CHACHA_POLY((uint8_t *)nonce,
                                       key_buffer,
                                       CRYS_CHACHA_Encrypt,
                                       (uint8_t *)additional_data,
                                       additional_data_length,
                                       (uint8_t *)plaintext,
                                       plaintext_length,
                                       ciphertext,
                                       (uint32_t *)&ciphertext[plaintext_length]);
    cryptocell_310_disable();

    if (ret != CRYS_OK) {
        DEBUG("%s : cryptocell_310 failed to encrypt with %s.\n", __FILE__,
              cryptocell310_status_to_humanly_readable(ret));
        return CRYS_to_psa_error(ret);
    }

    *ciphertext_length = plaintext_length + tag_length;

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

    DEBUG("Peripheral ChaCha20-Poly1305 AEAD decryption\n");

    if (nonce_length != CRYS_CHACHA_NONCE_MAX_SIZE_IN_BYTES ||
        tag_length != CHACHA20POLY1305_TAG_BYTES ||
        key_buffer_length != CRYS_CHACHA_KEY_MAX_SIZE_IN_BYTES) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (additional_data_length == 0 ||
        (ciphertext_length - tag_length) == 0) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    if (plaintext_size < ciphertext_length - tag_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    if (!cryptocell_310_data_within_ram(nonce) ||
        !cryptocell_310_data_within_ram(key_buffer) ||
        !cryptocell_310_data_within_ram(additional_data) ||
        !cryptocell_310_data_within_ram(ciphertext)) {
        DEBUG("%s : cryptocell_310 data required to be in RAM.\n", __FILE__);
        return PSA_ERROR_DATA_INVALID;
    }

    cryptocell_310_enable();
    CRYSError_t ret = CRYS_CHACHA_POLY((uint8_t *)nonce,
                                       key_buffer,
                                       CRYS_CHACHA_Decrypt,
                                       (uint8_t *)additional_data,
                                       additional_data_length,
                                       (uint8_t *)ciphertext,
                                       ciphertext_length - tag_length,
                                       plaintext,
                                       (uint32_t *)&ciphertext[ciphertext_length - tag_length]);
    cryptocell_310_disable();

    if (ret != CRYS_OK) {
        DEBUG("%s : cryptocell_310 failed to decrypt with %s.\n", __FILE__,
              cryptocell310_status_to_humanly_readable(ret));
        return CRYS_to_psa_error(ret);
    }

    *plaintext_length = ciphertext_length - tag_length;

    return PSA_SUCCESS;
}

#ifdef __cplusplus
}
#endif
