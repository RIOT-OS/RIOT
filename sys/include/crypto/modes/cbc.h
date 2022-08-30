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
 * @file        cbc.h
 * @brief       Cipher block chaining mode of operation for block ciphers
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 */

#ifndef CRYPTO_MODES_CBC_H
#define CRYPTO_MODES_CBC_H

#include <stddef.h>
#include "crypto/ciphers.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Encrypt data of arbitrary length in cipher block chaining mode.
 *
 * @param cipher     Already initialized cipher struct
 * @param iv         16 octet initialization vector. Must never be used more
 *                   than once for a given key.
 * @param input      pointer to input data to encrypt
 * @param input_len  length of the input data
 * @param output     pointer to allocated memory for encrypted data. It has to
 *                   be of size data_len + BLOCK_SIZE - data_len % BLOCK_SIZE.
 *
 * @return            <0 on error
 * @return            CIPHER_ERR_INVALID_LENGTH when input_len % BLOCK_SIZE != 0
 * @return            CIPHER_ERR_ENC_FAILED on internal encryption error
 * @return            otherwise number of input bytes that aren't consumed
 */
int cipher_encrypt_cbc(const cipher_t *cipher, uint8_t iv[16], const uint8_t *input,
                       size_t input_len, uint8_t *output);

/**
 * @brief Decrypt encrypted data in cipher block chaining mode.
 *
 * @param cipher     Already initialized cipher struct
 * @param iv         16 octet initialization vector.
 * @param input      pointer to input data to decrypt
 * @param input_len  length of the input data
 * @param output     pointer to allocated memory for plaintext data. It has to
 *                   be of size input_len.
 *
 * @return            <0 on error
 * @return            CIPHER_ERR_INVALID_LENGTH when input_len % BLOCK_SIZE != 0
 * @return            CIPHER_ERR_DEC_FAILED on internal decryption error
 * @return            otherwise number of bytes decrypted
 */
int cipher_decrypt_cbc(const cipher_t *cipher, uint8_t iv[16], const uint8_t *input,
                       size_t input_len, uint8_t *output);

/**
 * @brief   Encrypt data of arbitrary length in cipher block chaining
 *          mode and pad the data.
 *
 * @param cipher        Already initialized cipher struct
 * @param iv            16 octet initialization vector. Must never be used more
 *                      than once for a given key.
 * @param input         Pointer to input data to encrypt
 * @param input_len     Length of the input data
 * @param output        Pointer to allocated memory for encrypted data.
 * @param output_buffer_size    Size of the output buffer in bytes. Must be at
 *                              least `data_len + BLOCK_SIZE - data_len % BLOCK_SIZE`.
 *
 * @return              <0 on error
 * @return              CIPHER_ERR_INVALID_LENGTH when input_len % BLOCK_SIZE != 0
 * @return              CIPHER_ERR_DEC_FAILED on internal decryption error
 * @return              CIPHER_ERR_PADDING_ERROR on internal padding error
 * @return              CIPHER_ERR_UNKNOWN_PADDING if the padding type is unknown
 * @return              otherwise number of bytes decrypted
 *
 */
int cipher_encrypt_cbc_pkcs7(const cipher_t * cipher, uint8_t iv[16],
                             const void * input, size_t input_len,
                             void * output, size_t output_buffer_size);

/**
 * @brief Decrypt an unpad encrypted data in cipher block chaining mode.
 *
 * @param cipher        Already initialized cipher struct
 * @param iv            16 octet initialization vector.
 * @param input         Pointer to input data to decrypt
 * @param input_len     Length of the input data
 * @param output        Pointer to allocated memory for plaintext data. It has to
 *                      be of size @p input_len.
 * @param output_buffer_size    Size of the output buffer in bytes.
 *
 * @return              <0 on error
 * @return              CIPHER_ERR_INVALID_LENGTH when input_len % BLOCK_SIZE != 0
 * @return              CIPHER_ERR_DEC_FAILED on internal decryption error
 * @return              CIPHER_ERR_PADDING_ERROR on internal padding error
 * @return              CIPHER_ERR_UNKNOWN_PADDING if the padding type is unknown
 * @return              otherwise number of bytes decrypted
 */
int cipher_decrypt_cbc_pkcs7(const cipher_t * cipher, uint8_t iv[16],
                             const void * input, size_t input_len,
                             void * output, size_t output_buffer_size);

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_MODES_CBC_H */
/** @} */
