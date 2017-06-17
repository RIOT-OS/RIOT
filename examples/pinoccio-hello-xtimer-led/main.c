/*
 * Copyright (C) 2016 RWTH Aachen, Josua Arndt
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
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @}
 */
#include <stdio.h>

#include "xtimer.h"
#include "timex.h"
#include "rgbled.h"

#include "board.h"




#include <stdlib.h>

/* set interval to 1 second */
#define INTERVAL (1U * US_PER_SEC)

int main(void)
{
    puts("Pinoccio Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    xtimer_ticks32_t last_wakeup = xtimer_now();

    while(1) {
    	xtimer_periodic_wakeup(&last_wakeup, INTERVAL);
        printf("slept until %" PRIu32 "\n", xtimer_usec_from_ticks(xtimer_now()));
        // Toggle red led
        LED_PORT ^= RED;
    }
    return 0;
}
