/*
 * SPDX-FileCopyrightText: 2020 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
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

static void _callback(void *arg)
{
    (void)arg;
}

static xtimer_t t1 = { .callback=_callback };
static xtimer_t t2 = { .callback=_callback };

int main(void)
{
    /* ensure that xtimer_now64() is greater than UINT32_MAX
     * and the upper 32bit of xtimer_now64() equal 1 */
    _xtimer_current_time = (1LLU << 32U);
    xtimer_init();

    /* set to 100s (far in the future) */
    xtimer_set(&t1, 100000000LU);
    xtimer_set(&t2, 100000000LU);

    /* sleep 1 ms (uses a third xtimer internally).
     * shooting this timer did set all following short timer's to have a
     * long_start_time of 0.  The timer_callback main loop would correct the
     * timer_list_head, but not the following.
     */
    xtimer_usleep(1000);

    if (t2.long_start_time == 1) {
        puts("[SUCCESS]");
    }
    else {
        puts("[FAILED]");
    }

    return 0;
}
