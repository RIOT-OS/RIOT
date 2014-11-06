/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_f4vi1
 * @{
 *
 * @file
 * @brief       Board specific implementations for the f4vi1 board
 *
 * @author      Stefan Pfeiffer <pfeiffer@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"

static void leds_init(void);

void board_init(void)
{
    /* initialize the boards LEDs, this is done first for debugging purposes */
    leds_init();

    /* initialize the CPU */
    cpu_init();
}

/**
 * @brief Initialize the boards on-board LEDs (LD4,LD5 and LD6)
 *
 * The LED initialization is hard-coded in this function. As the LEDs are soldered
 * onto the board they are fixed to their CPU pins.
 *
 * The LEDs are connected to the following pins:
 * - LD4: PA 1
 * - LD5: PA 3
 * - LD6: PA 2
 */
static void leds_init(void)
{
    /* enable clock for port GPIOD */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    /* configure pins as general outputs */
    LED_PORT->MODER &= ~(0x000000FC);
    LED_PORT->MODER |= 0x00000054;
    /* set output speed high-speed */
    LED_PORT->OSPEEDR |= 0x000000FC;
    /* set output type to push-pull */
    LED_PORT->OTYPER &= ~(0x000E);
    /* disable pull resistors */
    LED_PORT->PUPDR &= ~(0x000000FC);

    /* turn all LEDs off */
    LED_PORT->BSRRL = 0x00E;
}
