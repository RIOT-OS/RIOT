/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_bcd Binary coded decimal
 * @ingroup     sys
 * @brief       Library to de- and encode binary coded decimals
 * @{
 *
 * @file
 * @brief       BCD definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#ifndef BCD_H
#define BCD_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Converts a byte to a binary coded decimal
 *
 * @param[in] byte  A byte
 *
 * @return  A binary coded decimal (4 MSB = 10s, 4 LSB = 1s)
 */
static inline uint8_t bcd_from_byte(uint8_t byte)
{
    /* ((byte / 10) << 4) | (byte % 10) */
    return byte + (6 * (byte / 10));
}

/**
 * @brief   Converts a binary coded decimal to a byte
 *
 * @param[in] bcd   A binary coded decimal (4 MSB = 10, 4 LSB = 1s)
 *
 * @return  A byte
 */
static inline uint8_t bcd_to_byte(uint8_t bcd)
{
    /* == (10 * (bcd >> 4)) + (bcd & 0xf) */
    return bcd - (6 * (bcd >> 4));
}

/**
 * @brief   Convert a decimal value into a BCD buffer.
 *          (This looks like the decimal integer value when printed as hex)
 *
 * This will e.g. turn the value 123 -> 0x123 (decimal: 291)
 *
 * @param[in]  val  Decimal value to print
 * @param[out] dst  Destination buffer
 * @param[in]  len  Size of the destination buffer
 *
 * @return number of bytes written
 * @retval -ENOBUFS if @p dst is not large enough
 *         In that case the state of @p dst is undefined.
 */
int bcd_buf_from_u32(uint32_t val, void *dst, size_t len);

/**
 * @brief   Convert a BCD buffer into it's binary representation
 *          (This will reverse @ref bcd_buf_from_u32)
 *
 * @param[in] src   The BCD buffer to convert
 * @param[in] len   Bytes in @p src
 *
 * @returns decimal representation of @p src
 */
uint32_t bcd_buf_to_u32(const void *src, size_t len);

/**
 * @brief   Convert a BCD buffer into it's binary representation
 *
 * @param[in] src   The BCD buffer to convert
 * @param[in] len   Bytes in @p src
 *
 * @returns decimal representation of @p src
 */
uint64_t bcd_buf_to_u64(const void *src, size_t len);

/**
 * @brief   Convert a string into a BCD buffer
 *          Digits may be separated by any character.
 *
 * @param[in]  str      Input string
 * @param[in]  str_len  Length of the input string
 * @param[out] dst      Destination buffer
 * @param[in]  dst_len  Size of the destination buffer
 *
 * @return number of bytes written
 * @retval -ENOBUFS if @p dst is not large enough
 *         In that case the state of @p dst is undefined.
 */
int bcd_buf_from_str(const char *str, size_t str_len,
                     void *dst, size_t dst_len);

#ifdef __cplusplus
}
#endif

#endif /* BCD_H */
/** @} */
