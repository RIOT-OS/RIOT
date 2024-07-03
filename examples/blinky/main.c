/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU GENERAL PUBLIC LICENSE.
 * See the file LICENSE in the top level directory for more details.
 *
 * The code below is based on the code here: https://devzone.nordicsemi.com/f/nordic-q-a/92877/bare-metal-programming-on-the-nrf9160
 */

/**
 * @file        main.c
 * @brief
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 */

#include <stdio.h>
#include "nrfx.h"
#include "secure_leds.h"

int main(void)
{
    puts("Wheeee \\o/");

    config_leds();
    while (1)
    {
        // In the main loop, we make each of the LEDs light up one at a time
        for (uint32_t i = 0; i < 4; ++i)
        {
            led_on(i);

            // Some delay
            volatile uint32_t j = 1000000;
            while (j > 0) {
                --j;
            }
            led_off(i);
        }
    }

    return 0;
}
