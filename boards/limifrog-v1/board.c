/*
 * Copyright (C) 2015 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_limifrog-v1
 * @{
 *
 * @file
 * @brief       Board specific implementations for the limifrog-v1 board
 *
 * @author      Katja Kirstein <katja.kirstein@haw-hamburg.de>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

static void leds_init(void);

void board_init(void)
{
    /* initialize the boards LEDs */
    leds_init();
    /* initialize the CPU */
    cpu_init();
}

/**
 * @brief Initialize the boards on-board LEDs
 *
 * The LED initialization is hard-coded in this function.
 * As the LED is soldered onto the board it is fixed to
 * its CPU pins.
 *
 * The red LED is connected to pin PC3
 */
static void leds_init(void)
{
    /* enable clock for port GPIOC */
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    /* set output speed to 50MHz */
    LED_RED_PORT->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR3;
    /* set output type to push-pull */
    LED_RED_PORT->OTYPER &= ~(GPIO_OTYPER_OT_3);
    /* configure pin as general output */
    LED_RED_PORT->MODER &= ~(GPIO_MODER_MODER3);
    LED_RED_PORT->MODER |= GPIO_MODER_MODER3_0;
    /* disable pull resistors */
    LED_RED_PORT->PUPDR &= ~(GPIO_PUPDR_PUPDR3);
    /* turn all LEDs off */
    LED_RED_PORT->BRR = (1 << 3);
}
