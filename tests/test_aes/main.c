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
#include "crypto/ciphers/aes.h"

uint8_t TEST_0_KEY[] = {
    0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
    0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF
};

uint8_t TEST_0_INP[] = {
    0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
    0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
};
uint8_t TEST_0_DEC[] = {
    0x37, 0x29, 0xa3, 0x6c, 0xaf, 0xe9, 0x84, 0xff,
    0x46, 0x22, 0x70, 0x42, 0xee, 0x24, 0x83, 0xf6
};

uint8_t TEST_1_KEY[] = {
    0x23, 0xA0, 0x18, 0x53, 0xFA, 0xB3, 0x89, 0x23,
    0x65, 0x89, 0x2A, 0xBC, 0x43, 0x99, 0xCC, 0x00
};

uint8_t TEST_1_INP[] = {
    0x11, 0x53, 0x81, 0xE2, 0x5F, 0x33, 0xE7, 0x41,
    0xBB, 0x12, 0x67, 0x38, 0xE9, 0x12, 0x54, 0x23
};
uint8_t TEST_1_DEC[] = {
    0xD7, 0x9A, 0x54, 0x0E, 0x61, 0x33, 0x03, 0x72,
    0x59, 0x0f, 0x87, 0x91, 0xEF, 0xB0, 0xF8, 0x16
};

uint8_t data_encrypted[64], data_decrypted[64];

void print_block(char *label, uint8_t *block)
{
    printf("%s\t", label);
    for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        printf("%02x", block[i]);
    }
    printf("\n");
}

void aes_op(uint8_t *key, uint8_t *data, uint8_t *expected)
{
    cipher_context_t ctx;
    int err;

    aes_init(&ctx, AES_BLOCK_SIZE, key, AES_KEY_SIZE);
    err = aes_encrypt(&ctx, data, data_encrypted);
    if (err < 0) {
        printf("ERROR: encryption error (code: %d)", err);
        return;
    }

    err = aes_decrypt(&ctx, data_encrypted, data_decrypted);
    if (err < 0) {
        printf("ERROR: decryption error (code: %d)", err);
        return;
    }


    printf("\n");
    print_block("Input    ", data);
    print_block("Encrypted", data_encrypted);
    print_block("Expected ", expected);
    print_block("Decrypted", data_decrypted);
    printf("\n");
}

int main(void)
{
    puts("Start.");

    aes_op(TEST_0_KEY, TEST_0_INP, TEST_0_DEC);
    aes_op(TEST_1_KEY, TEST_1_INP, TEST_1_DEC);

    puts("Done.");
    return 0;
}
