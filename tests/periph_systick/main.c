/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
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
 * @brief       SysTick timer test application
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "board.h"

#include "mutex.h"
#include "systick.h"
#include "macros/units.h"

#include "timex.h"

#define INTERVAL_US         ( 2 * US_PER_SEC)
#define INTERVAL_LONG_US    (30 * US_PER_SEC)

static mutex_t lock = MUTEX_INIT_LOCKED;

void systick_callback(void)
{
    mutex_unlock(&lock);
}

static void test_timer_set(unsigned period, unsigned iterations)
{

    systick_timer_init(MHZ(1));

    while (iterations--) {
        if (systick_timer_set(period)) {
            puts("too large offset");
            return;
        }
        mutex_lock(&lock);
        printf("now: %u\n", systick_timer_read());
    }
}

int main(void)
{
    printf("Test setting %lu ms intervals\n", INTERVAL_US / US_PER_MS);
    test_timer_set(INTERVAL_US, 5);

    printf("Test setting %lu s intervals\n", INTERVAL_LONG_US / US_PER_SEC);
    test_timer_set(INTERVAL_LONG_US, 5);

    return 0;
}
