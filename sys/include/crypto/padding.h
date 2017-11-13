/*
 * Copyright (C) 2017 Mathias Tausig
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file        padding.h
 * @brief       Padding modes for encryption
 *
 * @author      Mathias Tausig
 */

#ifndef CRYPTO_PADDING_H
#define CRYPTO_PADDING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "crypto/ciphers.h"

#define PADDING_TYPE_PKCS7 1

#define PADDING_NOT_ENOUGH_SPACE_AVAILABLE -1
#define PADDING_INVALID_BLOCKSIZE -2
#define PADDING_INVALID_PADDING -3
#define PADDING_DATA_TOO_LONG -4

/**
 * @brief Apply PKCS#7 padding to some date
 *
 * @param data              The data to be padded
 * @param data_len          The length of the data
 * @param blocksize         The length of the block. The padded data will be a multiple of this value.
 * @param padded_data       Place to store the padded data
 * @param padded_data_len   Amount of data available for storage
 *
 * @return           The length of the padded data stored or a (negative) error code
 *
 */
int32_t pkcs7_padding(const uint8_t *data, uint32_t data_len, uint8_t blocksize, uint8_t *padded_data, uint32_t padded_data_len);

/**
 * @brief Remove PKCS#7 padding from some date
 *
 * @param data              The padded data
 * @param data_len          The length of the padded data. Must be a multiple of the blocksize.
 * @param blocksize         The length of the block.
 * @param unpadded_data     Place to store the unpadded data
 * @param unpadded_data_len Amount of data available for storage
 *
 * @return           The length of the unpadded data stored or a (negative) error code
 *
 */
int32_t pkcs7_padding_remove(uint8_t *data, uint32_t data_len, uint8_t blocksize, uint8_t *unpadded_data, uint32_t unpadded_data_len);

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_PADDING_H */
