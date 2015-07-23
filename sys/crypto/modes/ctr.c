/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
* @ingroup     sys_crypto_modes
* @{
*
* @file
* @brief       Crypto mode - Counter
*
* @author      Nico von Geyso <nico.geyso@fu-berlin.de>
*
* @}
*/

#include "crypto/helper.h"
#include "crypto/modes/ctr.h"

int cipher_encrypt_ctr(cipher_t* cipher, uint8_t nonce_counter[16],
                       uint8_t nonce_len, uint8_t* input, size_t length,
                       uint8_t* output)
{
    size_t offset = 0;
    uint8_t stream_block[16] = {0}, block_size;

    block_size = cipher_get_block_size(cipher);
    do {
        uint8_t block_size_input;

        if (cipher_encrypt(cipher, nonce_counter, stream_block) != 1) {
            return CIPHER_ERR_ENC_FAILED;
        }

        block_size_input = (length - offset > block_size) ?
                           block_size : length - offset;
        for (uint8_t i = 0; i < block_size_input; ++i) {
            output[offset + i] = stream_block[i] ^ input[offset + i];
        }

        offset += block_size_input;
        crypto_block_inc_ctr(nonce_counter, block_size - nonce_len);
    } while (offset < length);

    return offset;
}

int cipher_decrypt_ctr(cipher_t* cipher, uint8_t nonce_counter[16],
                       uint8_t nonce_len, uint8_t* input, size_t length,
                       uint8_t* output)
{
    return cipher_encrypt_ctr(cipher, nonce_counter, nonce_len, input,
                              length, output);
}
