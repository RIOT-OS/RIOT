/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file        ccm.h
 * @brief       Counter with CBC-MAC mode of operation for block ciphers
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 */

#include "crypto/ciphers.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CCM error codes
 * @{
 */
#define CCM_ERR_INVALID_NONCE_LENGTH        (-2)
#define CCM_ERR_INVALID_CBC_MAC             (-3)
#define CCM_ERR_INVALID_DATA_LENGTH         (-3)
#define CCM_ERR_INVALID_LENGTH_ENCODING     (-4)
#define CCM_ERR_INVALID_MAC_LENGTH          (-5)
/** @} */

/**
 * @brief Block size required for the cipher. CCM is only defined for 128 bit ciphers.
 */
#define CCM_BLOCK_SIZE                      16

/**
 * @brief Maximum length for the appended MAC
 */
#define CCM_MAC_MAX_LEN                     16

/**
 * @brief Encrypt and authenticate data of arbitrary length in ccm mode.
 *
 * @param cipher           Already initialized cipher struct
 * @param auth_data        Additional data to authenticate in MAC
 * @param auth_data_len    Length of additional data, max (2^16 - 2^8)
 * @param mac_length       length of the appended MAC (between 4 and 16 - only
 *                         even values)
 * @param length_encoding  maximal supported length of plaintext
 *                         (2^(8*length_enc)).
 * @param nonce            Nounce for ctr mode encryption
 * @param nonce_len        Length of the nonce in octets
 *                         (maximum: 15-length_encoding)
 * @param input            pointer to input data to encrypt
 * @param input_len        length of the input data, [0, 2^32]
 * @param output           pointer to allocated memory for encrypted data. It
 *                         has to be of size data_len + mac_length.
 *
 * @return                 Length of encrypted data on a successful encryption,
 *                         can be 0 if input_len=0 (no plaintext)
 * @return                 A negative error code if something went wrong
 */
int cipher_encrypt_ccm(const cipher_t *cipher,
                       const uint8_t *auth_data, uint32_t auth_data_len,
                       uint8_t mac_length, uint8_t length_encoding,
                       const uint8_t *nonce, size_t nonce_len,
                       const uint8_t *input, size_t input_len,
                       uint8_t *output);

/**
 * @brief Decrypt data of arbitrary length in ccm mode.
 *
 * @param cipher           Already initialized cipher struct
 * @param auth_data        Additional data to authenticate in MAC
 * @param auth_data_len    Length of additional data, max (2^16 - 2^8)
 * @param mac_length       length of the appended MAC (between 4 and 16 - only
 *                         even values)
 * @param length_encoding  maximal supported length of plaintext
 *                         (2^(8*length_enc)).
 * @param nonce            Nounce for ctr mode encryption
 * @param nonce_len        Length of the nonce in octets
 *                         (maximum: 15-length_encoding)
 * @param input            pointer to input data to decrypt
 * @param input_len        length of the input data, [0, 2^32]
 * @param output           pointer to allocated memory for decrypted data. It
 *                         has to be of size data_len - mac_length.
 *
 * @return                 Length of the decrypted data on a successful decryption,
 *                         can be 0 if only auth_data and MAC is present.
 * @return                 A negative error code if something went wrong
 */
int cipher_decrypt_ccm(const cipher_t *cipher,
                       const uint8_t *auth_data, uint32_t auth_data_len,
                       uint8_t mac_length, uint8_t length_encoding,
                       const uint8_t *nonce, size_t nonce_len,
                       const uint8_t *input, size_t input_len,
                       uint8_t *output);

#ifdef __cplusplus
}
#endif

/** @} */
