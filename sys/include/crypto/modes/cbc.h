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
 * @file        cbc.h
 * @brief       Cipher block chaining mode of operation for block ciphers
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
 * @brief   Encrypt data of arbitrary length in cipher block chaining mode.
 *
 * @param[in]   cipher      Already initialized cipher struct
 * @param[in]   iv          16 octet initialization vector. Must never be used
 *                          more than once for a given key.
 * @param[in]   input       pointer to @p input_len bytes of data to encrypt
 * @param[in]   input_len   length of @p input_len in bytes
 * @param[out]  output      pointer to allocated memory for encrypted data
 *
 * @pre         @p input_len is a multiple of the block size of @p cipher
 *
 * @retval      <0                          error
 * @retval      CIPHER_ERR_INVALID_LENGTH   input_len % BLOCK_SIZE != 0
 * @retval      CIPHER_ERR_ENC_FAILED       internal encryption error
 * @return      number of encrypted bytes written to @p output
 */
ACCESS(read_only, 3, 4)
ACCESS(write_only, 5, 4)
ssize_t cipher_encrypt_cbc(const cipher_t *cipher, const uint8_t iv[16],
                           const void *input, size_t input_len, void *output);

/**
 * @brief Decrypt encrypted data in cipher block chaining mode.
 *
 * @param[in]   cipher      Already initialized cipher struct
 * @param[in]   iv          16 octet initialization vector.
 * @param[in]   input       pointer to input data to decrypt
 * @param[in]   input_len   length of the input data
 * @param[out]  output      pointer to allocated memory for decrypted data
 * @pre         @p input_len is a multiple of the block size of @p cipher
 *
 * @retval      <0                          error
 * @retval      CIPHER_ERR_INVALID_LENGTH   input_len % BLOCK_SIZE != 0
 * @retval      CIPHER_ERR_ENC_FAILED       internal decryption error
 * @return      number of decrypted bytes written to @p output
 */
ACCESS(read_only, 3, 4)
ACCESS(write_only,5, 4)
ssize_t cipher_decrypt_cbc(const cipher_t *cipher, const uint8_t iv[16],
                           const void *input, size_t input_len, void *output);

#ifdef __cplusplus
}
#endif

/** @} */
