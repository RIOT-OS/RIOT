/*
 * Copyright (C) 2014 Freie Universität Berlin, Computer Systems & Telematics
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/**
 * @defgroup    sys_crypto_modes modes
 * @ingroup     sys_crypto
 * @brief       collection of different block ciphers operation modes
 * @{
 *
 * @file        cbc.h
 * @brief       Cipher block chaining mode of operation for block ciphers
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 */

#ifndef __CRYPTO_MODES_CBC_H_
#define __CRYPTO_MODES_CBC_H_

#include "kernel.h"
#include "crypto/ciphers.h"

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
 */
int cipher_encrypt_cbc(cipher_t* cipher, uint8_t iv[16], uint8_t* input,
                       size_t input_len, uint8_t* output);


/**
 * @brief Decrypt encrypted data in cipher block chaining mode.
 *
 * @param cipher     Already initialized cipher struct
 * @param iv         16 octet initialization vector.
 * @param input      pointer to input data to decrypt
 * @param input_len  length of the input data
 * @param output     pointer to allocated memory for plaintext data. It has to
 *                   be of size input_len.
 */
int cipher_decrypt_cbc(cipher_t* cipher, uint8_t iv[16], uint8_t* input,
                       size_t input_len, uint8_t* output);

#endif /* __CRYPTO_MODES_CBC_H_ */
