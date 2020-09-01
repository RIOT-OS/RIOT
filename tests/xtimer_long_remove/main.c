/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       timer test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "msg.h"
#include "thread.h"
#include "xtimer.h"

#define NUMOF (3U)

int main(void)
{
    puts("[START]: xtimer_long_remove");

    for (unsigned int n = 0; n < NUMOF; n++) {
        printf("Setting %u timers, removing timer %u/%u\n", NUMOF, n, NUMOF);
        xtimer_t timers[NUMOF];
        for (unsigned int i = 0; i < NUMOF; i++) {
            xtimer_set64(&timers[i], ((uint64_t)1) << 32);
        }

        xtimer_remove(&timers[n]);

        /* timers that are already fired should be reset (0)*/
        if ((timers[n].offset) ||
            (timers[n].long_offset) ||
            (timers[n].start_time) ||
            (timers[n].long_start_time)) {
            return -1;
        }

    }

    puts("[SUCCESS]: xtimer_long_remove");

    return 0;
}
