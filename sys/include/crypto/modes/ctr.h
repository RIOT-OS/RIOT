/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef CRYPTO_MODES_CTR_H
#define CRYPTO_MODES_CTR_H

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

#include "crypto/ciphers.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 * @param length        length of the input data
 * @param output        pointer to allocated memory for encrypted data. It has
 *                      to be of size data_len.
 *
 * @return              Length of encrypted data on a successful encryption
 * @return              A negative error code if something went wrong
 */
int cipher_encrypt_ctr(const cipher_t *cipher, uint8_t nonce_counter[16],
                       uint8_t nonce_len, const uint8_t *input, size_t length,
                       uint8_t *output);

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
 * @param length        length of the input data
 * @param output        pointer to allocated memory for encrypted data. It has
 *                      to be of size data_len.
 *
 * @return              Length of decrypted data on a successful decryption
 * @return              A negative error code if something went wrong
 */
int cipher_decrypt_ctr(const cipher_t *cipher, uint8_t nonce_counter[16],
                       uint8_t nonce_len, const uint8_t *input, size_t length,
                       uint8_t *output);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CRYPTO_MODES_CTR_H */
