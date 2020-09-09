/*
 * Copyright (C) 2020 Scallog
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_olimexino-stm32
 * @{
 *
 * @file
 * @brief       Board specific implementations for the OLIMEXINO STM32 board
 *
 * @author      Corentin Vigourt <cvigourt@scallog.com>
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);

    /* initialize the button */
    gpio_init(BTN0_PIN, BTN0_MODE);
}
/** @} */
