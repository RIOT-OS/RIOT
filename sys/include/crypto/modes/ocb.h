/*
 * Copyright (C) 2018 Mathias Tausig
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file        ocb.h
 * @brief       Offset Codebook (OCB3) AEAD mode as specified in RFC 7253
 *
 * NOTE: The OCB algorithm is covered by patents in the USA owned by Phillip Rogaway.
 * A free licence is granted for any open-source or non-military project.
 * Check http://web.cs.ucdavis.edu/~rogaway/ocb/grant.htm for details.
 *
 * @author      Mathias Tausig <mathias@tausig.at>
 */

#ifndef CRYPTO_MODES_OCB_H
#define CRYPTO_MODES_OCB_H

#include "crypto/ciphers.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name OCB error codes
 * @{
 */

/**
 * Returned if a nonce of bad length (empty or more than 15 bytes) was used
 */
#define OCB_ERR_INVALID_NONCE_LENGTH        (-2)
/**
 * OCB only works with ciphers with a block size of 128 bit
 */
#define OCB_ERR_INVALID_BLOCK_LENGTH        (-3)
/**
 * Returned if the amount of input data cannot be handled by this implementation
 */
#define OCB_ERR_INVALID_DATA_LENGTH         (-3)
/**
 * Returned if a tag of bad length was requested (empty or more than 16 bytes)
 */
#define OCB_ERR_INVALID_TAG_LENGTH          (-4)
/**
 * Returned if the authentication failed during decryption
 */
#define OCB_ERR_INVALID_TAG                 (-5)

/** @} */

/**
 * @brief Encrypt and authenticate data of arbitrary length in OCB mode.
 *
 * @param cipher           Already initialized cipher struct
 * @param auth_data        Additional data to authenticate in MAC
 * @param auth_data_len    Length of additional data
 * @param tag_len       Length of the appended tag (at least 1, at most 16 bytes)
 *
 * @param nonce            Nonce for the encryption (must be unique)
 * @param nonce_len        Length of the nonce in bytes (at most 15)
 * @param input            pointer to input data to encrypt
 * @param input_len        length of the input data.
 *                         input_len + tag_len must be smaller than INT32_MAX (2^31-1)
 * @param output           pointer to allocated memory for encrypted data.
 *                         The tag will be appended to the ciphertext.
 *                         It has to be of size data_len + tag_len.
 * @return                 Length of the encrypted data (including the tag) or a (negative) error code
 */
int32_t cipher_encrypt_ocb(cipher_t *cipher, uint8_t *auth_data,
                           size_t auth_data_len,
                           uint8_t tag_len, uint8_t *nonce, size_t nonce_len,
                           uint8_t *input, size_t input_len, uint8_t *output);

/**
 * @brief Decrypt and verify the authentication of OCB encrypted data.
 *
 * @param cipher           Already initialized cipher struct
 * @param auth_data        Additional data to authenticate in MAC
 * @param auth_data_len    Length of additional data
 * @param tag_len       Length of the appended tag (at least 1, at most 16 bytes)
 *
 * @param nonce            Nonce for the encryption (must be unique)
 * @param nonce_len        Length of the nonce in bytes (at most 15)
 * @param input            pointer to the ciphertext with the tag appended
 * @param input_len        length of the input data.
 *                         input_len - tag_len must be smaller than INT32_MAX (2^31-1)
 * @param output           pointer to allocated memory for the plaintext data.
 *                         It has to be of size input_len - tag_len.
 *                         Will contain only zeroes, if the authentication fails.
 * @return                 Length of the plaintext data or a (negative) error code
 */
int32_t cipher_decrypt_ocb(cipher_t *cipher, uint8_t *auth_data,
                           size_t auth_data_len,
                           uint8_t tag_len, uint8_t *nonce, size_t nonce_len,
                           uint8_t *input, size_t input_len, uint8_t *output);
#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_MODES_OCB_H */
/** @} */
