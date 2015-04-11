/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_stm32f3discovery
 * @{
 *
 * @file
 * @brief       Board specific implementations for the STM32F4Discovery evaluation board
 *
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
 * @brief Initialize the boards on-board LEDs (LD3 and LD4)
 *
 * The LED initialization is hard-coded in this function. As the LEDs are soldered
 * onto the board they are fixed to their CPU pins.
 *
 * The LEDs are connected to the following pins:
 * - LD3: PD13
 * - LD4: PD12
 * - LD5: PD14
 * - LD6: PD15
 */
static void leds_init(void)
{
    /* enable clock for port GPIOD */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    /* configure pins as general outputs */
    LED_PORT->MODER &= ~(0xff000000);
    LED_PORT->MODER |= 0x55000000;
    /* set output speed high-speed */
    LED_PORT->OSPEEDR |= 0xff000000;
    /* set output type to push-pull */
    LED_PORT->OTYPER &= ~(0xf000);
    /* disable pull resistors */
    LED_PORT->PUPDR &= ~(0xff000000);

    /* turn all LEDs off */
    LED_PORT->BSRRH = 0xf000;
}
