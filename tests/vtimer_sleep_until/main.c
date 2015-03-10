/*
 * Copyright (C) 2014 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief vtimer_sleep_until test application
 *
 * @author Kaspar Schleiser <kaspar@schleiser.de> 
 *
 * @}
 */

#include <stdio.h>
#include <hwtimer.h>
#include <vtimer.h>

int main(void)
{
    puts("Test Starting.\n");
    timex_t last_wakeup = { 0 };
    timex_t interval = { 1, 0 };
/*    vtimer_now(&last_wakeup); */

    uint32_t pre, after;

    /* sleep so vtimer_now gets significantly larger than 0. */
    vtimer_usleep(1000000);

    while(1) {
        pre = hwtimer_now();
        /* first call should return immediately if now wasn't initialized */
        vtimer_sleep_until(&last_wakeup, interval);
        after = hwtimer_now();
        printf("Woke up. diff: %"PRIu32".\n", after-pre);
    }

    return 0;
}
