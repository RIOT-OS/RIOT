/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test input and output functionality of low-level GPIO driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "cpu.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "hwtimer.h"

/* only compile this if at least one GPIO device is defined */
#if GPIO_NUMOF

uint32_t state = 0;
uint32_t old_state = 0;

int main(void)
{
    puts("GPIO driver test");


    puts("Setting GPIOs to output w/o pull resistor");
    for (int i = 0; i < GPIO_NUMOF; i++) {
        gpio_init_out(i, GPIO_NOPULL);
    }

    puts("Turning pins 1s on");
    for (int i = 0; i < GPIO_NUMOF; i++) {
        gpio_set(i);
    }
    hwtimer_wait(1000000);


    puts("Turning pins 1s off");
    for (int i = 0; i < GPIO_NUMOF; i++) {
        gpio_clear(i);
    }
    hwtimer_wait(1000000);

    puts("Now toggling pins 6 times with 500ms interval");
    for (int c = 0; c < 6; c++) {
        for (int i = 0; i < GPIO_NUMOF; i++) {
            gpio_toggle(i);
        }
        hwtimer_wait(500000);
    }


    puts("\n--------------------------------------------\n");
    puts("Setting GPIOs to input w/o pull resister now");
    for (int i = 0; i < GPIO_NUMOF; i++) {
        gpio_init_in(i, GPIO_NOPULL);
    }

    puts("Will now poll all pins and print status on change");

    while (1) {
        state = 0;
        for (int i = 0; i < GPIO_NUMOF; i++) {
            if (gpio_read(i)) {
                state |= (1 << i);
            }
        }
        if (state != old_state) {
            for (int i = 0; i < GPIO_NUMOF; i++) {
                if (state & (1 << i)) {
                    printf("GPIO_%i:H  ", i);
                } else {
                    printf("GPIO_%i:L  ", i);
                }
            }
            printf("\n");
            old_state = state;
        }
    }

    return 0;
}

#else

int main(void)
{
    puts("No low-level GPIO device defined for this platform.");

    return 0;
}

#endif /* GPIO_NUMOF */
