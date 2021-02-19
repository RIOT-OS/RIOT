/*
 * Copyright (C) 2021 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stm32g0316-disco
 * @{
 *
 * @file
 * @brief       Board specific implementations for the STM32G0316-DISCO board
 *
 * @author      Dave VanKampen <david.vankampen@bissell.com>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    gpio_init(LED0_PIN, LED0_MODE);
    gpio_set(LED0_PIN);

    gpio_init(BTN0_PIN, BTN0_MODE);
}
