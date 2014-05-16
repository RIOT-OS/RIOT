/*
 * Copyright (C) 2013 Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief AES-128 test application
 *
 * @author Nico von Geyso <nico.geyso@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "kernel.h"
#include "crypto/ciphers.h"
#include "crypto/modes/ccm.h"

// PACKET VECTOR #1 (RFC 3610 - Page 10)
uint8_t TEST_1_KEY[] = {
    0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
    0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF
};
uint8_t TEST_1_KEY_LEN = 16;

uint8_t TEST_1_NONCE[] = {
    0x00,0x00,0x00,0x03,0x02,0x01,0x00,0xA0,
    0xA1,0xA2,0xA3,0xA4,0xA5
};
uint8_t TEST_1_NONCE_LEN = 13;

uint8_t TEST_1_INPUT[] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,   // additional auth data
    0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,   // input
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,   // input
    0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E         // input
};
uint8_t TEST_1_INPUT_LEN = 23;
uint8_t TEST_1_ADATA_LEN = 8;

uint8_t TEST_1_EXPECTED[] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07, // additional auth data
    0x58,0x8C,0x97,0x9A,0x61,0xC6,0x63,0xD2,
    0xF0,0x66,0xD0,0xC2,0xC0,0xF9,0x89,0x80,
    0x6D,0x5F,0x6B,0x61,0xDA,0xC3,0x84,0x17,
    0xE8,0xD1,0x2C,0xFD,0xF9,0x26,0xE0
};
uint8_t TEST_1_EXPECTED_LEN = 39;

// PACKET VECTOR #2 (RFC 3610 - Page 10)
uint8_t TEST_2_KEY[] = {
    0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
    0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF
};
uint8_t TEST_2_KEY_LEN = 16;

uint8_t TEST_2_NONCE[] = {
    0x00,0x00,0x00,0x04,0x03,0x02,0x01,0xA0,
    0xA1,0xA2,0xA3,0xA4,0xA5
};
uint8_t TEST_2_NONCE_LEN = 13;

uint8_t TEST_2_INPUT[] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
    0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F
};
uint8_t TEST_2_INPUT_LEN = 24;
uint8_t TEST_2_ADATA_LEN = 8;

uint8_t TEST_2_EXPECTED[] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x72,0xC9,0x1A,0x36,0xE1,0x35,0xF8,0xCF,
    0x29,0x1C,0xA8,0x94,0x08,0x5C,0x87,0xE3,
    0xCC,0x15,0xC4,0x39,0xC9,0xE4,0x3A,0x3B,
    0xA0,0x91,0xD5,0x6E,0x10,0x40,0x09,0x16
};
uint8_t TEST_2_EXPECTED_LEN = 40;

uint8_t data_encrypted[64], data_decrypted[64];

void print_block(char *label, uint8_t *block, uint8_t len)
{
    printf("%s", label);
    for (int i = 0; i < len; i++) {
        if (i%16 == 0)
            printf("\n\t");

        printf("%02x", block[i]);
    }
    printf("\n\n");
}

void ccm_op(uint8_t *key, uint8_t key_len, uint8_t *adata, uint8_t adata_len,
        uint8_t *nonce, uint8_t nonce_len, uint8_t *plain, uint8_t plain_len,
        uint8_t *output_expected, uint8_t output_expected_len)
{
    cipher_t cipher;
    int len_encrypted, len_decrypted;

    cipher_init(&cipher, CIPHER_AES_128, key, key_len);
    len_encrypted = cipher_encrypt_ccm(&cipher, adata, adata_len, 8, 2, nonce,
            nonce_len, plain, plain_len, data_encrypted);
    if (len_encrypted < 0) {
        printf("ERROR: encryption error (code: %d)", len_encrypted);
        return;
    }

    len_decrypted = cipher_decrypt_ccm(&cipher, adata, adata_len, 8, 2, nonce,
            nonce_len, data_encrypted, len_encrypted, data_decrypted);
    if (len_decrypted < 0) {
        printf("ERROR: decryption error (code: %d)", len_decrypted);
        return;
    }


    printf("\n");
    print_block("Input", plain, plain_len);
    print_block("Encrypted", data_encrypted, len_encrypted);
    print_block("Expected", output_expected, output_expected_len);
    print_block("Decrypted", data_decrypted, len_decrypted);
    printf("\n");
}

int main(void)
{
    puts("Start.");

    ccm_op(TEST_1_KEY, TEST_1_KEY_LEN, TEST_1_INPUT, TEST_1_ADATA_LEN,
            TEST_1_NONCE, TEST_1_NONCE_LEN, TEST_1_INPUT+TEST_1_ADATA_LEN,
            TEST_1_INPUT_LEN, TEST_1_EXPECTED+TEST_1_ADATA_LEN,
            TEST_1_EXPECTED_LEN-TEST_1_ADATA_LEN);

    ccm_op(TEST_2_KEY, TEST_2_KEY_LEN, TEST_2_INPUT, TEST_2_ADATA_LEN,
            TEST_2_NONCE, TEST_2_NONCE_LEN, TEST_2_INPUT+TEST_2_ADATA_LEN,
            TEST_2_INPUT_LEN, TEST_2_EXPECTED+TEST_2_ADATA_LEN,
            TEST_2_EXPECTED_LEN-TEST_2_ADATA_LEN);

    puts("Done.");
    return 0;
}
