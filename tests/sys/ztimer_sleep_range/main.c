/*
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     test
 * @{
 *
 * @file
 * @brief       ztimer sleep range test application
 *
 * @author      Snehal Verma <snehalverma10@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include "ztimer.h"
#include "timex.h"

#define RUNS                5U
#define SLEEP_TIMES_NUMOF   2U

// In microseconds
static const uint32_t sleep_times[][2] = {
    {10000, 11000},
    {50000, 51000},
};

int main(void)
{
    printf("Running test %u times with %u distinct sleep time ranges\n",
            RUNS, SLEEP_TIMES_NUMOF);

    uint32_t start_test = ztimer_now(ZTIMER_USEC);

    for (unsigned m = 0; m < RUNS; m++) {
        for (unsigned n = 0; n < SLEEP_TIMES_NUMOF; n++) {
            uint32_t start_sleep = ztimer_now(ZTIMER_USEC);
            ztimer_sleep_range(sleep_times[n][0], sleep_times[n][1]);
            uint32_t slept = ztimer_now(ZTIMER_USEC) - start_sleep;
            printf("Slept between %" PRIu32 " us and %" PRIu32 " us, actually slept %" PRIu32 " us\n",
                   sleep_times[n][0], sleep_times[n][1], slept);
        }
    }

    printf("Test ran for %" PRIu32 " us\n",
            ztimer_now(ZTIMER_USEC) - start_test);

    return 0;
}
