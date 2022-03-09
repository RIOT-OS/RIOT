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
 * @brief       Crypto mode - electronic code book
 *
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 *
 * @}
 */

#include <stddef.h>
#include <stdint.h>

#include "crypto/modes/ecb.h"

int cipher_encrypt_ecb(const cipher_t *cipher, const uint8_t *input,
                       size_t length, uint8_t *output)
{
    size_t offset;
    uint8_t block_size;

    block_size = cipher_get_block_size(cipher);
    if (length % block_size != 0) {
        return CIPHER_ERR_INVALID_LENGTH;
    }

    offset = 0;
    do {
        if (cipher_encrypt(cipher, input + offset, output + offset) != 1) {
            return CIPHER_ERR_ENC_FAILED;
        }

        offset += block_size;
    } while (offset < length);

    return offset;
}

int cipher_decrypt_ecb(const cipher_t *cipher, const uint8_t *input,
                       size_t length, uint8_t *output)
{
    size_t offset = 0;
    uint8_t block_size;

    block_size = cipher_get_block_size(cipher);
    if (length % block_size != 0) {
        return CIPHER_ERR_INVALID_LENGTH;
    }

    do {
        if (cipher_decrypt(cipher, input + offset, output + offset) != 1) {
            return CIPHER_ERR_DEC_FAILED;
        }

        offset += block_size;
    } while (offset < length);

    return offset;
}
