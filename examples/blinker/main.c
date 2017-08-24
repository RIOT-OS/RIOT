/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Blinker application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "xtimer.h"

#define BLINK_HALF_PERIOD  (250 * US_PER_MS)

int main(void)
{
    (void) puts("This is RIOT!");

#if defined(LED0_TOGGLE)
    while (1) {
        LED0_TOGGLE;
        xtimer_usleep(BLINK_HALF_PERIOD);
    }
#endif

    return 0;
}
