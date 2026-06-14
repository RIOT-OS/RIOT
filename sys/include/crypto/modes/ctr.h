/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

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

#include <stddef.h>
#include <unistd.h>

#include "compiler_hints.h"
#include "crypto/ciphers.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Encrypt data of arbitrary length in counter mode.
 *
 * @param[in]       cipher          Already initialized cipher struct
 * @param[in,out]   nonce_counter   Initialized buffer for nonce and counter
 * @param[in]       nonce_len       Length of the nonce in bytes
 * @param[in]       input           pointer to input data to encrypt
 * @param[in]       length          length of @p input and @p output in bytes
 * @param[out]      output          buffer to write the encrypted data to
 *
 * @warning Reusing the same nonce with the same key is a catastrophic
 *          failure and reveals the XOR of the plaintext of the two messages
 *          that used the same key and nonce.
 *
 * @pre             @p nonce_len < 16
 * @pre             @p length < `pow(2, 16 - nonce_len)`
 * @pre             The first @p nonce_len bytes of @p nonce_counter have been
 *                  initialized with a nonce that has not been used yet with
 *                  the same key, the remaining bytes are zero.
 *
 * @retval          CIPHER_ERR_ENC_FAILED   Encryption failed
 * @retval          <0                      Other error
 * @return          The number of encrypted bytes written to @p output - so
 *                  @p length - or a negative error code.
 */
ACCESS(read_only, 4, 5)
ACCESS(write_only, 6, 5)
ssize_t cipher_encrypt_ctr(const cipher_t *cipher, uint8_t nonce_counter[16],
                           uint8_t nonce_len, const uint8_t *input,
                           size_t length, uint8_t *output);

/**
 * @brief Decrypt data of arbitrary length in counter mode.
 *
 * Encryption and decryption in ctr mode are basically the same.
 *
 * @param[in]       cipher          Already initialized cipher struct
 * @param[in,out]   nonce_counter   Initialized buffer for nonce and counter
 * @param[in]       nonce_len       Length of the nonce in bytes
 * @param[in]       input           pointer to input data to encrypt
 * @param[in]       length          length of @p input and @p output in bytes
 * @param[out]      output          buffer to write the encrypted data to
 *
 * @pre             @p nonce_len < 16
 * @pre             @p length < `pow(2, 16 - nonce_len)`
 * @pre             The first @p nonce_len bytes of @p nonce_counter have been
 *                  initialized with a nonce that has not been used yet with
 *                  the same key, the remaining bytes are zero.
 *
 * @retval          CIPHER_ERR_ENC_FAILED   Encryption failed
 * @retval          <0                      Other error
 * @return          The number of encrypted bytes written to @p output - so
 *                  @p length - or a negative error code.
 */
ACCESS(read_only, 4, 5)
ACCESS(write_only, 6, 5)
ssize_t  cipher_decrypt_ctr(const cipher_t *cipher, uint8_t nonce_counter[16],
                            uint8_t nonce_len, const uint8_t *input,
                            size_t length, uint8_t *output);

#ifdef __cplusplus
}
#endif

/** @} */
