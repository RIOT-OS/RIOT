/*
 * Copyright (C) 2014 Loci Controls Inc.
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_cc2538dk
 * @{
 *
 * @file
 * @brief       Board specific implementations for the CC2538DK board
 *
 * @author      Ian Martin <ian@locicontrols.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "cpu.h"

/**
 * @brief Initialize the SmartRF06 board
 */
void board_init(void)
{
    /* initialize the boards LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_init(LED3_PIN, GPIO_OUT);
}
