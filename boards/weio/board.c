/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_weio
 * @{
 *
 * @file
 * @brief       Board specific implementations for the WeIO board
 *
 * @author      Paul RATHGEB <paul.rathgeb@skynet.be>
 *
 * @}
 */

#include "board.h"

static void leds_init(void);
extern void SystemInit(void);

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();
    /* initialize the boards LEDs */
    leds_init();
}

/**
 * @brief Initialize the boards on-board LEDs (LED1 to LED4)
 *
 * The LED initialization is hard-coded in this function. As the LEDs are soldered
 * onto the board they are fixed to their CPU pins.
 *
 * The LEDs are connected to the following pins:
 * - LED1: P1.13
 * - LED2: P1.14
 * - LED3: P1.15
 */
static void leds_init(void)
{
    /* configure LED pins as output */
    LED_PORT->DIR[1] = (LED1_PIN | LED2_PIN | LED3_PIN);

    /* clear all LEDs */
    LED_PORT->SET[1] = (LED1_PIN | LED2_PIN | LED3_PIN);
}
