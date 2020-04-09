/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file        ccms.h
 * @brief       Counter with CCM* mode of operation for block ciphers
 *
 * @author      Francisco Molina <francisco.molina@inria.cl>
 */

#ifndef CRYPTO_MODES_CCMS_H
#define CRYPTO_MODES_CCMS_H

#include "ccm_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Encrypt and authenticate data of arbitrary length in ccms mode.
 *
 * @param cipher           Already initialized cipher struct
 * @param auth_data        Additional data to authenticate in MAC
 * @param auth_data_len    Length of additional data, max (2^16 - 2^8)
 * @param mac_length       length of the appended MAC (between 0 and 16 - only
 *                         even values, 2 excluded)
 * @param length_encoding  maximal supported length of plaintext
 *                         (2^(8*length_enc)), implementation supports (2^32)
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
int cipher_encrypt_ccms(cipher_t *cipher,
                        const uint8_t *auth_data, uint32_t auth_data_len,
                        uint8_t mac_length, uint8_t length_encoding,
                        const uint8_t *nonce, size_t nonce_len,
                        const uint8_t *input, size_t input_len,
                        uint8_t *output);


/**
 * @brief Decrypt data of arbitrary length in ccms mode.
 *
 * @param cipher           Already initialized cipher struct
 * @param auth_data        Additional data to authenticate in MAC
 * @param auth_data_len    Length of additional data, max (2^16 - 2^8)
 * @param mac_length       length of the appended MAC (between 0 and 16 - only
 *                         even values)
 * @param length_encoding  maximal supported length of plaintext
 *                         (2^(8*length_enc)), implementation supports (2^32)
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
int cipher_decrypt_ccms(cipher_t *cipher,
                        const uint8_t *auth_data, uint32_t auth_data_len,
                        uint8_t mac_length, uint8_t length_encoding,
                        const uint8_t *nonce, size_t nonce_len,
                        const uint8_t *input, size_t input_len,
                        uint8_t *output);

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_MODES_CCMS_H */
/** @} */
