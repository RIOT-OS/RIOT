/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
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
 * @brief       Implementation of the Grøstl-256 hash function
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Praveen Gauravaram
 * @author      Lars R. Knudsen
 * @author      Krystian Matusiewicz
 * @author      Florian Mendel
 * @author      Christian Rechberger
 * @author      Martin Schläffer
 * @author      Søren S. Thomsen
 *
 */

#ifndef __GROESTL256_H_
#define __GROESTL256_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The Grøstl-256 state context
 */
typedef struct {
    uint8_t bytes_in_block;
    uint8_t first_padding_block;
    uint8_t last_padding_block;
} groestl256_ctx;
/** @} */

/**
 * @name Algorithm constants
 * @{
 */
#define CRYPTO_BYTES	32
#define ROUNDS		((CRYPTO_BYTES<=32?10:14))
#define STATEBYTES	((CRYPTO_BYTES<=32?64:128))
#define STATEWORDS	(STATEBYTES/4)
#define STATECOLS	(STATEBYTES/8)
#define COLWORDS	(STATEWORDS/8)
#define BYTESLICE(i)	(((i)%8)*STATECOLS+(i)/8)

static const uint32_t columnconstant[2] = { 0x30201000, 0x70605040 };
static const uint8_t shiftvalues[2][8] = { {0, 1, 2, 3, 4, 5, 6, 7}, {1, 3, 5, 7, 0, 2, 4, 6} };
/** @} */

/**
 * @name Algorithm macros
 * @{
 */
#define mul2(x,t) \
{\
  t = x & 0x80808080;\
  x ^= t;\
  x <<= 1;\
  t = t >> 7;\
  t ^= (t << 1);\
  x ^= t;\
  x ^= (t << 3);\
}
/** @} */

/**
 * @brief  Calculate Grøstl-256 hash from input
 *
 * @param[out]  out             Hash digest output variable
 * @param[in]   in              Input bytes to hash
 * @param[in]   inlen           Length of input bytes
 *
 * @return 0 if successful, -1 on error
 */
int groestl256(uint8_t *out, const uint8_t *in, uint64_t inlen);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __GROESTL256_H_ */
