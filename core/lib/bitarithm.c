/*
 * SPDX-FileCopyrightText: 2013 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     core_util
 * @{
 *
 * @file
 * @brief       Bit arithmetic helper functions implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "bitarithm.h"

unsigned bitarith_msb_32bit_no_native_clz(unsigned v)
{
    register unsigned r; /* result of log2(v) will go here */
    register unsigned shift;

    /* begin{code-style-ignore} */
    r =     (v > 0xFFFF) << 4; v >>= r;
    shift = (v > 0xFF  ) << 3; v >>= shift; r |= shift;
    shift = (v > 0xF   ) << 2; v >>= shift; r |= shift;
    shift = (v > 0x3   ) << 1; v >>= shift; r |= shift;
                                            r |= (v >> 1);
    /* end{code-style-ignore} */

    return r;
}

unsigned bitarithm_bits_set(unsigned v)
{
    unsigned c; /* c accumulates the total bits set in v */

    for (c = 0; v; c++) {
        v &= v - 1; /* clear the least significant bit set */
    }

    return c;
}

#if !ARCH_32_BIT
uint8_t bitarithm_bits_set_u32(uint32_t v)
{
    uint8_t c;

    for (c = 0; v; c++) {
        v &= v - 1; /* clear the least significant bit set */
    }
    return c;
}
#endif

const uint8_t bitarithm_MultiplyDeBruijnBitPosition[32] =
{
    0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
    31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
};
