/*
 * Copyright (C) 2008, 2009, 2010 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
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
 * @brief       Default application that shows a lot of functionality of RIOT
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "ztimer.h"
#include "periph/gpio.h"


int main(void)
{
    //GPIO_IN, GPIO_IN_PD, GPIO_IN_PU, GPIO_OUT, GPIO_OD, GPIO_OD_PU
    int pin_led = 0;
    int pin_button = 0;
    bool led_state = false;

    gpio_init(pin_led, GPIO_OUT);
    gpio_init(pin_button, GPIO_IN_PU);
    

    while (1) {
        gpio_write(pin_led, led_state);
        bool state = gpio_read(pin_button);

        if (state) {
            printf("here\n");
        }


        //ZTIMER_MSEC / ZTIMER_USEC
        ztimer_sleep(ZTIMER_MSEC, 1);    
    }

    return 0;
}
