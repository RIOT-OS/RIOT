/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_rf231usb
 * @{
 *
 * @file
 * @brief       Board specific implementations for the rf231usb
 *
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "cpu.h"

/**
 * @brief Initialize the boards on-board LEDs (red/orange/green)
 *
 * See board.h for pin assignments.
 *
 */
void led_init(void)
{
    /* disable interrupts */
    LED_PORT->PIO_IDR   = LED_ALL_PINS;
    /* enable pull-up */
    LED_PORT->PIO_PUDR  = LED_ALL_PINS;
    /* disable multi-drive */
    LED_PORT->PIO_MDDR  = LED_ALL_PINS;
    /* enable initial state up (not on) */
    LED_PORT->PIO_SODR  = LED_ALL_PINS;
    /* enable output write */
    LED_PORT->PIO_OWER  = LED_ALL_PINS;
    /* set as output pin */
    LED_PORT->PIO_OER   = LED_ALL_PINS;
    /* enable PIO control */
    LED_PORT->PIO_PER   = LED_ALL_PINS;
}

void board_init(void)
{
    /* initialize clocks */
    clock_init();

    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs */
    led_init();

    /* turn the green LED on */
    LED_GREEN_ON;
}
