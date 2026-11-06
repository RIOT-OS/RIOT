/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @brief       Example functions for AES GCM Encryption and Decryption with PSA Crypto
 *
 * @author  Oliver Fritz <oliver.fritz-default@protonmail.com>
 * @author  Sergey Kreis <sergey.kreis@mailbox.tu-dresden.de>
 *
 * @}
 */

#include <stdio.h>
#include "psa/crypto.h"

/* Test Vectors from the National Institute of Standards and Technology (NIST)
 *
 * https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Algorithm-Validation-Program/documents/mac/gcmvs.pdf
 * See page 4 for a download of the test vectors.
 */

/* Keys */
static const uint8_t KEY_GCM_128[] = {
    0xFE, 0xDC, 0x71, 0x55, 0x19, 0x2D, 0x00, 0xB2,
    0x3C, 0xDD, 0x98, 0x75, 0x0D, 0xB9, 0xEB, 0xBA
};

static const uint8_t KEY_GCM_192[] = {
    0x0F, 0x88, 0x77, 0xB5, 0x62, 0x37, 0xEB, 0xCA,
    0x46, 0x85, 0xAB, 0xDD, 0xF3, 0x94, 0xCE, 0x66,
    0x2A, 0x9C, 0x01, 0x2D, 0x88, 0xEE, 0xED, 0xA1
};

static const uint8_t KEY_GCM_256[] = {
    0x46, 0x3B, 0x41, 0x29, 0x11, 0x76, 0x7D, 0x57,
    0xA0, 0xB3, 0x39, 0x69, 0xE6, 0x74, 0xFF, 0xE7,
    0x84, 0x5D, 0x31, 0x3B, 0x88, 0xC6, 0xFE, 0x31,
    0x2F, 0x3D, 0x72, 0x4B, 0xE6, 0x8E, 0x1F, 0xCA
};

/* Nonces */
static const uint8_t NONCE_GCM_128[] = {
    0xA7, 0x6B, 0x74, 0xF5, 0x5C, 0x1A,
    0x17, 0x56, 0xA0, 0x83, 0x38, 0xB1
};

static const uint8_t NONCE_GCM_192[] = {
    0x18, 0x17, 0xA5, 0xCA, 0x06, 0xE3,
    0x43, 0x2B, 0x2E, 0x01, 0x5D, 0xB6
};

static const uint8_t NONCE_GCM_256[] = {
    0x61, 0x1C, 0xE6, 0xF9, 0xA6, 0x88,
    0x07, 0x50, 0xDE, 0x7D, 0xA6, 0xCB
};

/* Plaintexts */
static const uint8_t PLAINTEXT_GCM_128[] = {
    0x68, 0x31, 0x43, 0x5B, 0x88, 0x57, 0xDA, 0xF1,
    0xC5, 0x13, 0xB1, 0x48, 0x82, 0x0D, 0x13, 0xB5,
    0xA7, 0x2C, 0xC4, 0x90, 0xBD, 0xA7, 0x9A, 0x98,
    0xA6, 0xF5, 0x20, 0xD8, 0x76, 0x3C, 0x39, 0xD1
};

static const uint8_t PLAINTEXT_GCM_192[] = {
    0x2F, 0xA4, 0xC3, 0x83, 0x77, 0xF6, 0x68, 0x42,
    0xAD, 0x54, 0x6C, 0x04, 0x3C, 0x9E, 0xE8, 0x1B,
    0x33, 0xE6, 0xB4, 0xCD, 0x5A, 0x66, 0x47, 0x52,
    0x0F, 0xD5, 0x58, 0x9C, 0xBA, 0x20, 0xE3, 0x66
};

static const uint8_t PLAINTEXT_GCM_256[] = {
    0xE7, 0xD1, 0xDC, 0xF6, 0x68, 0xE2, 0x87, 0x68,
    0x61, 0x94, 0x0E, 0x01, 0x2F, 0xE5, 0x2A, 0x98,
    0xDA, 0xCB, 0xD7, 0x8A, 0xB6, 0x3C, 0x08, 0x84,
    0x2C, 0xC9, 0x80, 0x1E, 0xA5, 0x81, 0x68, 0x2A,
    0xD5, 0x4A, 0xF0, 0xC3, 0x4D, 0x0D, 0x7F, 0x6F,
    0x59, 0xE8, 0xEE, 0x0B, 0xF4, 0x90, 0x0E, 0x0F,
    0xD8, 0x50, 0x42
};

/* Additional Data */
static const uint8_t ADDITIONAL_DATA_GCM_128[] = {
    0x2A, 0xD2, 0x06, 0xC4, 0x17, 0x6E, 0x7E, 0x55,
    0x2A, 0xA0, 0x88, 0x36, 0x88, 0x68, 0x16, 0xFA,
    0xFA, 0x77, 0xE7, 0x59
};

