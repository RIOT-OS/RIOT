/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_util
 * @{
 *
 * @file        bitarithm.c
 * @brief       Bit arithmetic helper functions implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

unsigned bitarithm_msb(unsigned v)
{
    register unsigned r; // result of log2(v) will go here

#if ARCH_32_BIT
    register unsigned shift;

    r =     (v > 0xFFFF) << 4; v >>= r;
    shift = (v > 0xFF  ) << 3; v >>= shift; r |= shift;
    shift = (v > 0xF   ) << 2; v >>= shift; r |= shift;
    shift = (v > 0x3   ) << 1; v >>= shift; r |= shift;
                                            r |= (v >> 1);
#else
    r = 0;
    while (v >>= 1) { // unroll for more speed...
        r++;
    }

#endif

    return r;
}
/*---------------------------------------------------------------------------*/
unsigned bitarithm_lsb(register unsigned v)
{
    register unsigned r = 0;

    while ((v & 0x01) == 0) {
        v >>= 1;
        r++;
    };

    return r;
}
/*---------------------------------------------------------------------------*/
unsigned bitarithm_bits_set(unsigned v)
{
    unsigned c; // c accumulates the total bits set in v

    for (c = 0; v; c++) {
        v &= v - 1; // clear the least significant bit set
    }

    return c;
}
