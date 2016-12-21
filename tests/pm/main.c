/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       Power Management test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "periph/gpio.h"
#include "pm.h"
#include "xtimer.h"

#define SLEEP (5U)

static void cb(void *arg)
{
    puts("*");
}

int main(void)
{
    printf("PM test application started.\n"
           "Will now alternate between sleeping and spinning\n"
           "every %u seconds.\n"
           "While sleeping, the LED is off, while spinning, the led is on.\n"
           "Every four cycles, one more power mode will be unlocked.\n"
           "The board's first button can be used to generate an interrupt,\n"
           "(which just prints \"*\").\n",
            SLEEP);

    puts("initializing button...");
    gpio_init_int(BUTTON_GPIO, GPIO_IN_PU, GPIO_RISING, cb, NULL);

    unsigned mode = PM_NUM_MODES;
    unsigned n = 1;
    while(1) {
        if ((n % 4) == 0) {
            mode--;
            printf("unblocking mode %u\n", mode);
            pm_unblock(mode);
        }
        if (n++ & 1) {
            printf("sleeping %u\n", SLEEP);
            LED0_OFF;
            xtimer_sleep(SLEEP);
        }
        else {
            printf("spinning %u\n", SLEEP);
            LED0_ON;
            xtimer_spin(SLEEP * (1000000));
        }
    }

    return 0;
}
