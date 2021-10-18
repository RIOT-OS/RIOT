/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_stm32f723e-disco
 * @{
 *
 * @file
 * @brief       Board specific implementations for the STM32F723E-DISCO board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

#include <stdio.h>

void board_init(void)
{
    /* initialize the boards LEDs */
    /* NOTE: LED0 must be explicitly enabled as it is also used for SPI_DEV(0) */
#ifdef AUTO_INIT_LED0
    gpio_init(LED0_PIN, GPIO_OUT);
#endif
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);
}
