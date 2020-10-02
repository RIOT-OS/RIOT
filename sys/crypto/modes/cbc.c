/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file
 * @brief       Crypto mode - cipher block chaining
 *
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 *
 * @}
 */


#include <string.h>
#include "crypto/modes/cbc.h"

int cipher_encrypt_cbc(const cipher_t *cipher, uint8_t iv[16],
                       const uint8_t *input, size_t length, uint8_t *output)
{
    size_t offset = 0;
    uint8_t block_size, input_block[CIPHER_MAX_BLOCK_SIZE] = { 0 },
            *output_block_last;

    block_size = cipher_get_block_size(cipher);
    if (length % block_size != 0) {
        return CIPHER_ERR_INVALID_LENGTH;
    }

    output_block_last = iv;
    do {
        /* CBC-Mode: XOR plaintext with ciphertext of (n-1)-th block */
        memcpy(input_block, input + offset, block_size);
        for (int i = 0; i < block_size; ++i) {
            input_block[i] ^= output_block_last[i];
        }

        if (cipher_encrypt(cipher, input_block, output + offset) != 1) {
            return CIPHER_ERR_ENC_FAILED;
        }

        output_block_last = output + offset;
        offset += block_size;
    } while (offset < length);

    return offset;
}


int cipher_decrypt_cbc(const cipher_t *cipher, uint8_t iv[16],
                       const uint8_t *input, size_t length, uint8_t *output)
{
    size_t offset = 0;
    const uint8_t *input_block, *input_block_last;
    uint8_t block_size;


    block_size = cipher_get_block_size(cipher);
    if (length % block_size != 0) {
        return CIPHER_ERR_INVALID_LENGTH;
    }

    input_block_last = iv;
    do {
        input_block = input + offset;
        uint8_t *output_block = output + offset;

        if (cipher_decrypt(cipher, input_block, output_block) != 1) {
            return CIPHER_ERR_DEC_FAILED;
        }

        /* CBC-Mode: XOR plaintext with ciphertext of (n-1)-th block */
        for (uint8_t i = 0; i < block_size; ++i) {
            output_block[i] ^= input_block_last[i];
        }

        input_block_last = input_block;
        offset += block_size;
    } while (offset < length);

    return offset;
}
