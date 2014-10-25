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
    timex_t last_wakeup, interval = { 0, 2000 };
    vtimer_now(&last_wakeup);

    uint32_t pre, after;

    while(1) {
        pre = hwtimer_now();
        vtimer_sleep_until(&last_wakeup, interval);
        after = hwtimer_now();
        printf("Woke up. diff: %i.\n", after-pre);
    }

    return 0;
}
