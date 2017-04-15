/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu
 * @{
 *
 * @file
 * @brief       MSPGCC supplemental functions
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se
 *
 * @}
 */

#ifdef __MSPGCC__

/* internal GCC type for "half integer" (16 bit) */
/* See also:
 * http://stackoverflow.com/questions/4559025/what-does-gcc-attribute-modexx-actually-do
 * http://www.delorie.com/gnu/docs/gcc/gccint_53.html
 */
typedef unsigned int UHItype __attribute__ ((mode (HI)));

/**
 * @brief Count leading zeros
 *
 * Naive implementation
 */
int __clzhi2(UHItype x)
{
    int i = 0;
    for (UHItype mask = (1 << 15); mask != 0; mask >>= 1) {
        if (x & mask) {
            return i;
        }
        ++i;
    }
    return i; /* returns 16 if x == 0 */
}

/**
 * @brief Count trailing zeros
 *
 * Naive implementation
 */
int __ctzhi2(UHItype x)
{
    int i = 0;
    for (UHItype mask = 1; mask != 0; mask <<= 1) {
        if (x & mask) {
            return i;
        }
        ++i;
    }
    return i; /* returns 16 if x == 0 */
}
#endif /* __MSPGCC__ */
