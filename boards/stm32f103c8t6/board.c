/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_stm32f130c8t6
 * @{
 *
 * @file
 * @brief       Board specific implementations for the STM32F103C8T6 demo board
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

#include <stdio.h>

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LED */
    gpio_init(LED0_PIN, GPIO_OUT);
}
