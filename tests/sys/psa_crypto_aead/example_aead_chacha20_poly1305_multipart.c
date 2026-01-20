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

uint8_t PLAINTEXT[] = {
    0x4c, 0x61, 0x64, 0x69, 0x65, 0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x47, 0x65, 0x6e, 0x74, 0x6c,
    0x65, 0x6d, 0x65, 0x6e, 0x20, 0x6f, 0x66, 0x20, 0x74, 0x68, 0x65, 0x20, 0x63, 0x6c, 0x61, 0x73,
    0x73, 0x20, 0x6f, 0x66, 0x20, 0x27, 0x39, 0x39, 0x3a, 0x20, 0x49, 0x66, 0x20, 0x49, 0x20, 0x63,
    0x6f, 0x75, 0x6c, 0x64, 0x20, 0x6f, 0x66, 0x66, 0x65, 0x72, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x6f,
    0x6e, 0x6c, 0x79, 0x20, 0x6f, 0x6e, 0x65, 0x20, 0x74, 0x69, 0x70, 0x20, 0x66, 0x6f, 0x72, 0x20,
    0x74, 0x68, 0x65, 0x20, 0x66, 0x75, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x73, 0x75, 0x6e, 0x73,
    0x63, 0x72, 0x65, 0x65, 0x6e, 0x20, 0x77, 0x6f, 0x75, 0x6c, 0x64, 0x20, 0x62, 0x65, 0x20, 0x69,
    0x74, 0x2e
};

uint8_t ADDITIONAL_DATA[] = {
    0x50, 0x51, 0x52, 0x53,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7
};

uint8_t KEY_CHACHA20[] = {
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f
};

uint8_t NONCE[] = {
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x07, 0x00, 0x00, 0x00
};

uint8_t CIPHERTEXT[] = {
    0xd3, 0x1a, 0x8d, 0x34, 0x64, 0x8e, 0x60, 0xdb, 0x7b, 0x86, 0xaf, 0xbc, 0x53, 0xef, 0x7e, 0xc2,
    0xa4, 0xad, 0xed, 0x51, 0x29, 0x6e, 0x08, 0xfe, 0xa9, 0xe2, 0xb5, 0xa7, 0x36, 0xee, 0x62, 0xd6,
    0x3d, 0xbe, 0xa4, 0x5e, 0x8c, 0xa9, 0x67, 0x12, 0x82, 0xfa, 0xfb, 0x69, 0xda, 0x92, 0x72, 0x8b,
    0x1a, 0x71, 0xde, 0x0a, 0x9e, 0x06, 0x0b, 0x29, 0x05, 0xd6, 0xa5, 0xb6, 0x7e, 0xcd, 0x3b, 0x36,
    0x92, 0xdd, 0xbd, 0x7f, 0x2d, 0x77, 0x8b, 0x8c, 0x98, 0x03, 0xae, 0xe3, 0x28, 0x09, 0x1b, 0x58,
    0xfa, 0xb3, 0x24, 0xe4, 0xfa, 0xd6, 0x75, 0x94, 0x55, 0x85, 0x80, 0x8b, 0x48, 0x31, 0xd7, 0xbc,
    0x3f, 0xf4, 0xde, 0xf0, 0x8e, 0x4b, 0x7a, 0x9d, 0xe5, 0x76, 0xd2, 0x65, 0x86, 0xce, 0xc6, 0x4b,
    0x61, 0x16
};

uint8_t TAG[] = {
    0x1a, 0xe1, 0x0b, 0x59,
    0x4f, 0x09, 0xe2, 0x6a,
    0x7e, 0x90, 0x2e, 0xcb,
    0xd0, 0x60, 0x06, 0x91
};

psa_status_t example_aead_chacha20_poly1305_multipart(void)
{
    psa_aead_operation_t operation;
    operation = psa_aead_operation_init();
    psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;
    psa_key_id_t key_id = 0;
    psa_key_attributes_t attr = psa_key_attributes_init();
    psa_key_usage_t usage = PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT;
    psa_algorithm_t alg = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(PSA_ALG_CHACHA20_POLY1305);

    const size_t cipher_output_size = sizeof(CIPHERTEXT);
    //const size_t plain_output_size = sizeof(PLAINTEXT);

    uint8_t ciphertext_out[cipher_output_size];
    //uint8_t plain_out[plain_output_size];

    psa_set_key_algorithm(&attr, alg);
    psa_set_key_usage_flags(&attr, usage);
    psa_set_key_bits(&attr, 256);
    psa_set_key_type(&attr, PSA_KEY_TYPE_CHACHA20);

    status = psa_import_key(&attr, KEY_CHACHA20, sizeof KEY_CHACHA20, &key_id);
    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        printf("Import Key Error: %s", psa_status_to_humanly_readable(status));
        return status;
    }

    status = psa_aead_encrypt_setup(&operation, key_id, alg);
    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        printf("Encrypt Setup Error: %s", psa_status_to_humanly_readable(status));
        return status;
    }

    status = psa_aead_set_nonce(&operation, NONCE, sizeof NONCE);
    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        printf("Encrypt Set Nonce Error: %s", psa_status_to_humanly_readable(status));
        return status;
    }

    size_t max_encrypt_size = sizeof(PLAINTEXT);
    uint16_t processed_bytes = 0;
    uint8_t input_length = 35;
    uint16_t calculated_output = 0;
    while (processed_bytes < max_encrypt_size) {
        if ((sizeof(PLAINTEXT) - processed_bytes) < 35) {
            input_length = sizeof(PLAINTEXT) - processed_bytes;
        }
        size_t new_output_length = 0;
        status = psa_aead_update(&operation,
                                 &PLAINTEXT[processed_bytes],
                                 input_length,
                                 &ciphertext_out[calculated_output],
                                 (cipher_output_size - calculated_output),
                                 &new_output_length);
        if (status != PSA_SUCCESS) {
            psa_destroy_key(key_id);
            printf("Encrypt Update Error: %s", psa_status_to_humanly_readable(status));
            return status;
        }
        calculated_output += new_output_length;
        processed_bytes += input_length;
    }

    size_t finish_output_length = 0;
    uint8_t calculated_tag[16];
    size_t calculated_tag_length;
    status = psa_aead_finish(&operation,
                             &ciphertext_out[calculated_output],
                             (cipher_output_size - calculated_output),
                             &finish_output_length,
                             calculated_tag,
                             sizeof(calculated_tag),
                             &calculated_tag_length);
    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        printf("Encrypt Finish Error: %s", psa_status_to_humanly_readable(status));
        return status;
    }
    calculated_output += finish_output_length;

    if (memcmp(ciphertext_out, CIPHERTEXT, sizeof(CIPHERTEXT)) != 0) {
        puts("Wrong Ciphertext on encryption");
        return PSA_ERROR_DATA_INVALID;
    }

    if (memcmp(calculated_tag, TAG, sizeof(TAG)) != 0) {
        puts("Wrong Tag on encryption");
        return PSA_ERROR_DATA_INVALID;
    }

    return PSA_SUCCESS;
}
