/*
 * SPDX-FileCopyrightText: 2018 SKF AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the DS3234 RTC driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "ds3234.h"
#include "ds3234_params.h"
#include "kernel_defines.h"

int main(void)
{
    puts("DS3234 RTC PPS test application\n");

    for (unsigned k = 0; k < ARRAY_SIZE(ds3234_params); ++k) {
        printf("Init #%u... ", k);
        int res = ds3234_pps_init(&ds3234_params[k]);
        if (res == 0) {
            puts("[OK]");
        }
        else {
            puts("[Failed]\n");
            return 1;
        }
    }

    puts("DS3234 init done.\n");
    puts("Check SQW pin on all connected DS3234 for a 1 Hz square wave signal\n");

    while (1) {
        /* Spin */
    }

    return 0;
}
