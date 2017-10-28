/*
 * Copyright (C) Mathias Tausig
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

#include "crypto/ciphers.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
  
#define PADDING_NOT_ENOUGH_SPACE_AVAILABLE -1
#define PADDING_INVALID_BLOCKSIZE -2
  
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
  int pkcs7_padding (uint8_t *data, uint32_t data_len, uint8_t blocksize, uint8_t *padded_data, uint32_t padded_data_len);


#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_PADDING_H */
