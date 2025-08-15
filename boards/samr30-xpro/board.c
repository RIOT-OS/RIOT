/*
 * SPDX-FileCopyrightText: 2017 Baptiste Clenet <bapclenet@gmail.com>
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_samr30-xpro
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the Atem SAM R30 Xplained Pro board
 *
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "periph/gpio.h"

void board_antenna_config(uint8_t antenna)
{
    if (antenna == RFCTL_ANTENNA_EXT){
        gpio_clear(RFCTL1_PIN);
        gpio_set(RFCTL2_PIN);
    }
    else if (antenna == RFCTL_ANTENNA_BOARD){
        gpio_set(RFCTL1_PIN);
        gpio_clear(RFCTL2_PIN);
    }
}

void board_init(void)
{
    /* initialize the on-board antenna switch */
    gpio_init(RFCTL1_PIN, GPIO_OUT);
    gpio_init(RFCTL2_PIN, GPIO_OUT);
    /* set default antenna switch configuration */
    board_antenna_config(RFCTL_ANTENNA_DEFAULT);
}
