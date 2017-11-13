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
 * @brief       Crypto mode - cipher block chaining
 *
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 *
 * @}
 */

#include <string.h>
#include "crypto/modes/cbc.h"

int cipher_encrypt_cbc(cipher_t *cipher, uint8_t iv[16],
                       const uint8_t *input, size_t length, uint8_t *output)
{
    size_t offset = 0;
    uint8_t block_size, input_block[CIPHER_MAX_BLOCK_SIZE] = { 0 },
    *output_block_last;

    block_size = cipher_get_block_size(cipher);
    if (length % block_size != 0) {
        return CIPHER_ERR_INVALID_LENGTH;
    }

    if (length == 0) {
        /* no plaintext, no operation */
        return 0;
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

int cipher_encrypt_cbc_with_padding(cipher_t *cipher, uint8_t iv[16],
                                    const uint8_t *input, size_t length, uint8_t *output, uint8_t padding_type)
{
    if (padding_type == PADDING_TYPE_PKCS7) {
        /* Encrypt the bulk of data without the last block */
        size_t last_block_offset = length - (length % cipher->interface->block_size);
        if (last_block_offset == length) {
            /* input data is a multiple of the blocksize -> we need to append a whole block */
            last_block_offset -= cipher->interface->block_size;
        }

        int encrypted_bytes = cipher_encrypt_cbc(cipher, iv, input, last_block_offset, output);
        if (encrypted_bytes < 0) {
            return encrypted_bytes;
        }
        /* Move the iv to the last block of the encrypted data */
        if (encrypted_bytes > 0) {
            iv = output + last_block_offset - cipher->interface->block_size;
        }
        /* Pad the final block and encrypt it */
        uint8_t padded_data[length - last_block_offset + cipher->interface->block_size];
        int32_t padded_length = pkcs7_padding(input + last_block_offset, length - last_block_offset, cipher->interface->block_size, padded_data, sizeof(padded_data));
        if (padded_length < 0) {
            return CIPHER_ERR_PADDING_ERROR;
        }

        int encrypted_bytes_last_block = cipher_encrypt_cbc(cipher, iv, padded_data, padded_length, output + last_block_offset);
        if (encrypted_bytes_last_block < 0) {
            return encrypted_bytes;
        }

        encrypted_bytes += encrypted_bytes_last_block;
        return encrypted_bytes;
    }
    else {
        return CIPHER_ERR_UNKNOWN_PADDING;
    }
}


int cipher_decrypt_cbc(cipher_t *cipher, uint8_t iv[16],
                       const uint8_t *input, size_t length, uint8_t *output)
{
    size_t offset = 0;
    const uint8_t *input_block, *input_block_last;
    uint8_t block_size;


    block_size = cipher_get_block_size(cipher);
    if (length % block_size != 0) {
        return CIPHER_ERR_INVALID_LENGTH;
    }

    if (length == 0) {
        /* no plaintext, no operation */
        return 0;
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

int cipher_decrypt_cbc_with_padding(cipher_t *cipher, uint8_t iv[16],
                                    const uint8_t *input, size_t length, uint8_t *output, uint8_t padding_type)
{
    if (padding_type == PADDING_TYPE_PKCS7) {
        int decrypted_len = cipher_decrypt_cbc(cipher, iv, input, length, output);
        if (decrypted_len < 0) {
            return decrypted_len;
        }
        /* Decryption worked. Now remove the padding. */
        int32_t unpadded_length = pkcs7_padding_remove(output, length, cipher->interface->block_size);
        if (unpadded_length < 0) {
            return CIPHER_ERR_PADDING_ERROR;
        }
        return unpadded_length;
    }
    else {
        return CIPHER_ERR_UNKNOWN_PADDING;
    }
}
