/*
 * Copyright (C) 2013 INRIA
 *               2017 Freie Universität Berlin
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

/* as default we run the test 100k times */
#ifndef TEST_ITER
#define TEST_ITER           (100000UL)
#endif

#define STEP                (0.1)

int main(void)
{
    double x = 1234567.0 / 1024.0;

    puts("Testing floating point arithmetic...\n");

    for (unsigned long i = 0; i < TEST_ITER; i++) {
        x += STEP;
        double z = (x - floor(x));
        if (z >= 1.0) {
            puts("[FAILED]");
            return 1;
        }
    }

    puts("[SUCCESS]");

    return 0;
}
