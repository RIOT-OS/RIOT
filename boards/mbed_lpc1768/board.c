/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_mbed_lpc1768
 * @{
 *
 * @file
 * @brief       Board specific implementations for the mbed LPC1768 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"

static void leds_init(void);
extern void SystemInit(void);

void board_init(void)
{
    /* initialize core clocks via CMSIS function */
    SystemInit();
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
 * - LED1: P1.18
 * - LED2: P1.20
 * - LED3: P1.21
 * - LED4: P1.23
 */
static void leds_init(void)
{
    /* configure LED pins as output */
    LED_PORT->FIODIR |= (LED0_MASK | LED1_MASK | LED2_MASK | LED3_MASK);

    /* clear all LEDs */
    LED_PORT->FIOCLR = (LED0_MASK | LED1_MASK | LED2_MASK | LED3_MASK);
}
