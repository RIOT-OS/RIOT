/**
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup     sys_div
 * @{
 * @file
 * @brief    Integer division function implementations
 *
 * @author   Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include "div.h"

uint64_t _div_mulhi64(const uint64_t a, const uint64_t b)
{
    /* Handle overflow explicit because we don't have 128 bit integers on
     * our platforms. */
    const uint32_t a_lo = (const uint32_t)a;
    const uint32_t a_hi = (const uint32_t)(a >> 32);
    const uint32_t b_lo = (const uint32_t)b;
    const uint32_t b_hi = (const uint32_t)(b >> 32);

    const uint64_t a_x_b_mid = (const uint64_t)a_hi * b_lo;
    const uint64_t b_x_a_mid = (const uint64_t)b_hi * a_lo;
    const uint64_t a_x_b_lo =  (const uint64_t)a_lo * b_lo;
    const uint64_t a_x_b_hi =  (const uint64_t)a_hi * b_hi;

    /* We may get up to 2 carry bits from the lower part of the multiplication */
    const uint32_t carry_bits = ((uint64_t)(uint32_t)a_x_b_mid +
                          (uint64_t)(uint32_t)b_x_a_mid +
                          (a_x_b_lo >> 32) ) >> 32;

    const uint64_t multhi = a_x_b_hi +
                      (a_x_b_mid >> 32) + (b_x_a_mid >> 32) +
                      carry_bits;

    return multhi;
}
