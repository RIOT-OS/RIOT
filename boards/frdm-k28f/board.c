/*
 * Copyright (C) 2020 Koen Zandberg
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_frdm-k28f
 * @{
 *
 * @file
 * @brief       Board specific implementations for the FRDM-K28F
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the CPU core */
    cpu_init();

    /* initialize and turn off the on-board RGB-LED */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_clear(LED0_PIN);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_clear(LED1_PIN);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_clear(LED2_PIN);
}
