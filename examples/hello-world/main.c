/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 *
 * @}
 */

#include <stdio.h>
#include "periph/pm.h"
#include "periph/gpio.h"

#define WAKE_PIN                  GPIO_PIN(PORT_C, 13)
extern void pm_set(unsigned mode);
/* First callback, set_ups another isr */
void wake_cb(void)
{
    puts("Wakeup");
}


int main(void)
{
    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    gpio_init_int(WAKE_PIN, GPIO_IN, GPIO_FALLING, (gpio_cb_t)wake_cb, NULL);

    pm_set(1);

    while(1){

    }

    return 0;
}