static const uint8_t ADDITIONAL_DATA_GCM_192[] = {
    0x0B, 0xB4, 0x95, 0x97, 0x7B, 0xA3, 0xEA, 0x02,
    0xE8, 0x02, 0x02, 0x93, 0x81, 0x7C, 0xE5, 0xC5,
    0xEA, 0xBA, 0x0C, 0x5D
};

static const uint8_t ADDITIONAL_DATA_GCM_256[] = {
    0x0A, 0x68, 0x2F, 0xBC, 0x61, 0x92, 0xE1, 0xB4,
    0x7A, 0x5E, 0x08, 0x68, 0x78, 0x7F, 0xFD, 0xAF,
    0xE5, 0xA5, 0x0C, 0xEA, 0xD3, 0x57, 0x58, 0x49,
    0x99, 0x0C, 0xDD, 0x2E, 0xA9, 0xB3, 0x59, 0x77,
    0x49, 0x40, 0x3E, 0xFB, 0x4A, 0x56, 0x68, 0x4F,
    0x0C, 0x6B, 0xDE, 0x35, 0x2D, 0x4A, 0xEE, 0xC5
};

/* Ciphertexts */
static uint8_t CIPHERTEXT_GCM_128[] = {
    0x15, 0x82, 0x38, 0x05, 0xDA, 0x89, 0xA1, 0x92,
    0x3B, 0xFC, 0x1D, 0x6F, 0x87, 0x78, 0x4D, 0x56,
    0xBA, 0xD1, 0x12, 0x8B, 0x4D, 0xFF, 0xDB, 0xDE,
    0xEF, 0xBB, 0x2F, 0xA5, 0x62, 0xC3, 0x5E, 0x68,
    /* The last 16B of the ciphertext are the tag. */
    0xD2, 0x3D, 0xC4, 0x55, 0xCE, 0xD4, 0x98, 0x87,
    0xC7, 0x17, 0xE8, 0xEA, 0xBE, 0xEC, 0x29, 0x84
};

static uint8_t CIPHERTEXT_GCM_192[] = {
    0x7C, 0x03, 0xDA, 0xD4, 0xC8, 0xAC, 0x7F, 0xDF,
    0x40, 0x45, 0x3B, 0x14, 0x84, 0xA4, 0xDB, 0x42,
    0x83, 0x9A, 0x0A, 0x06, 0xB6, 0x99, 0x51, 0xA5,
    0x73, 0x12, 0x3A, 0x0D, 0x80, 0x32, 0xD6, 0xB3,
    0x3F, 0xC4, 0x9F, 0x14, 0xBE, 0xE1, 0x22, 0x32,
    0x4A, 0xEF, 0x23, 0x65, 0x46, 0xB3, 0x01, 0xD9
};

static uint8_t CIPHERTEXT_GCM_256[] = {
    0x88, 0x86, 0xE1, 0x96, 0x01, 0x0C, 0xB3, 0x84,
    0x9D, 0x9C, 0x1A, 0x18, 0x2A, 0xBE, 0x1E, 0xEA,
    0xB0, 0xA5, 0xF3, 0xCA, 0x42, 0x3C, 0x36, 0x69,
    0xA4, 0xA8, 0x70, 0x3C, 0x0F, 0x14, 0x6E, 0x8E,
    0x95, 0x6F, 0xB1, 0x22, 0xE0, 0xD7, 0x21, 0xB8,
    0x69, 0xD2, 0xB6, 0xFC, 0xD4, 0x21, 0x6D, 0x7D,
    0x4D, 0x37, 0x58,
    0x24, 0x69, 0xCE, 0xCD, 0x70, 0xFD, 0x98, 0xFE,
    0xC9, 0x26, 0x4F, 0x71, 0xDF, 0x1A, 0xEE, 0x9A
};

