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
 * @file        ctr.h
 * @brief       Counter mode of operation for block ciphers
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 */

#ifndef __CRYPTO_MODES_CTR_H_
#define __CRYPTO_MODES_CTR_H_

#include "kernel.h"
#include "crypto/ciphers.h"

/**
 * @brief Encrypt data of arbitrary length in counter mode.
 *
 * @param cipher        Already initialized cipher struct
 * @param nonce_counter A nounce and a counter encoded in 16 octets. The counter
 *                      will be modified in each block encryption.
 * @param nonce_len     Length of the nonce in octets. As nounce and counter
 *                      have to fit in one aligned 16 octet block, maximum
 *                      length of nonce is limited by input_len:
 *                      16 - log_2(input_len)
 * @param input         pointer to input data to encrypt
 * @param input_len     length of the input data
 * @param output        pointer to allocated memory for encrypted data. It has
 *                      to be of size data_len.
 */
int cipher_encrypt_ctr(cipher_t* cipher, uint8_t nonce_counter[16],
                       uint8_t nonce_len, uint8_t* input, size_t length,
                       uint8_t* output);


/**
 * @brief Decrypt data of arbitrary length in counter mode. Encryption and
 *        decryption in ctr mode are basically the same.
 *
 * @param cipher        Already initialized cipher struct
 * @param nonce_counter A nounce and a counter encoded in 16 octets. The counter
 *                      will be modified in each block encryption.
 * @param nonce_len     Length of the nonce in octets. As nounce and counter
 *                      have to fit in one aligned 16 octet block, maximum
 *                      length of nonce is limited by input_len:
 *                      16 - log_2(input_len)
 * @param input         pointer to input data to encrypt
 * @param input_len     length of the input data
 * @param output        pointer to allocated memory for encrypted data. It has
 *                      to be of size data_len.
 */
int cipher_decrypt_ctr(cipher_t* cipher, uint8_t nonce_counter[16],
                       uint8_t nonce_len, uint8_t* input, size_t length,
                       uint8_t* output);

#endif /* __CRYPTO_MODES_CTR_H_*/
