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
#include <vtimer.h>

int main(void)
{
    puts("Test Starting.\n");
    timex_t last_wakeup, interval = { 1, 0 };
    vtimer_now(&last_wakeup);

    while(1) {
        vtimer_sleep_until(&last_wakeup, interval);
        puts("Woke up.");
    }

    return 0;
}
