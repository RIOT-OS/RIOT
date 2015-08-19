/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_rf-motherboard-l1
 * @{
 *
 * @file
 * @brief       Board specific implementations for the nucleo-l1 board
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

static void leds_init(void);

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs */
    leds_init();
}

/**
 * @brief Initialize the boards on-board LEDs
 *
 * The LED initialization is hard-coded in this function. As the LED is soldered
 * onto the board it is fixed to its CPU pins.
 *
 * The green LED is connected to pin PA5
 */
static void leds_init(void)
{
    /* enable clock for port GPIOD */
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN;

    LED_GREEN_PORT->OSPEEDR |= 0x00000f0f;    // high speed
    LED_GREEN_PORT->OTYPER &= ~(0x00000033);  // push-pull
    LED_GREEN_PORT->MODER &= ~(0x00000f0f);   // set as general purpose output
    LED_GREEN_PORT->MODER |=   0x00000505;
    LED_GREEN_PORT->PUPDR &= ~(0x00000f0f);   // disable the pull-ups
    LED_GREEN_PORT->BSRRL = 0x00000033;        // turn the leds off

    LED_RED_PORT->OSPEEDR |= 0x00000030;      // high speed
    LED_RED_PORT->OTYPER &= ~(0x00000004);    // push-pull
    LED_RED_PORT->MODER &= ~(0x00000030);     // set general purpose output
    LED_RED_PORT->MODER |= 0x00000010;
    LED_RED_PORT->PUPDR &= ~(0x00000030);     // disable the pull-ups
    LED_RED_PORT->BSRRL = 0x00000010;          // turn the leds off
}
