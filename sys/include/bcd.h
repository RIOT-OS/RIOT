/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BCD_H
#define BCD_H
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
 */

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

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BCD_H */
