/*
 * SPDX-FileCopyrightText: 2013 INRIA
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Float test application
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <math.h>

#include "board.h"

/* as default we run the test 1k times */
#ifndef TEST_ITER
#define TEST_ITER           (1000UL)
#endif

#define FLOAT_STEP          (0.1)

int main(void)
{
    double x = 1234567.0 / 1024.0;

    puts("Testing floating point arithmetic...\n");

    for (unsigned long i = 0; i < TEST_ITER; i++) {
        x += FLOAT_STEP;
        double z = (x - floor(x));
        if (z >= 1.0) {
            puts("[FAILED]");
            return 1;
        }
    }

    puts("[SUCCESS]");

    return 0;
}
