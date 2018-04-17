/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_frdm-kw41z
 * @{
 *
 * @file
 * @brief       Board specific initialization for the FRDM-KW41Z
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the CPU core */
    cpu_init();

    /* initialize and turn off LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_set(LED0_PIN);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_set(LED1_PIN);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_set(LED2_PIN);
    gpio_init(LED3_PIN, GPIO_OUT);
    gpio_set(LED3_PIN);
}
