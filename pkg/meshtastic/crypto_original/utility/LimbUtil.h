/*
 * Copyright (C) 2015 Southern Storm Software, Pty Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef CRYPTO_LIMBUTIL_H
#define CRYPTO_LIMBUTIL_H

#include "ProgMemUtil.h"

// Number of limbs in a big number value of various sizes.
#define NUM_LIMBS_BITS(n) \
    (((n) + sizeof(limb_t) * 8 - 1) / (8 * sizeof(limb_t)))
#define NUM_LIMBS_128BIT NUM_LIMBS_BITS(128)
#define NUM_LIMBS_256BIT NUM_LIMBS_BITS(256)
#define NUM_LIMBS_512BIT NUM_LIMBS_BITS(512)

// The number of bits in a limb.
#define LIMB_BITS   (8 * sizeof(limb_t))

// Read a limb-sized quantity from program memory.
#if BIGNUMBER_LIMB_8BIT
#define pgm_read_limb(x)    (pgm_read_byte((x)))
#elif BIGNUMBER_LIMB_16BIT
#define pgm_read_limb(x)    (pgm_read_word((x)))
#elif BIGNUMBER_LIMB_32BIT
#define pgm_read_limb(x)    (pgm_read_dword((x)))
#elif BIGNUMBER_LIMB_64BIT
#define pgm_read_limb(x)    (pgm_read_qword((x)))
#endif

// Expand a 32-bit value into a set of limbs depending upon the limb size.
// This is used when initializing constant big number values in the code.
// For 64-bit system compatibility it is necessary to use LIMB_PAIR(x, y).
#if BIGNUMBER_LIMB_8BIT
#define LIMB(value)     ((uint8_t)(value)), \
                        ((uint8_t)((value) >> 8)), \
                        ((uint8_t)((value) >> 16)), \
                        ((uint8_t)((value) >> 24))
#define LIMB_PAIR(x,y)  LIMB((x)), LIMB((y))
#elif BIGNUMBER_LIMB_16BIT
#define LIMB(value)     ((uint16_t)(value)), \
                        ((uint16_t)(((uint32_t)(value)) >> 16))
#define LIMB_PAIR(x,y)  LIMB((x)), LIMB((y))
#elif BIGNUMBER_LIMB_32BIT
#define LIMB(value)     (value)
#define LIMB_PAIR(x,y)  LIMB((x)), LIMB((y))
#elif BIGNUMBER_LIMB_64BIT
#define LIMB(value)     (value)
#define LIMB_PAIR(x,y)  ((((uint64_t)(y)) << 32) | ((uint64_t)(x)))
#endif

#endif
