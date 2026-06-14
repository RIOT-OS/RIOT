/*
 * Copyright (C) 2018 Mathias Tausig
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
 * @file        ocb.h
 * @brief       Offset Codebook (OCB3) AEAD mode as specified in RFC 7253
 *
 * @warning     The OCB algorithm is covered by patents in the USA
 *              owned by Phillip Rogaway. A free licence is granted
 *              for any open-source or non-military project. Check
 *              http://web.cs.ucdavis.edu/~rogaway/ocb/grant.htm for details.
 *
 * @author      Mathias Tausig <mathias@tausig.at>
 */

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include "compiler_hints.h"
#include "crypto/ciphers.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name OCB error codes
 * @{
 */

/**
 * @brief   Returned if a nonce of bad length (empty or more than 15 bytes)
 *          was used
 */
#define OCB_ERR_INVALID_NONCE_LENGTH (-2)
/**
 * @brief   OCB only works with ciphers with a block size of 128 bit
 */
#define OCB_ERR_INVALID_BLOCK_LENGTH (-3)
/**
 * @brief   Returned if the amount of input data cannot be handled by this
 *          implementation
 */
#define OCB_ERR_INVALID_DATA_LENGTH  (-3)
/**
 * @brief   Returned if a tag of bad length was requested (empty or more than
 *          16 bytes)
 */
#define OCB_ERR_INVALID_TAG_LENGTH   (-4)
/**
 * @brief   Returned if the authentication failed during decryption
 */
#define OCB_ERR_INVALID_TAG          (-5)

/** @} */

/**
 * @brief   Encrypt and authenticate data of arbitrary length in OCB mode.
 *
 * @param[in]   cipher           Already initialized cipher struct
 * @param[in]   auth_data        Additional data to authenticate in MAC
 * @param[in]   auth_data_len    Length of @p auth_data in bytes
 * @param[in]   tag_len          Length of the appended tag
 * @param[in]   nonce            Nonce for the encryption
 * @param[in]   nonce_len        Length of @p nonce in bytes
 * @param[in]   input            pointer to input data to encrypt
 * @param[in]   input_len        length of @p input in bytes
 * @param[out]  output           buffer to write the ciphertext and tag to
 *
 * @pre         @p nonce has not been used with the same key before
 * @pre         1 <= @p tag_len <= 16
 * @pre         @p input_len + @p tag_len <= `SSIZE_MAX`
 * @pre         @p nonce_len <= 15
 *
 * @return      Number of bytes written to @p output or negative error code
 * @retval      OCB_ERR_INVALID_NONCE_LENGTH    @p nonce_len invalid
 * @retval      OCB_ERR_INVALID_BLOCK_LENGTH    @p cipher incompatible with OCB
 * @retval      OCB_ERR_INVALID_DATA_LENGTH     @p input_len too large
 * @retval      OCB_ERR_INVALID_TAG_LENGTH      @p tag_len invalid
 */
ACCESS(read_only, 2, 3)
ACCESS(read_only, 5, 6)
ACCESS(read_only, 7, 8)
ssize_t cipher_encrypt_ocb(const cipher_t *cipher,
                           const uint8_t *auth_data, size_t auth_data_len,
                           uint8_t tag_len,
                           const uint8_t *nonce, size_t nonce_len,
                           const uint8_t *input, size_t input_len,
                           uint8_t output[input_len + tag_len]);

/**
 * @brief   Decrypt and verify the authentication of OCB encrypted data.
 *
 * @param[in]   cipher           Already initialized cipher struct
 * @param[in]   auth_data        Additional data to authenticate in MAC
 * @param[in]   auth_data_len    Length of @p auth_data in bytes
 * @param[in]   tag_len          Length of the appended tag at the end of @p input
 * @param[in]   nonce            Nonce for the encryption
 * @param[in]   nonce_len        @p length of @p nonce in bytes
 * @param[in]   input            pointer to the ciphertext with the tag appended
 * @param[in]   input_len        length of @p input in bytes
 * @param[out]  output           Buffer to write the plaintext to
 *
 * @pre         @p nonce has not been used with the same key before
 * @pre         1 <= @p tag_len <= 16
 * @pre         @p input_len >= @p tag_len
 * @pre         @p input_len - @p tag_len <= `SSIZE_MAX`
 * @pre         @p nonce_len <= 15
 *
 * @return      Number of bytes written to @p output or negative error code
 * @retval      OCB_ERR_INVALID_NONCE_LENGTH    @p nonce_len invalid
 * @retval      OCB_ERR_INVALID_BLOCK_LENGTH    @p cipher incompatible with OCB
 * @retval      OCB_ERR_INVALID_DATA_LENGTH     @p input_len too large
 * @retval      OCB_ERR_INVALID_TAG_LENGTH      @p tag_len invalid
 */
ACCESS(read_only, 2, 3)
ACCESS(read_only, 5, 6)
ACCESS(read_only, 7, 8)
ssize_t cipher_decrypt_ocb(const cipher_t *cipher,
                           const uint8_t *auth_data, size_t auth_data_len,
                           uint8_t tag_len,
                           const uint8_t *nonce, size_t nonce_len,
                           const uint8_t *input, size_t input_len,
                           uint8_t output[input_len - tag_len]);
#ifdef __cplusplus
}
#endif

/** @} */
