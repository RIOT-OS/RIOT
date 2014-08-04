/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_pca10000
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the nRF51822 evaluation board pca10000
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include "board.h"
#include "cpu.h"
#include "periph/uart.h"

extern void SystemInit(void);

void leds_init(void);

void board_init(void)
{
    /* initialize the boards LEDs */
    leds_init();

    /* initialize the CPU */
    cpu_init();
}

/**
 * @brief Initialize the boards on-board RGB LED
 *
 * The LED initialization is hard-coded in this function.
 *
 * The LED channels are connected to the following pins:
 * - RED:   21
 * - GREEN: 22
 * - BLUE:  23
 */
void leds_init(void)
{
    /* set LED pins to function as output */
    NRF_GPIO->DIRSET = (LED_RED_PIN | LED_GREEN_PIN | LED_BLUE_PIN);

    /* turn all LEDs off (low active) */
    NRF_GPIO->OUTSET = (LED_RED_PIN | LED_GREEN_PIN | LED_BLUE_PIN);
}