psa_status_t _example_aes_gcm(const uint8_t *key, const size_t key_length,
                              const uint8_t *nonce, const size_t nonce_length,
                              const uint8_t *plaintext, const size_t plaintext_length,
                              const uint8_t *aad, const size_t aad_length,
                              const uint8_t *ciphertext, const size_t ciphertext_length)
{
    psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;

    /* KEY IMPORT ------------------------ */
    psa_key_id_t key_id = 0;
    psa_key_attributes_t attr = psa_key_attributes_init();
    psa_key_usage_t usage = PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT;
    /* Note: To have a truncated tag, use PSA_ALG_AEAD_WITH_SHORTENED_TAG */
    psa_algorithm_t alg = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(PSA_ALG_GCM);

    psa_set_key_algorithm(&attr, alg);
    psa_set_key_usage_flags(&attr, usage);
    psa_set_key_bits(&attr, PSA_BYTES_TO_BITS(key_length));
    psa_set_key_type(&attr, PSA_KEY_TYPE_AES);

    status = psa_import_key(&attr, key, key_length, &key_id);
    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        printf("Import Key Error: %s\n", psa_status_to_humanly_readable(status));
        return status;
    }
    /* KEY IMPORT ------------------------ */

    /* ENCRYPT & TAG-GEN ----------------- */
    const size_t ciphertext_out_size = PSA_AEAD_ENCRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_AES,
                                                                    alg,
                                                                    plaintext_length);
    uint8_t ciphertext_out[ciphertext_out_size];

    size_t output_len;
    status = psa_aead_encrypt(key_id,
                              alg,
                              nonce,
                              nonce_length,
                              aad,
                              aad_length,
                              plaintext,
                              plaintext_length,
                              ciphertext_out,
                              ciphertext_out_size,
                              &output_len);

    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        printf("Encrypt Error: %s\n", psa_status_to_humanly_readable(status));
        return status;
    }

    if (memcmp(ciphertext_out, ciphertext, ciphertext_length) != 0) {
        psa_destroy_key(key_id);
        puts("Wrong ciphertext on encryption\n");
        return PSA_ERROR_DATA_INVALID;
    }
    /* ENCRYPT & TAG-GEN ----------------- */

    /* DECRYPT & VERIFY ------------------ */
    const size_t plaintext_out_size = PSA_AEAD_DECRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_AES,
                                                                   alg,
                                                                   ciphertext_length);
    uint8_t plaintext_out[plaintext_out_size];

    status = psa_aead_decrypt(key_id,
                              alg,
                              nonce,
                              nonce_length,
                              aad,
                              aad_length,
                              ciphertext,
                              ciphertext_length,
                              plaintext_out,
                              plaintext_out_size,
                              &output_len);

    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        printf("Decrypt Error: %s\n", psa_status_to_humanly_readable(status));
        return status;
    }

    if (memcmp(plaintext_out, plaintext, plaintext_length) != 0) {
        printf("Wrong plaintext on decryption\n");
        return PSA_ERROR_DATA_INVALID;
    }
    /* DECRYPT & VERIFY ------------------ */

    psa_destroy_key(key_id);

    return status;
}

/**
 * @brief   Example function to perform an AEAD AES GCM 128 encryption and decryption
 *          with the PSA Crypto API.
 *
 * @return  psa_status_t
 */
psa_status_t example_aead_aes_gcm_128_oneshot(void)
{
    return _example_aes_gcm(KEY_GCM_128, sizeof(KEY_GCM_128),
                            NONCE_GCM_128, sizeof(NONCE_GCM_128),
                            PLAINTEXT_GCM_128, sizeof(PLAINTEXT_GCM_128),
                            ADDITIONAL_DATA_GCM_128, sizeof(ADDITIONAL_DATA_GCM_128),
                            CIPHERTEXT_GCM_128, sizeof(CIPHERTEXT_GCM_128));
}

/**
 * @brief   Example function to perform an AEAD AES GCM 192 encryption and decryption
 *          with the PSA Crypto API.
 *
 * @return  psa_status_t
 */
psa_status_t example_aead_aes_gcm_192_oneshot(void)
{
    return _example_aes_gcm(KEY_GCM_192, sizeof(KEY_GCM_192),
                            NONCE_GCM_192, sizeof(NONCE_GCM_192),
                            PLAINTEXT_GCM_192, sizeof(PLAINTEXT_GCM_192),
                            ADDITIONAL_DATA_GCM_192, sizeof(ADDITIONAL_DATA_GCM_192),
                            CIPHERTEXT_GCM_192, sizeof(CIPHERTEXT_GCM_192));
}

/**
 * @brief   Example function to perform an AEAD AES GCM 256 encryption and decryption
 *          with the PSA Crypto API.
 *
 * @return  psa_status_t
 */
psa_status_t example_aead_aes_gcm_256_oneshot(void)
{
    return _example_aes_gcm(KEY_GCM_256, sizeof(KEY_GCM_256),
                            NONCE_GCM_256, sizeof(NONCE_GCM_256),
                            PLAINTEXT_GCM_256, sizeof(PLAINTEXT_GCM_256),
                            ADDITIONAL_DATA_GCM_256, sizeof(ADDITIONAL_DATA_GCM_256),
                            CIPHERTEXT_GCM_256, sizeof(CIPHERTEXT_GCM_256));
}
