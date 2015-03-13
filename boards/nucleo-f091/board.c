/*
 * Copyright (C) 2015  Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-f091
 * @{
 *
 * @file
 * @brief       Board specific implementations for the nucleo-f091 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
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
 * The green LED is connected to pin PA5
 */
static void leds_init(void)
{
    /* enable clock for port GPIOA */
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    /* set output speed to 50MHz */
    LED_GREEN_PORT->OSPEEDR |= 0x00000c00;
    /* set output type to push-pull */
    LED_GREEN_PORT->OTYPER &= ~(0x00000020);
    /* configure pin as general output */
    LED_GREEN_PORT->MODER &= ~(0x00000c00);
    LED_GREEN_PORT->MODER |= 0x00000400;
    /* disable pull resistors */
    LED_GREEN_PORT->PUPDR &= ~(0x00000c00);
    /* turn all LEDs off */
    LED_GREEN_PORT->BRR = 0x00c0;
}
