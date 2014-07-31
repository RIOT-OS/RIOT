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
 * @author      Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "bitarithm.h"

unsigned bitarithm_msb(unsigned v)
{
    if ((signed) v >= 0) {
        v &= -v;
        return bitarithm_lsb(v);
    }
    else {
        return sizeof (v) * 8 - 1;
    }
}

unsigned bitarithm_lsb(unsigned v)
{
    unsigned r = 0;
    while ((v & 1) == 0) {
        v >>= 1;
        r++;
    }
    return r;
}

unsigned bitarithm_bits_set(unsigned v)
{
    unsigned c; // c accumulates the total bits set in v

    for (c = 0; v; c++) {
        v &= v - 1; // clear the least significant bit set
    }

    return c;
}
