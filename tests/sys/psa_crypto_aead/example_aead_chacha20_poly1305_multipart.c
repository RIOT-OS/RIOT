/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @brief   Example functions for Chacha20Poly1305 Multipart Encryption and Decryption with PSA Crypto
 *
 * @author  Oliver Fritz <oliver.fritz-default@protonmail.com>
 * @author  Sergey Kreis <sergey.kreis@mailbox.tu-dresden.de>
 *
 * @}
 */
#include <stdio.h>
#include "psa/crypto.h"

/* Test Vector from RFC 7539, https://datatracker.ietf.org/doc/html/rfc7539. */

static const uint8_t PLAINTEXT[] = {
    0x4c, 0x61, 0x64, 0x69, 0x65, 0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x47, 0x65, 0x6e, 0x74, 0x6c,
    0x65, 0x6d, 0x65, 0x6e, 0x20, 0x6f, 0x66, 0x20, 0x74, 0x68, 0x65, 0x20, 0x63, 0x6c, 0x61, 0x73,
    0x73, 0x20, 0x6f, 0x66, 0x20, 0x27, 0x39, 0x39, 0x3a, 0x20, 0x49, 0x66, 0x20, 0x49, 0x20, 0x63,
    0x6f, 0x75, 0x6c, 0x64, 0x20, 0x6f, 0x66, 0x66, 0x65, 0x72, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x6f,
    0x6e, 0x6c, 0x79, 0x20, 0x6f, 0x6e, 0x65, 0x20, 0x74, 0x69, 0x70, 0x20, 0x66, 0x6f, 0x72, 0x20,
    0x74, 0x68, 0x65, 0x20, 0x66, 0x75, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x73, 0x75, 0x6e, 0x73,
    0x63, 0x72, 0x65, 0x65, 0x6e, 0x20, 0x77, 0x6f, 0x75, 0x6c, 0x64, 0x20, 0x62, 0x65, 0x20, 0x69,
    0x74, 0x2e
};

static const uint8_t ADDITIONAL_DATA[] = {
    0x50, 0x51, 0x52, 0x53,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7
};

static const uint8_t KEY_CHACHA20[] = {
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f
};

static const uint8_t NONCE[] = {
    0x07, 0x00, 0x00, 0x00, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47
};

static const uint8_t CIPHERTEXT[] = {
    0xd3, 0x1a, 0x8d, 0x34, 0x64, 0x8e, 0x60, 0xdb, 0x7b, 0x86, 0xaf, 0xbc, 0x53, 0xef, 0x7e, 0xc2,
    0xa4, 0xad, 0xed, 0x51, 0x29, 0x6e, 0x08, 0xfe, 0xa9, 0xe2, 0xb5, 0xa7, 0x36, 0xee, 0x62, 0xd6,
    0x3d, 0xbe, 0xa4, 0x5e, 0x8c, 0xa9, 0x67, 0x12, 0x82, 0xfa, 0xfb, 0x69, 0xda, 0x92, 0x72, 0x8b,
    0x1a, 0x71, 0xde, 0x0a, 0x9e, 0x06, 0x0b, 0x29, 0x05, 0xd6, 0xa5, 0xb6, 0x7e, 0xcd, 0x3b, 0x36,
    0x92, 0xdd, 0xbd, 0x7f, 0x2d, 0x77, 0x8b, 0x8c, 0x98, 0x03, 0xae, 0xe3, 0x28, 0x09, 0x1b, 0x58,
    0xfa, 0xb3, 0x24, 0xe4, 0xfa, 0xd6, 0x75, 0x94, 0x55, 0x85, 0x80, 0x8b, 0x48, 0x31, 0xd7, 0xbc,
    0x3f, 0xf4, 0xde, 0xf0, 0x8e, 0x4b, 0x7a, 0x9d, 0xe5, 0x76, 0xd2, 0x65, 0x86, 0xce, 0xc6, 0x4b,
    0x61, 0x16
};

