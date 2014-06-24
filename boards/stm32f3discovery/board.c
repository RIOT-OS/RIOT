/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_stm32f3discovery
 * @{
 *
 * @file
 * @brief       Board specific implementations for the STM32F3Discovery evaluation board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"

static void leds_init(void);

void board_init(void)
{
    /* initialize the boards LEDs */
    leds_init();

    /* initialize the CPU */
    cpu_init();
}

/**
 * @brief Initialize the boards on-board LEDs (LD3 to LD10)
 *
 * The LED initialization is hard-coded in this function. As the LEDs are soldered
 * onto the board they are fixed to their CPU pins.
 *
 * The LEDs are connected to the following pins:
 * - LD3:   PE9
 * - LD4:   PE8
 * - LD5:   PE10
 * - LD6:   PE15
 * - LD7:   PE11
 * - LD8:   PE14
 * - LD9:   PE12
 * - LD10:  PE13
 */
static void leds_init(void)
{
    /* enable clock for port GPIOE */
    RCC->AHBENR |= RCC_AHBENR_GPIOEEN;

    /* set output speed to 50MHz */
    LED_PORT->OSPEEDR |= 0xffff0000;
    /* set output type to push-pull */
    LED_PORT->OTYPER &= ~(0x0000ff00);
    /* configure pins as general outputs */
    LED_PORT->MODER &= ~(0xffff0000);
    LED_PORT->MODER |= 0x55550000;
    /* disable pull resistors */
    LED_PORT->PUPDR &= ~(0xffff0000);

    /* turn all LEDs off */
    LED_PORT->BRR = 0xff00;
}
