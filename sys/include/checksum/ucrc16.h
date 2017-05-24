/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_checksum_ucrc16     CRC16 (lightweight)
 * @ingroup     sys_checksum
 * @brief       Lightweight CRC16 checksum algorithms
 *
 * This CRC16 implementation does not use pre-calculated lookup tables and is
 * thus very lightweight (memory-wise), but as a caveat slower (about factor 8)
 * than the version with pre-calculated lookup tables.
 *
 * Additionally compared to @ref sys_checksum_crc16_ccitt this is a generalized
 * implementation. One can easily exchange generator polynomials and starting
 * seeds.
 *
 * @{
 *
 * @file
 * @brief   ucrc16 definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef CHECKSUM_UCRC16_H
#define CHECKSUM_UCRC16_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @{
 * @brief   Various generator polynomials
 */
#define UCRC16_CCITT_POLY_BE    (0x1021)    /**< CRC16-CCITT polynomial (big-endian) */
#define UCRC16_CCITT_POLY_LE    (0x8408)    /**< CRC16-CCITT polynomial (little-endian) */
/** @} */

/**
 * @brief   Calculate CRC16 (big-endian version)
 *
 * @param[in] buf   Start of memory are to checksum
 * @param[in] len   Number of bytes in @p buf to calculate checksum for
 * @param[in] poly  The generator polynomial for the checksum
 * @param[in] seed  The seed (starting value) for the checksum
 *
 * @return  Checksum of the specified memory area based on @p seed and @p poly
 *
 * @note    The return value is not the complement of the sum but the sum itself
 */
uint16_t ucrc16_calc_be(const uint8_t *buf, size_t len, uint16_t poly,
                        uint16_t seed);

/**
 * @brief   Calculate CRC16 (little-endian version)
 *
 * @param[in] buf   Start of memory are to checksum
 * @param[in] len   Number of bytes in @p buf to calculate checksum for
 * @param[in] poly  The generator polynomial for the checksum
 * @param[in] seed  The seed (starting value) for the checksum
 *
 * @return  Checksum of the specified memory area based on @p seed and @p poly
 *
 * @note    The return value is not the complement of the sum but the sum itself
 */
uint16_t ucrc16_calc_le(const uint8_t *buf, size_t len, uint16_t poly,
                        uint16_t seed);

#ifdef __cplusplus
}
#endif

#endif /* CHECKSUM_UCRC16_H */
/** @} */
