/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 * Copyright (C) 2014 Daniel Beer <dlbeer@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file
 * @brief       Headers for the implementation for convertion between c25510 and ed25519
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Daniel Beer <dlbeer@gmail.com>
 *
 */

#ifndef __MORPH25519_H_
#define __MORPH25519_H_

#include <stdint.h>

/**
 * @brief  Convert a Edwards Y to a Montgomery X
 *
 * @param[out]  montgomery_x	Montgomery X coordinate output
 * @param[in]   edwards_y	Edwards Y coordinate input
 */
void morph25519_e2m(uint8_t *montgomery_x, const uint8_t *edwards_y);

/**
 * @brief  Convert a Montgomery X and a parity bit to a Edwards X/Y
 *
 * @param[out]   ex		Edwards X coordinate output
 * @param[out]   ey		Edwards Y coordinate output
 * @param[in]    mx		Montgomery X coordinate input
 * @param[in]    parity		Parity bit
 *
 * @return Returns non-zero value if successful
 */
uint8_t morph25519_m2e(uint8_t *ex, uint8_t *ey, const uint8_t *mx, int parity);

/**
 * @brief  Return a parity bit for the Edwards X coordinate
 *
 * @param[in]   edwards_x       Edwards X coordinate input
 *
 * @return Returns a parity bit for the Edwards X coordinate input
 */
static inline int morph25519_eparity(const uint8_t *edwards_x)
{
    return edwards_x[0] & 1;
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __MORPH25519_H_ */
