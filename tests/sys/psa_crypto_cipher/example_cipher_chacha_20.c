/*
 * Copyright (C) 2024 TU-Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @brief       Tests the PSA cipher configurations
 *              Contents have been copied from `examples/psa_crypto`
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "psa/crypto.h"

void tohex(const unsigned char * in, size_t insz, char * out, size_t outsz)
{
    const unsigned char * pin = in;
    const char * hex = "0123456789ABCDEF";
    char * pout = out;
    for(; pin < in+insz; pout +=3, pin++){
        pout[0] = hex[(*pin>>4) & 0xF];
        pout[1] = hex[ *pin     & 0xF];
        pout[2] = ':';
        if (pout + 3 - out > (int) outsz){
            /* Better to truncate output string than overflow buffer */
            /* it would be still better to either return a status */
            /* or ensure the target buffer is large enough and it never happen */
            break;
        }
    }
    pout[-1] = 0;
}

static const uint8_t KEY_CHACHA20[] = {
    0x90, 0x6f, 0xdc, 0xf1, 0x72, 0xe6, 0x8a, 0xd1, 0xbb, 0xd0, 0xa3, 0x24,
    0x2a, 0xda, 0x91, 0xdb, 0x3a, 0x8d, 0xb8, 0xd4, 0x9a, 0x75, 0xc7, 0x14, 
    0x00, 0x08, 0x9a, 0x8b, 0x86, 0x55, 0x2e, 0x9a
};

static const uint8_t PLAINTEXT[] = {
    0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x52, 0x49, 0x4F, 0x54, 0x21, 0x20, 
    0x54, 0x68, 0x65, 0x20, 0x41, 0x6E, 0x73, 0x77, 0x65, 0x72, 0x20, 0x69,
    0x73, 0x20, 0x34, 0x32, 0x2E
};

/**
 * @brief   Example function to perform an CHACHA20 encryption and decryption
 *          with the PSA Crypto API.
 *
 * @return  psa_status_t
 */
psa_status_t example_cipher_chacha_20(void)
{
    psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;
    psa_key_id_t key_id = 0;
    psa_key_attributes_t attr = psa_key_attributes_init();
    psa_key_usage_t usage = PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT;
    size_t encr_output_size = PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_CHACHA20,
                                                             PSA_ALG_STREAM_CIPHER, sizeof(PLAINTEXT));

    char plain_hex[sizeof(PLAINTEXT) * 3];
    char cipher_hex[encr_output_size * 3];
    uint8_t cipher_out[encr_output_size];
    uint8_t plain_out[sizeof(PLAINTEXT)];
    size_t output_len = 0;

    psa_set_key_algorithm(&attr, PSA_ALG_STREAM_CIPHER);
    psa_set_key_usage_flags(&attr, usage);
    psa_set_key_bits(&attr, 256);
    psa_set_key_type(&attr, PSA_KEY_TYPE_CHACHA20);

    status = psa_import_key(&attr, KEY_CHACHA20, sizeof(KEY_CHACHA20), &key_id);
    if (status != PSA_SUCCESS) {
        return status;
    }
    tohex(PLAINTEXT, sizeof(PLAINTEXT), plain_hex, sizeof(plain_hex));
    printf("Plain before: %s / %.*s (%u)\n", plain_hex, sizeof(PLAINTEXT), PLAINTEXT,(sizeof(plain_hex) / 3));
    status = psa_cipher_encrypt(key_id, PSA_ALG_STREAM_CIPHER, PLAINTEXT,
                                sizeof(PLAINTEXT), cipher_out, encr_output_size, &output_len);
    if (status != PSA_SUCCESS) {
        return status;
    }
    tohex(cipher_out, output_len, cipher_hex, sizeof(cipher_hex));
    printf("Cipher      : %s (%u)\n", cipher_hex, (sizeof(cipher_hex)/3));
    status = psa_cipher_decrypt(key_id, PSA_ALG_STREAM_CIPHER, cipher_out,
                                sizeof(cipher_out), plain_out, sizeof(plain_out), &output_len);
    tohex(plain_out, output_len, plain_hex, sizeof(plain_hex));
    printf("Plain after : %s (%u)\n", plain_hex, (sizeof(plain_hex) / 3));
    if (status == PSA_SUCCESS) {
        return (memcmp(PLAINTEXT, plain_out, sizeof(plain_out)) ? -1 : 0);
    }
    status = psa_destroy_key(key_id);
    return status;
}
