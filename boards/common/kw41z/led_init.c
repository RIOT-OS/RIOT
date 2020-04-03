/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_kw41z
 * @{
 *
 * @file
 * @brief       Common led initialization for KW41Z based boards
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "periph/gpio.h"

void led_init(void)
{
    /* initialize and turn off LEDs */
#ifdef LED0_PIN
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_set(LED0_PIN);
#endif
#ifdef LED1_PIN
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_set(LED1_PIN);
#endif
#ifdef LED2_PIN
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_set(LED2_PIN);
#endif
#ifdef LED3_PIN
    gpio_init(LED3_PIN, GPIO_OUT);
    gpio_set(LED3_PIN);
#endif
}
