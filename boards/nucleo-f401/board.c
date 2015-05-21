/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Adam Berlinger
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_nucleo-f401
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Nucleo F401RE evaluation board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Adam Berlinger <berlingeradam@gmail.com>
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
 * @brief Initialize the boards on-board LEDs (LD2)
 *
 * The LED initialization is hard-coded in this function. As the LEDs are soldered
 * onto the board they are fixed to their CPU pins.
 *
 * The LEDs are connected to the following pins:
 * - LD2: PA5
 */
static void leds_init(void)
{
    /* enable clock for port GPIOD */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    /* configure pins as general outputs */
    LED_PORT->MODER &= ~(uint32_t)(0xC00);
    LED_PORT->MODER |= (uint32_t)(0x400);
    /* set output speed high-speed */
    LED_PORT->OSPEEDR |= (uint32_t)(0xC00);
    /* set output type to push-pull */
    LED_PORT->OTYPER &= ~(uint32_t)(0x20);
    /* disable pull resistors */
    LED_PORT->PUPDR &= ~(uint32_t)(0xC00);

    /* turn LED off */
    LED_PORT->BSRRH = 0x20;
}
