/*
 * Copyright (C) 2017, 2019 Ken Rabold, JP Bonn
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_hifive1b
 * @{
 *
 * @file
 * @brief       Support for the SiFive HiFive1b RISC-V board
 *
 * @author      Ken Rabold, JP Bonn
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* Initialize CPU and clocks */
    cpu_init();

    /* Configure GPIOs for LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);

    /* Turn all the LEDs off */
    LED0_OFF;
    LED1_OFF;
    LED2_OFF;
}
