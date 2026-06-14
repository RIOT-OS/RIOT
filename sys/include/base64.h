/*
 * SPDX-FileCopyrightText: 2014 Hochschule für Angewandte Wissenschaften Hamburg (HAW)
 * SPDX-FileCopyrightText: 2014 Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_base64 Base64 encoder decoder
 * @ingroup     sys_serialization
 * @brief       Base64 encoder and decoder
 * @{
 *
 * @brief       Encoding and decoding functions for base64
 * @author      Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 */

#include <stddef.h> /* for size_t */
#include <stdbool.h> /* for bool */

#ifdef __cplusplus
extern "C" {
#endif

#define BASE64_SUCCESS                  (0)  /**< return value for success                       */
#define BASE64_ERROR_BUFFER_OUT         (-1) /**< error value for invalid output buffer pointer  */
#define BASE64_ERROR_BUFFER_OUT_SIZE    (-2) /**< error value for invalid output buffer size     */
#define BASE64_ERROR_DATA_IN            (-3) /**< error value for invalid input buffer           */
#define BASE64_ERROR_DATA_IN_SIZE       (-4) /**< error value for invalid input buffer size      */
#define BASE64_ERROR_OVERFLOW           (-5) /**< error value for buffer overflow                */

/**
 * @brief   Estimates the amount of bytes needed for decoding @p base64_in_size
 *          characters from base64.
 *
 * @param[in]   base64_in_size      Size of the string to be decoded
 *
 * @return  Amount of bytes estimated to be used after decoding
 */
static inline size_t base64_estimate_decode_size(size_t base64_in_size)
{
    return (((base64_in_size + 3) / 4) * 3);
}

/**
 * @brief   Estimates the length of the resulting string after encoding
 *          @p data_in_size bytes into base64.
 *
 * @pre     @p data_in_size must be smaller than ~75% of the SIZE_MAX to
 *          prevent integer overflow in the calculation of the return value.
 *
 * @param[in]   data_in_size        Amount of bytes to be encoded
 *
 * @return Amount of characters the output string is estimated to have
 */
static inline size_t base64_estimate_encode_size(size_t data_in_size)
{
    return (4 * ((data_in_size + 2) / 3));
}

/**
 * @brief   Checks if the given size of data can return a valid estimate for
 *          the size without an integer overflow.
 *
 * @param[in]   data_in_size        Amount of bytes to be encoded
 *
 * @retval  true    if the given size can be encoded to base64
 * @retval  false   if the given size would overflow
 */
static inline bool base64_can_estimate_encode_size(size_t data_in_size)
{
    return (data_in_size <= (SIZE_MAX / 4) * 3);
}

/**
 * @brief           Encodes a given datum to base64 and save the result to the given destination.
 *
 * @param[in]       data_in         Pointer to the datum to encode
 * @param[in]       data_in_size    The size of `data_in`
 * @param[out]      base64_out      Pointer to store the encoded base64 string (may be tainted
 *                                  on error)
 * @param[in,out]   base64_out_size Pointer to the variable containing the size of `base64_out.`
 *                                  This value is overwritten with the estimated size used for
 *                                  the encoded base64 string on BASE64_ERROR_BUFFER_OUT_SIZE.
 *                                  This value is overwritten with the actual used size for the
 *                                  encoded base64 string on BASE64_SUCCESS.
 *
 * @retval  BASE64_SUCCESS                  on success
 * @retval  BASE64_ERROR_BUFFER_OUT_SIZE    on insufficient size for encoding to `base64_out`
 * @retval  BASE64_ERROR_BUFFER_OUT         if `base64_out` equals NULL but the `base64_out_size`
 *                                          is sufficient
 * @retval  BASE64_ERROR_DATA_IN            if `data_in` equals NULL
 * @retval  BASE64_ERROR_OVERFLOW           if `data_in_size` is too large to be encoded to base64
 */
int base64_encode(const void *data_in, size_t data_in_size,
                  void *base64_out, size_t *base64_out_size);

/**
 * @brief           Encodes a given datum to base64 with URL and Filename Safe Alphabet
 *                  and save the result to the given destination.
 *
 * @see             [RFC 4648, section 5](https://tools.ietf.org/html/rfc4648#section-5)
 *
 * @note            Requires the use of the `base64url` module.
 *
 * @param[in]       data_in         Pointer to the datum to encode
 * @param[in]       data_in_size    The size of `data_in`
 * @param[out]      base64_out      Pointer to store the encoded base64 string (may be tainted
 *                                  on error)
 * @param[in,out]   base64_out_size Pointer to the variable containing the size of `base64_out.`
 *                                  This value is overwritten with the estimated size used for
 *                                  the encoded base64 string on BASE64_ERROR_BUFFER_OUT_SIZE.
 *                                  This value is overwritten with the actual used size for the
 *                                  encoded base64 string on BASE64_SUCCESS.
 *
 * @retval  BASE64_SUCCESS                  on success
 * @retval  BASE64_ERROR_BUFFER_OUT_SIZE    on insufficient size for encoding to `base64_out`
 * @retval  BASE64_ERROR_BUFFER_OUT         if `base64_out` equals NULL but the `base64_out_size`
 *                                          is sufficient
 * @retval  BASE64_ERROR_DATA_IN            if `data_in` equals NULL
 * @retval  BASE64_ERROR_OVERFLOW           if `data_in_size` is too large to be encoded to base64
 */
int base64url_encode(const void *data_in, size_t data_in_size,
                     void *base64_out, size_t *base64_out_size);

/**
 * @brief           Decodes a given base64 string and save the result to the given destination.
 *
 * @param[in]       base64_in        Pointer to the datum to decode
 * @param[in]       base64_in_size   The size of `base64_in`
 * @param[out]      data_out         Pointer to store the decoded base64 string (may be tainted
 *                                   on error)
 * @param[in,out]   data_out_size    The size of `data_out`.
 *                                   This value is overwritten with the estimated size used for the
 *                                   decoded string on BASE64_ERROR_BUFFER_OUT_SIZE.
 *                                   This value is overwritten with the actual used size for the
 *                                   decoded string on BASE64_SUCCESS.
 *
 * @retval  BASE64_SUCCESS                  on success
 * @retval  BASE64_ERROR_BUFFER_OUT_SIZE    on insufficient size for decoding to `data_out`
 * @retval  BASE64_ERROR_BUFFER_OUT         if `data_out` equals NULL but the size for
 *                                          `data_out_size` is sufficient
 * @retval  BASE64_ERROR_DATA_IN            if `base64_in` equals NULL
 * @retval  BASE64_ERROR_DATA_IN_SIZE       if `base64_in_size` has remainder 1 (mod 4)
 */
int base64_decode(const void *base64_in, size_t base64_in_size,
                  void *data_out, size_t *data_out_size);

#ifdef __cplusplus
}
#endif

/** @} */
