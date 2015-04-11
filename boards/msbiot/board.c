/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_msbiot
 * @{
 *
 * @file
 * @brief       Board specific implementations for the MSB-IoT board
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 *
 * @}
 */

#include "board.h"

static void leds_init(void);

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs */
    leds_init();
}

/**
 * @brief Initialize the boards standard LEDs (RED, YELLOW, GREEN)
 *
 * The LED initialization is hard-coded in this function. As the LEDs are soldered
 * onto the board they are fixed to their CPU pins.
 *
 * The LEDs are connected to the following pins:
 * - LED RED:    PB8
 * - LED YELLOW: PB14
 * - LED GREEN:  PB15
 */
static void leds_init(void)
{
    /* enable clock for port GPIOB */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    /* set output speed to 50MHz */
    LED_PORT->OSPEEDR &= ~(0xF0030000);
    LED_PORT->OSPEEDR |= 0xA0020000;
    /* set output type to push-pull */
    LED_PORT->OTYPER &= ~(0x0000C100);
    /* configure pins as general outputs */
    LED_PORT->MODER &= ~(0xF0030000);
    LED_PORT->MODER |= 0x50010000;
    /* disable pull resistors */
    LED_PORT->PUPDR &= ~(0xF0030000);

    /* turn all LEDs off */
    LED_PORT->BSRRL = 0xC100;
}
