/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @brief       Tests the PSA aead configurations
 *
 * @author      Lukas Luger <lukas.luger@mailbox.tu-dresden.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include "od.h"
#include "psa/crypto.h"

#define AES_128_KEY_SIZE (16)

static const uint8_t KEY_128[] = {
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f
};
/* certain PSA backends require the data to be in RAM rather than ROM
 * so these values cannot be `const` */
static uint8_t NONCE[] = {
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16
};

static uint8_t ADDITIONAL_DATA[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};

static uint8_t PLAINTEXT[] = {
    0x20, 0x21, 0x22, 0x23
};

static uint8_t CIPHERTEXT[] = {
    0x71, 0x62, 0x01, 0x5b, 0x4d, 0xac, 0x25, 0x5d
};

/**
 * @brief   Example function to perform an AES-128 CCM encryption and decryption
 *          with the PSA Crypto API.
 *
 * @return  psa_status_t
 */
psa_status_t example_aead_aes_128_ccm(void)
{
    psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;
    psa_key_id_t key_id = 0;
    psa_key_attributes_t attr = psa_key_attributes_init();
    psa_key_usage_t usage = PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT;
    psa_algorithm_t algo = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4);
    size_t encr_output_size = PSA_AEAD_ENCRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_AES,
                                                           algo, sizeof(PLAINTEXT));

    uint8_t cipher_out[encr_output_size];
    uint8_t plain_out[sizeof(PLAINTEXT)];
    size_t output_len = 0;

    psa_set_key_algorithm(&attr, algo);
    psa_set_key_usage_flags(&attr, usage);
    psa_set_key_bits(&attr, 128);
    psa_set_key_type(&attr, PSA_KEY_TYPE_AES);

    status = psa_import_key(&attr, KEY_128, AES_128_KEY_SIZE, &key_id);
    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        return status;
    }

    status = psa_aead_encrypt(key_id, algo, NONCE, sizeof(NONCE), ADDITIONAL_DATA, sizeof(ADDITIONAL_DATA),
                              PLAINTEXT, sizeof(PLAINTEXT), cipher_out, encr_output_size, &output_len);
    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        return status;
    }

    if (memcmp(CIPHERTEXT, cipher_out, sizeof(CIPHERTEXT))) {
        printf("AES 128 CCM: wrong ciphertext on encryption\n");
        psa_destroy_key(key_id);
        return -1;
    }

    status = psa_aead_decrypt(key_id, algo, NONCE, sizeof(NONCE), ADDITIONAL_DATA, sizeof(ADDITIONAL_DATA),
                              cipher_out, sizeof(cipher_out), plain_out, sizeof(plain_out),
                              &output_len);
    psa_destroy_key(key_id);
    if (status == PSA_SUCCESS && memcmp(PLAINTEXT, plain_out, sizeof(plain_out))) {
        printf("AES 128 CCM: wrong plaintext on decryption\n");
        return -1;
    }
    return status;
}
