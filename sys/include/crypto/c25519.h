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
 * @brief       Headers for the implementation of the Curve25519 Diffie-Hellman function
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Daniel Beer <dlbeer@gmail.com>
 * @author      D. J. Bernstein
 *
 */

#ifndef __C25519_H_
#define __C25519_H_

#include <stdint.h>
#include "f25519.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Algorithm constants
 * @{
 */
/* This is the site of a Curve25519 exponent (private key) */
#define C25519_EXPONENT_SIZE	32

/* X-coordinate of the base point */
extern const uint8_t c25519_base_x[F25519_SIZE];
/** @} */

/**
 * @brief  X-coordinate scalar multiply
 *
 * @param[out]  result      Return X-coordinate of e*q
 * @param[in]   q           X-coordinate field element
 * @param[in]   e           Scalar multiply exponent
 */
void c25519_smult(uint8_t *result, const uint8_t *q, const uint8_t *e);

/**
 * @brief  Finalize random generated key
 *
 * @param[in]   key         Random generated key
 */
static inline void c25519_prepare(uint8_t *key)
{
    key[0] &= 0xf8;
    key[31] &= 0x7f;
    key[31] |= 0x40;
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __C25519_H_ */
