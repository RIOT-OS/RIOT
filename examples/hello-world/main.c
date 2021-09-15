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
#include "periph/gpio.h"
#include "periph/rtt.h"
#include "xtimer.h"
#include "cpu.h"

int main(void)
{
    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    uint32_t tmp;
    gpio_init(GPIO_PIN(PORT_A, 8), GPIO_OUT);
    gpio_clear(GPIO_PIN(PORT_A, 8));
    printf("%i\n", RTT_FREQUENCY);
    tmp = RCC->ICSCR;
    tmp &= ~0xFF000000;
    RCC->ICSCR = tmp;
    gpio_set(GPIO_PIN(PORT_A, 8));
    xtimer_sleep(10);
    gpio_clear(GPIO_PIN(PORT_A, 8));
    xtimer_sleep(1);
    tmp = RCC->ICSCR;
    tmp &= ~0xFF000000;
    tmp |= 0xFF000000;
    RCC->ICSCR = tmp;
    gpio_set(GPIO_PIN(PORT_A, 8));
    xtimer_sleep(10);
    gpio_clear(GPIO_PIN(PORT_A, 8));
    return 0;
}