static const uint8_t TAG[] = {
    0x1a, 0xe1, 0x0b, 0x59,
    0x4f, 0x09, 0xe2, 0x6a,
    0x7e, 0x90, 0x2e, 0xcb,
    0xd0, 0x60, 0x06, 0x91
};

psa_status_t example_aead_chacha20_poly1305_multipart(void)
{
    psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;

    /* KEY IMPORT ------------------------ */
    psa_key_id_t key_id = 0;
    psa_key_attributes_t attr = psa_key_attributes_init();
    psa_key_usage_t usage = PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT;
    psa_algorithm_t alg = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(PSA_ALG_CHACHA20_POLY1305);

    psa_set_key_algorithm(&attr, alg);
    psa_set_key_usage_flags(&attr, usage);
    psa_set_key_bits(&attr, PSA_BYTES_TO_BITS(sizeof(KEY_CHACHA20)));
    psa_set_key_type(&attr, PSA_KEY_TYPE_CHACHA20);

    status = psa_import_key(&attr, KEY_CHACHA20, sizeof(KEY_CHACHA20), &key_id);
    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        printf("Import Key Error: %s\n", psa_status_to_humanly_readable(status));
        return status;
    }
    /* KEY IMPORT ------------------------ */

    /* ENCRYPT & TAG-GEN ----------------- */
    /* Note: Since the PSA AEAD Multipart Operation seperates ciphertext and tag,
     * we calculate the ciphertext size with the related cipher macro. */
    const size_t ciphertext_size = PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_CHACHA20,
                                                                  alg,
                                                                  sizeof(PLAINTEXT));
    const size_t tag_size = PSA_AEAD_TAG_LENGTH(PSA_KEY_TYPE_CHACHA20,
                                                PSA_BYTES_TO_BITS(sizeof(KEY_CHACHA20)),
                                                alg);
    uint8_t ciphertext[ciphertext_size];
    uint8_t tag[tag_size];

    psa_aead_operation_t operation;
    operation = psa_aead_operation_init();

    status = psa_aead_encrypt_setup(&operation, key_id, alg);
    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        printf("Encrypt Setup Error: %s\n", psa_status_to_humanly_readable(status));
        return status;
    }

    status = psa_aead_set_nonce(&operation, NONCE, sizeof(NONCE));
    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        printf("Encrypt Set Nonce Error: %s\n", psa_status_to_humanly_readable(status));
        return status;
    }

    size_t chunk_length = 5;
    size_t bytes_in = 0;
    while (bytes_in < sizeof(ADDITIONAL_DATA)) {
        chunk_length = (sizeof(ADDITIONAL_DATA) - bytes_in) < chunk_length ? (sizeof(ADDITIONAL_DATA) - bytes_in) : chunk_length;

        status = psa_aead_update_ad(&operation,
                                    &ADDITIONAL_DATA[bytes_in],
                                    chunk_length);
        if (status != PSA_SUCCESS) {
            psa_destroy_key(key_id);
            printf("Encrypt Update Ad Error: %s\n", psa_status_to_humanly_readable(status));
            return status;
        }

        bytes_in += chunk_length;
    }

    chunk_length = 35;
    bytes_in = 0;
    size_t bytes_out = 0;
    while (bytes_in < sizeof(PLAINTEXT)) {
        chunk_length = (sizeof(PLAINTEXT) - bytes_in) < chunk_length ? (sizeof(PLAINTEXT) - bytes_in) : chunk_length;

        size_t chunk_out_length;
        status = psa_aead_update(&operation,
                                 &PLAINTEXT[bytes_in],
                                 chunk_length,
                                 &ciphertext[bytes_out],
                                 ciphertext_size - bytes_out,
                                 &chunk_out_length);

        if (status != PSA_SUCCESS) {
            psa_destroy_key(key_id);
            printf("Encrypt Update Error: %s\n", psa_status_to_humanly_readable(status));
            return status;
        }

        bytes_in += chunk_length;
        bytes_out += chunk_out_length;
    }

    size_t finish_out_length;
    size_t tag_length;
    status = psa_aead_finish(&operation,
                             &ciphertext[bytes_out],
                             ciphertext_size - bytes_out,
                             &finish_out_length,
                             tag,
                             sizeof(tag),
                             &tag_length);
    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        printf("Encrypt Finish Error: %s\n", psa_status_to_humanly_readable(status));
        return status;
    }
    bytes_out += finish_out_length;

    if (memcmp(ciphertext, CIPHERTEXT, sizeof(CIPHERTEXT)) != 0) {
        psa_destroy_key(key_id);
        puts("Wrong Ciphertext on encryption");
        return PSA_ERROR_DATA_INVALID;
    }

    if (memcmp(tag, TAG, sizeof(TAG)) != 0) {
        psa_destroy_key(key_id);
        puts("Wrong Tag on encryption");
        return PSA_ERROR_DATA_INVALID;
    }
    /* ENCRYPT & TAG-GEN ----------------- */

    /* DECRYPT & VERIFY ------------------ */
    /* IoT-TODO: You can't use PSA_CIPHER_DECRYPT_OUTPUT_SIZE here, since
     * it subtracts the IV length. */
    const size_t plaintext_size = PSA_CIPHER_DECRYPT_OUTPUT_MAX_SIZE(sizeof(CIPHERTEXT));
    uint8_t plaintext[plaintext_size];

    operation = psa_aead_operation_init();

    status = psa_aead_decrypt_setup(&operation, key_id, alg);
    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        printf("Decrypt Setup Error: %s\n", psa_status_to_humanly_readable(status));
        return status;
    }

    status = psa_aead_set_nonce(&operation, NONCE, sizeof(NONCE));
    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        printf("Decrypt Set Nonce Error: %s\n", psa_status_to_humanly_readable(status));
        return status;
    }

    chunk_length = 5;
    bytes_in = 0;
    while (bytes_in < sizeof(ADDITIONAL_DATA)) {
        chunk_length = (sizeof(ADDITIONAL_DATA) - bytes_in) < chunk_length ? (sizeof(ADDITIONAL_DATA) - bytes_in) : chunk_length;

        status = psa_aead_update_ad(&operation,
                                    &ADDITIONAL_DATA[bytes_in],
                                    chunk_length);
        if (status != PSA_SUCCESS) {
            psa_destroy_key(key_id);
            printf("Decrypt Update Ad Error: %s\n", psa_status_to_humanly_readable(status));
            return status;
        }

        bytes_in += chunk_length;
    }

    chunk_length = 35;
    bytes_in = 0;
    bytes_out = 0;
    while (bytes_in < sizeof(CIPHERTEXT)) {
        chunk_length = (sizeof(CIPHERTEXT) - bytes_in) < chunk_length ? (sizeof(CIPHERTEXT) - bytes_in) : chunk_length;

        size_t chunk_out_length;
        status = psa_aead_update(&operation,
                                 &CIPHERTEXT[bytes_in],
                                 chunk_length,
                                 &plaintext[bytes_out],
                                 plaintext_size - bytes_out,
                                 &chunk_out_length);

        if (status != PSA_SUCCESS) {
            psa_destroy_key(key_id);
            printf("Decrypt Update Error: %s\n", psa_status_to_humanly_readable(status));
            return status;
        }

        bytes_in += chunk_length;
        bytes_out += chunk_out_length;
    }

    status = psa_aead_verify(&operation,
                             &plaintext[bytes_out],
                             plaintext_size - bytes_out,
                             &finish_out_length,
                             TAG,
                             sizeof(TAG));
    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        printf("Decrypt Verify Error: %s\n", psa_status_to_humanly_readable(status));
        return status;
    }
    bytes_out += finish_out_length;

    if (memcmp(plaintext, PLAINTEXT, sizeof(PLAINTEXT)) != 0) {
        psa_destroy_key(key_id);
        puts("Wrong Plaintext on decryption");
        return PSA_ERROR_DATA_INVALID;
    }
    /* DECRYPT & VERIFY ------------------ */

    return PSA_SUCCESS;
}
