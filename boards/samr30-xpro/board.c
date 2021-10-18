/*
 * Copyright (C) 2017 Baptiste Clenet <bapclenet@gmail.com>
 *               2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

void led_init(void);

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
    /* initialize the boards LEDs */
    led_init();

    /* initialize the on-board antenna switch */
    gpio_init(RFCTL1_PIN, GPIO_OUT);
    gpio_init(RFCTL2_PIN, GPIO_OUT);
    /* set default antenna switch configuration */
    board_antenna_config(RFCTL_ANTENNA_DEFAULT);
}

/**
 * @brief Initialize the boards on-board LED
 */
void led_init(void)
{
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_set(LED0_PIN); /* gpio is inverted => clear */
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_set(LED1_PIN); /* gpio is inverted => clear */
}
