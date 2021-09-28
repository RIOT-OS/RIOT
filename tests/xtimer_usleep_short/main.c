/*
 * Copyright (C) 2017 HAW-Hamburg
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
 * @brief       xtimer_usleep_short test application
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @}
 */
#include <stdio.h>
#include "ztimer.h"

#define TEST_USLEEP_MIN (0)
#define TEST_USLEEP_MAX (500)

int main(void)
{
    ztimer_sleep(ZTIMER_MSEC, 3 * 1000);
    printf("This test will call xtimer_usleep for values from %d down to %d\n",
            TEST_USLEEP_MAX, TEST_USLEEP_MIN);

    for (int i = TEST_USLEEP_MAX; i >= TEST_USLEEP_MIN; i--) {
        printf("going to sleep %d usecs...\n", i);
        ztimer_sleep(ZTIMER_USEC, i);
    }

    puts("[SUCCESS]");

    return 0;
}
