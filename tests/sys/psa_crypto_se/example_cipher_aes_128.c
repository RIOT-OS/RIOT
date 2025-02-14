/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @brief       Tests the PSA secure element configurations
 *              Contents have been copied from `examples/advanced_examples/psa_crypto`
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "psa/crypto.h"

#define AES_128_KEY_SIZE    (16)
#define AES_256_KEY_SIZE    (32)

static const uint8_t KEY_128[] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

static uint8_t PLAINTEXT[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51
};
static uint8_t PLAINTEXT_LEN = 32;

/**
 * @brief   Example function to perform an AES-128 CBC encryption and decryption
 *          with the PSA Crypto API.
 *
 * @return  psa_status_t
 */
psa_status_t example_cipher_aes_128(void)
{
    psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;
    psa_key_id_t key_id = 0;
    psa_key_attributes_t attr = psa_key_attributes_init();
    psa_key_usage_t usage = PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT;

    size_t encr_output_size = PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_AES,
                                                             PSA_ALG_CBC_NO_PADDING, PLAINTEXT_LEN);

    uint8_t cipher_out[encr_output_size];
    uint8_t plain_out[sizeof(PLAINTEXT)];
    size_t output_len = 0;

    psa_set_key_algorithm(&attr, PSA_ALG_CBC_NO_PADDING);
    psa_set_key_usage_flags(&attr, usage);
    psa_set_key_bits(&attr, 128);
    psa_set_key_type(&attr, PSA_KEY_TYPE_AES);

    psa_key_lifetime_t lifetime = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION
                                      (PSA_KEY_LIFETIME_VOLATILE, PSA_ATCA_LOCATION_DEV0);
    psa_set_key_lifetime(&attr, lifetime);

    status = psa_import_key(&attr, KEY_128, AES_128_KEY_SIZE, &key_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_cipher_encrypt(key_id, PSA_ALG_CBC_NO_PADDING, PLAINTEXT,
                                PLAINTEXT_LEN, cipher_out, encr_output_size, &output_len);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_cipher_decrypt(key_id, PSA_ALG_CBC_NO_PADDING, cipher_out,
                                sizeof(cipher_out), plain_out, sizeof(plain_out), &output_len);
    if (status == PSA_SUCCESS) {
        return (memcmp(PLAINTEXT, plain_out, sizeof(plain_out)) ? -1 : 0);
    }
    return status;
}
