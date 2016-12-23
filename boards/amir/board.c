/*
 * Copyright (C) 2016 Seyed Amir Alavi
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_Amir
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Amir board
 *
 * @author      Seyed Amir Alavi <seydamir@gmail.com>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"


void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs and turn them off */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_set(LED0_PIN);
    gpio_set(LED1_PIN);
    gpio_set(LED2_PIN);
}
