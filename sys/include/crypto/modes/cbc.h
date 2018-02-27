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

#include "crypto/ciphers.h"
#include "crypto/padding.h"

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
 * @return           The length of the encrypted data or a (negative) error code
 *
 */
int cipher_encrypt_cbc(cipher_t * cipher, uint8_t iv[16], const uint8_t * input,
                       size_t input_len, uint8_t * output);

/**
 * @brief Encrypt data of arbitrary length in cipher block chaining mode and pad the data.
 *
 * @param cipher        Already initialized cipher struct
 * @param iv            16 octet initialization vector. Must never be used more
 *                      than once for a given key.
 * @param input         pointer to input data to encrypt
 * @param input_len     length of the input data
 * @param output        pointer to allocated memory for encrypted data. It has to
 *                      be of size data_len + BLOCK_SIZE - data_len % BLOCK_SIZE.
 * @param padding_type  Specify the type of padding to be used (as defined in padding.h)
 *
 * @return              The length of the encrypted data or a (negative) error code
 *
 */
int cipher_encrypt_cbc_with_padding(cipher_t * cipher, uint8_t iv[16], const uint8_t * input,
                                    size_t input_len, uint8_t * output, uint8_t padding_type);


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
 * @return           The length of the decrypted data or a (negative) error code
 *
 */
int cipher_decrypt_cbc(cipher_t * cipher, uint8_t iv[16], const uint8_t * input,
                       size_t input_len, uint8_t * output);

/**
 * @brief Decrypt an unpad encrypted data in cipher block chaining mode.
 *
 * @param cipher     Already initialized cipher struct
 * @param iv         16 octet initialization vector.
 * @param input      pointer to input data to decrypt
 * @param input_len  length of the input data
 * @param output     pointer to allocated memory for plaintext data. It has to
 *                   be of size input_len.
 * @param padding_type  Specify the type of padding to be used (as defined in padding.h)
 *
 * @return           The length of the decrypted data or a (negative) error code
 *
 */
int cipher_decrypt_cbc_with_padding(cipher_t * cipher, uint8_t iv[16], const uint8_t * input,
                                    size_t input_len, uint8_t * output, uint8_t padding_type);

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_MODES_CBC_H */
