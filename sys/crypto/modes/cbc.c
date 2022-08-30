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
 * @author      Mathias Tausig
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include <string.h>
#include "crypto/modes/cbc.h"

int cipher_encrypt_cbc(const cipher_t *cipher, uint8_t iv[16],
                       const uint8_t *input, size_t input_len, uint8_t *output)
{
    size_t offset = 0;
    uint8_t block_size, input_block[CIPHER_MAX_BLOCK_SIZE] = { 0 },
    *output_block_last;

    if (input_len == 0) {
        /* no plaintext, no operation */
        return 0;
    }

    block_size = cipher_get_block_size(cipher);
    if (input_len % block_size != 0) {
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
    } while (offset < input_len);

    return offset;
}

int cipher_encrypt_cbc_pkcs7(const cipher_t *cipher, uint8_t iv[16],
                             const void *input, size_t input_len,
                             void *output, size_t output_buffer_size)
{
    uint8_t block_size = cipher->interface->block_size;

    /*
     * Output buffer size must be a multiple of block_size and large enough
     * for the cipher text including the paddíng
     */
    if (output_buffer_size < (input_len + block_size - (input_len % block_size))) {
        return CIPHER_ERR_BUFFER_TOO_SMALL;
    }

    /* Encrypt the bulk of data without the last block */
    size_t padding_len_required = block_size - (input_len % block_size);
    size_t last_block_offset = input_len - (input_len % block_size);

    int encrypted_bytes = cipher_encrypt_cbc(cipher, iv, (const uint8_t *)input,
                                             last_block_offset, (uint8_t *)output);
    if (encrypted_bytes < 0) {
        return encrypted_bytes;
    }

    /* Move the iv to the last block of the encrypted data */
    if (encrypted_bytes > 0) {
        iv = (uint8_t *)output + last_block_offset - cipher->interface->block_size;
    }

    /* Pad the final block and encrypt it */
    uint8_t padded_data[CIPHER_MAX_BLOCK_SIZE];
    if (padding_len_required < block_size) {
        memcpy(padded_data, (const uint8_t *)input + last_block_offset,
               input_len - last_block_offset);
        memset(padded_data + (block_size - padding_len_required), padding_len_required, padding_len_required);
    }
    else {
        /* If input % block_size == 0, append a block of block_size */
        memset(padded_data, padding_len_required, sizeof(padded_data));
    }

    int encrypted_bytes_last_block = cipher_encrypt_cbc(cipher, iv, padded_data, block_size,
                                                        (uint8_t *)output + last_block_offset);
    if (encrypted_bytes_last_block < 0) {
        return encrypted_bytes;
    }

    encrypted_bytes += encrypted_bytes_last_block;
    return encrypted_bytes;
}

int cipher_decrypt_cbc(const cipher_t *cipher, uint8_t iv[16],
                       const uint8_t *input, size_t input_len, uint8_t *output)
{
    size_t offset = 0;
    const uint8_t *input_block, *input_block_last;
    uint8_t block_size;


    block_size = cipher_get_block_size(cipher);
    if (input_len % block_size != 0) {
        return CIPHER_ERR_INVALID_LENGTH;
    }

    if (input_len == 0) {
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
    } while (offset < input_len);

    return offset;
}

int cipher_decrypt_cbc_pkcs7(const cipher_t *cipher, uint8_t iv[16],
                             const void *input, size_t input_len,
                             void *output, size_t output_buffer_size)
{
    int decrypted_len = cipher_decrypt_cbc(cipher, iv, (const uint8_t *)input,
                                           input_len, (uint8_t *)output);
    if (decrypted_len < 0) {
        return decrypted_len;
    }

    /* Decryption worked. Now remove the padding. */
    uint8_t padding_len = ((uint8_t *)output)[output_buffer_size-1];
    /* Check if the padding is correct */
    for (uint8_t i = output_buffer_size - 1; i >= output_buffer_size - padding_len; --i) {
        if (((uint8_t *)output)[i] != padding_len) {
            return CIPHER_ERR_PADDING_ERROR;
        }
    }

    int32_t unpadded_length = output_buffer_size - padding_len;
    if (unpadded_length < 0) {
        return CIPHER_ERR_PADDING_ERROR;
    }
    return unpadded_length;
}
