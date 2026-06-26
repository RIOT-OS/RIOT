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
 * @file        ccm.h
 * @brief       Counter with CBC-MAC mode of operation for block ciphers
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
 * @name CCM error codes
 * @{
 */
#define CCM_ERR_INVALID_NONCE_LENGTH        (-2)
#define CCM_ERR_INVALID_CBC_MAC             (-3)
#define CCM_ERR_INVALID_DATA_LENGTH         (-3)
#define CCM_ERR_INVALID_LENGTH_ENCODING     (-4)
#define CCM_ERR_INVALID_MAC_LENGTH          (-5)
#define CCM_ERR_INVALID_BLOCK_SIZE          (-6)
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
 * @brief   Parameters defining the maximum supported payload length
 *
 * This is named *q* in the [CCM specification][CCM spec] and controls the
 * length of *Q* in bytes, which encodes the payload length of a message in
 * bytes.
 *
 * Hence, the payload must be no larger than `2^(8 * q) - 1` bytes in size.
 *
 * The value of *q* also determines the length of the nonce, as the sum of both
 * must be 15. Since a nonce cannot be reused, the parameter *q* controls a
 * trade-off between the maximum message size and the maximum number of messages
 * that can be sent before replacing the key.
 *
 * [CCM spec]: https://nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38c.pdf
 */
typedef enum {
    CIPHER_CCM_MAX_64KIB = 2,   /**< Support payloads up to 64 KiB in size */
    CIPHER_CCM_MAX_16MIB = 3,   /**< Support payloads up to 16 MiB in size */
    CIPHER_CCM_MAX_4GIB = 4,    /**< Support payloads up to 4 GiB in size */
    CIPHER_CCM_MAX_1TIB = 5,    /**< Support payloads up to 1 TiB in size */
    CIPHER_CCM_MAX_256TIB = 6,  /**< Support payloads up to 256 TiB in size */
    CIPHER_CCM_MAX_64PIB = 7,   /**< Support payloads up to 64 PiB in size */
    CIPHER_CCM_MAX_16EIB = 8,   /**< Support payloads up to 16 EiB in size */
} cipher_ccm_q_t;

/**
 * @brief   Encrypt and authenticate data of arbitrary length in CCM mode.
 *
 * @param[in]   cipher          Already initialized cipher struct
 * @param[in]   auth_data       Additional data to authenticate in MAC
 * @param[in]   auth_data_len   Length of additional data, max (2^16 - 2^8)
 * @param[in]   mac_length      length of the appended MAC (between 4 and 16 -
 *                              only even values)
 * @param[in]   length_encoding maximal supported length of plaintext
 *                              (2^(8*length_enc)).
 * @param[in]   nonce           Nounce for ctr mode encryption
 * @param[in]   nonce_len       Length of the nonce in octets
 *                              (maximum: 15-length_encoding)
 * @param[in]   input           pointer to input data to encrypt
 * @param[in]   input_len       length of the input data, [0, 2^32]
 * @param[out]  output          pointer to allocated memory for encrypted data.
 *                              It has to be of size data_len + mac_length.
 *
 * @pre     2 <= @p length_encoding <= 8
 * @pre     @p length_encoding + @p nonce_len == 15
 * @pre     @p mac_length is even and 4 <= @p mac_length <= 16
 * @pre     @p auth_data_len < 2^64 (you won't exceed this with `size_t`)
 *
 * @retval  CCM_ERR_INVALID_NONCE_LENGTH    @p nonce_len invalid
 * @retval  CCM_ERR_INVALID_DATA_LENGTH     @p input_len invalid
 * @retval  CCM_ERR_INVALID_LENGTH_ENCODING @p length_encoding invalid
 * @retval  CCM_ERR_INVALID_MAC_LENGTH      @p mac_length invliad
 * @retval  CCM_ERR_INVALID_BLOCK_SIZE      @p cipher has invalid block size
 * @retval  <0                              Other error
 * @return  Length of encrypted data on a successful encryption,
 *          can be @p mac_length if @p input_len is `0` (no plaintext)
 *
 * @pre     2 <= @p length_encoding <= 8
 * @pre     @p length_encoding + @p nonce_len == 15
 * @pre     @p mac_length is even and 4 <= @p mac_length <= 16
 * @pre     @p auth_data_len < 2^64 (you won't exceed this with `size_t`)
 */
ACCESS(read_only, 2, 3)
ACCESS(read_only, 6, 7)
ACCESS(read_only, 8, 9)
ssize_t cipher_encrypt_ccm(const cipher_t *cipher,
                           const uint8_t *auth_data, size_t auth_data_len,
                           uint8_t mac_length,
                           cipher_ccm_q_t length_encoding,
                           const uint8_t *nonce, size_t nonce_len,
                           const uint8_t *input, size_t input_len,
                           uint8_t output[input_len + mac_length]);

/**
 * @brief   Decrypt data of arbitrary length in CCM mode.
 *
 * @param[in]   cipher           Already initialized cipher struct
 * @param[in]   auth_data        Additional data to authenticate in MAC
 * @param[in]   auth_data_len    Length of additional data, max (2^16 - 2^8)
 * @param[in]   mac_length       length of the appended MAC (between 4 and 16 - only
 *                               even values)
 * @param[in]   length_encoding  maximal supported length of plaintext
 *                               (2^(8*length_enc)).
 * @param[in]   nonce            Nounce for ctr mode encryption
 * @param[in]   nonce_len        Length of the nonce in octets
 *                               (maximum: 15-length_encoding)
 * @param[in]   input            pointer to input data to decrypt
 * @param[in]   input_len        length of the input data, [0, 2^32]
 * @param[out]  output           pointer to allocated memory for decrypted data. It
 *                               has to be of size data_len - mac_length.
 * @retval  CCM_ERR_INVALID_NONCE_LENGTH    @p nonce_len invalid
 * @retval  CCM_ERR_INVALID_CBC_MAC         Computed MAC did not match provided
 *                                          (data integrity compromised)
 * @retval  CCM_ERR_INVALID_DATA_LENGTH     @p input_len invalid
 * @retval  CCM_ERR_INVALID_LENGTH_ENCODING @p length_encoding invalid
 * @retval  CCM_ERR_INVALID_MAC_LENGTH      @p mac_length invliad
 * @retval  CCM_ERR_INVALID_BLOCK_SIZE      @p cipher has invalid block size
 * @retval  0                               no ciphertext to decrypt, but
 *                                          integrity @p auth_data verified
 * @retval  <0                              Other error
 *
 * @return                 Length of the decrypted data on a success
 *
 * @pre     2 <= @p length_encoding <= 8
 * @pre     @p length_encoding + @p nonce_len == 15
 * @pre     @p mac_length is even and 4 <= @p mac_length <= 16
 * @pre     @p auth_data_len < 2^64 (you won't exceed this with `size_t`)
 */
ACCESS(read_only, 2, 3)
ACCESS(read_only, 6, 7)
ACCESS(read_only, 8, 9)
ssize_t cipher_decrypt_ccm(const cipher_t *cipher,
                           const uint8_t *auth_data, size_t auth_data_len,
                           uint8_t mac_length,
                           cipher_ccm_q_t length_encoding,
                           const uint8_t *nonce, size_t nonce_len,
                           const uint8_t *input, size_t input_len,
                           uint8_t output[input_len - mac_length]);

#ifdef __cplusplus
}
#endif

/** @} */
