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
 * @file        ccm.h
 * @brief       Counter with CBC-MAC mode of operation for block ciphers
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 */

#ifndef CRYPTO_MODES_CCM_H
#define CRYPTO_MODES_CCM_H

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
 * @brief Encrypt and authenticate data of arbitrary length in ccm mode.
 *
 * @param cipher           Already initialized cipher struct
 * @param auth_data        Additional data to authenticate in MAC
 * @param auth_data_len    Length of additional data
 * @param mac_length       length of the appended MAC (between 4 and 16 - only
 *                         even values)
 * @param length_encoding  maximal supported length of plaintext
 *                         (2^(8*length_enc)).
 * @param nonce            Nounce for ctr mode encryption
 * @param nonce_len        Length of the nonce in octets
 *                         (maximum: 15-length_encoding)
 * @param input            pointer to input data to encrypt
 * @param input_len        length of the input data
 * @param output           pointer to allocated memory for encrypted data. It
 *                         has to be of size data_len + mac_length.
 * @return                 Length of encrypted data on a successful encryption
 * @return                 A negative error code if something went wrong
 */
int cipher_encrypt_ccm(cipher_t* cipher,
                       const uint8_t* auth_data, uint32_t auth_data_len,
                       uint8_t mac_length, uint8_t length_encoding,
                       const uint8_t* nonce, size_t nonce_len,
                       const uint8_t* input, size_t input_len,
                       uint8_t* output);


/**
 * @brief Decrypt data of arbitrary length in ccm mode.
 *
 * @param cipher           Already initialized cipher struct
 * @param auth_data        Additional data to authenticate in MAC
 * @param auth_data_len    Length of additional data
 * @param mac_length       length of the appended MAC (between 4 and 16 - only
 *                         even values)
 * @param length_encoding  maximal supported length of plaintext
 *                         (2^(8*length_enc)).
 * @param nonce            Nounce for ctr mode encryption
 * @param nonce_len        Length of the nonce in octets
 *                         (maximum: 15-length_encoding)
 * @param input            pointer to input data to decrypt
 * @param input_len        length of the input data
 * @param output           pointer to allocated memory for decrypted data. It
 *                         has to be of size data_len - mac_length.
 *
 * @return                 Length of the decrypted data on a successful decryption
 * @return                 A negative error code if something went wrong
 */
int cipher_decrypt_ccm(cipher_t* cipher,
                       const uint8_t* auth_data, uint32_t auth_data_len,
                       uint8_t mac_length, uint8_t length_encoding,
                       const uint8_t* nonce, size_t nonce_len,
                       const uint8_t* input, size_t input_len,
                       uint8_t* output);

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_MODES_CCM_H */
/** @} */
