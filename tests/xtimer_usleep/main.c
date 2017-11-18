/*
 * Copyright (C) 2017 Inria
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
 * @brief       xtimer_usleep test application
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 * @}
 */

#include <inttypes.h>
#include <stdio.h>

#include "xtimer.h"
#include "timex.h"

#define RUNS                (5U)
#define SLEEP_TIMES_NUMOF   (sizeof(sleep_times) / sizeof(sleep_times[0]))

static const uint32_t sleep_times[] = { 10000, 50000, 100000 };

int main(void)
{
    uint32_t start_test, testtime;

    printf("Running test %u times with %u distinct sleep times\n", RUNS,
           (unsigned)SLEEP_TIMES_NUMOF);
    puts("Please hit any key and then ENTER to continue");
    getchar();
    start_test = xtimer_now_usec();
    for (unsigned m = 0; m < RUNS; m++) {
        for (unsigned n = 0;
             n < sizeof(sleep_times) / sizeof(sleep_times[0]);
             n++) {
            uint32_t diff, start;
            start = xtimer_now_usec();
            xtimer_usleep(sleep_times[n]);
            diff = xtimer_now_usec() - start;
            printf("Slept for %" PRIu32 " us (expected: %" PRIu32 " us)\n",
                   diff, sleep_times[n]);
        }
    }
    testtime = xtimer_now_usec() - start_test;
    printf("Test ran for %" PRIu32 " us\n", testtime);

    return 0;
}
