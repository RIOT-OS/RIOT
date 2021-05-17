/*
 * Copyright (C) 2019 Christian Amsüss <chrysn@fsfe.org>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf52840dongle-flocklab
 * @{
 *
 * @file
 * @brief       Board initialization for the nRF52840-dongle
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the board's single LED */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_set(LED0_PIN);

    /* initialize the board's RGB LED */
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_set(LED1_PIN);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_set(LED2_PIN);
    gpio_init(LED3_PIN, GPIO_OUT);
    gpio_set(LED3_PIN);

    /* initialize the flocklab RGB LED */
    gpio_init(FLOCKLAB_LED1_PIN, GPIO_OUT);
    gpio_set(FLOCKLAB_LED1_PIN);
    gpio_init(FLOCKLAB_LED2_PIN, GPIO_OUT);
    gpio_set(FLOCKLAB_LED2_PIN);
    gpio_init(FLOCKLAB_LED3_PIN, GPIO_OUT);
    gpio_set(FLOCKLAB_LED3_PIN);

    /* initialize the output pins flocklab monitors */
    gpio_init(FLOCKLAB_INT1, GPIO_OUT);
    gpio_clear(FLOCKLAB_INT1);
    gpio_init(FLOCKLAB_INT2, GPIO_OUT);
    gpio_clear(FLOCKLAB_INT2);

    /* initialize the input pin flocklab can provide input to */
    gpio_init(FLOCKLAB_SIG1, GPIO_IN);
    gpio_init(FLOCKLAB_SIG2, GPIO_IN);

    /* initialize the CPU */
    cpu_init();
}
