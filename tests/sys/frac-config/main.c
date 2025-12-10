/*
 * SPDX-FileCopyrightText: 2018 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Frac library static configuration helper
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "frac.h"

int main(void)
{
    puts("frac library static configuration generator");
    while (1) {
        int res = 0;
        uint32_t num = 0;
        uint32_t den = 0;
        do {
            puts("Enter fraction numerator (multiplier):");
            res = scanf("%" SCNu32, &num);
        } while (res == 0);
        do {
            puts("Enter fraction denominator (divider):");
            res = scanf("%" SCNu32, &den);
        } while (res == 0);
        if (den == 0) {
            continue;
        }
        frac_t frac;
        frac_init(&frac, num, den);

        printf("Static initialization of frac_t for (%" PRIu32 " / %" PRIu32 "):\n",
            num, den);
        puts("(Copy and paste into your code)\n");
        printf("{ .frac = 0x%" PRIx32 ", .shift = %u }\n\n", frac.frac, (unsigned)frac.shift);
    }
    return 0;
}
