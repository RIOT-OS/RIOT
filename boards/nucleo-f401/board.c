/*
 * Copyright (C) 2015 Lari Lehtomäki
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-f401
 * @{
 *
 * @file
 * @brief       Board specific implementations for the nucleo-f401 board
 *
 * @author      Lari Lehtomäki <lari@lehtomaki.fi>
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
 * @brief Initialize the boards on-board LED
 *
 * The LED initialization is hard-coded in this function. As the LED is
 * soldered onto the board it is fixed to its CPU pins.
 *
 * The green LED is connected to pin PA5
 */
static void leds_init(void)
{
    /* enable clock for port GPIOA */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    /* set output speed to 50MHz */
    LED_GREEN_PORT->OSPEEDR |= (0x3 << LED_GREEN_PIN * 2);
    /* set output type to push-pull */
    LED_GREEN_PORT->OTYPER &= ~(1 << LED_GREEN_PIN);
    /* configure pins as general outputs */
    LED_GREEN_PORT->MODER &= ~(0x3 << LED_GREEN_PIN * 2);
    LED_GREEN_PORT->MODER |= (0x1 << LED_GREEN_PIN * 2);
    /* disable pull resistors */
    LED_GREEN_PORT->PUPDR &= ~(0x3 << LED_GREEN_PIN * 2);
    /* turn all LEDs off */
    LED_GREEN_PORT->BSRRL &= (1 << LED_GREEN_PIN);

}
