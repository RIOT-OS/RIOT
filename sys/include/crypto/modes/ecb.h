/*
 * Copyright (C) 2014 Freie Universität Berlin, Computer Systems & Telematics
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file        ecb.h
 * @brief       Electronic code book mode of operation for block ciphers
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 */

#ifndef __CRYPTO_MODES_ECB_H_
#define __CRYPTO_MODES_ECB_H_

#include "kernel.h"
#include "crypto/ciphers.h"


/**
 * @brief Encrypt data of arbitrary length in ecb mode. Caution each identical
 *        block results in an identical encrypted block. Normally you do not
 *        want to use ECB.
 *
 *
 * @param cipher     Already initialized cipher struct
 * @param input      pointer to input data to encrypt
 * @param input_len  length of the input data
 * @param output     pointer to allocated memory for encrypted data. It has to
 *                   be of size data_len + BLOCK_SIZE - data_len % BLOCK_SIZE.
 */
int cipher_encrypt_ecb(cipher_t* cipher, uint8_t* input, size_t length,
                       uint8_t* output);


/**
 * @brief Decrypts data of arbitrary length in ecb mode.
 *
 *
 * @param cipher     Already initialized cipher struct
 * @param input      pointer to input data to decrypt
 * @param input_len  length of the input data
 * @param output     pointer to allocated memory for plaintext data. It has to
 *                   be of size input_len.
 */
int cipher_decrypt_ecb(cipher_t* cipher, uint8_t* input, size_t length,
                       uint8_t* output);

#endif /* __CRYPTO_MODES_ECB_H_*/
