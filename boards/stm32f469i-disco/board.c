/*
 * Copyright (C) 2021 luisan00
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_stm32f469i-disco
 * @{
 *
 * @file
 * @brief       Board specific implementations for the STM32F469I-DISCO board
 *
 * @author      Luis A. Ruiz <luisan00@hotmail.com>
 * @}
 */
#include "board.h"
#include "periph/gpio.h"

#include <stdio.h>

void board_init(void)
{
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_init(LED3_PIN, GPIO_OUT);
}
