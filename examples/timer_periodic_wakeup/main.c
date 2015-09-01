/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     xtimer_examples
 * @{
 *
 * @file
 * @brief       example application for setting a periodic wakeup
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include "xtimer.h"

/* set interval to 1 second */
#define INTERVAL (1000000U)

int main(void)
{
    uint32_t last_wakeup = xtimer_now();

    while(1) {
        xtimer_usleep_until(&last_wakeup, INTERVAL);
        printf("slept until %"PRIu32"\n", xtimer_now());
    }

    return 0;
}
