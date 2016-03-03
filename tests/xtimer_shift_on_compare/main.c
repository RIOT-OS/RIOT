/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       timer test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include "xtimer.h"
#include "periph_conf.h"

#define ITERATIONS 128
#define MAXSHIFT   8

static unsigned min[MAXSHIFT];
static unsigned max[MAXSHIFT];
static unsigned avg[MAXSHIFT];
static unsigned total[MAXSHIFT];

int main(void)
{
    puts("xtimer_shift_on_compare test application.\n");

    printf("trying %u iterations\n", ITERATIONS);

    uint32_t a, b;
    unsigned i = 0;

    for (int shift = 0; shift < MAXSHIFT; shift++) {
        min[shift] = (unsigned)0xffffffff;
        max[shift] = 0;
        total[shift] = 0;
        avg[shift] = 0;

        for (int j=0; j < ITERATIONS; j++) {
            i = 0;
            do {
                i++;
                if (i >= 0xF) {
                    break;
                }
                else {
                }

                a = _lltimer_now() | _high_cnt;
                b = _lltimer_now() | _high_cnt;
            } while ((a>>shift) != (b>>shift));

            min[shift] = i < min[shift] ? i : min[shift];
            max[shift] = i > max[shift] ? i : max[shift];
            if (i != 0xF) {
                total[shift]++;
                avg[shift] += i;
            }
        }
    }

    for (int i=0; i < MAXSHIFT; i++) {
        if (min[i] >= 0xF) {
            printf("shift %i is too little.\n", i);
        } else {
            printf("shift %i min=%u max=%u n_success=%u avg=%u\n", i, min[i], max[i], total[i], avg[i]/total[i]);
        }
    }

    printf("\nTest complete.\n");
    return 0;
}
