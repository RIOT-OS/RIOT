/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_nrf52-previewdk
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the nRF52832 preview development kit board
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Jan Wagner <mail@jwagner.eu>
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
 * @brief Initialize the boards on-board LEDs
 *
 * The LED initialization is hard-coded in this function.
 *
 * The 4 LEDs are connected to the following pins:
 * - ONE:    17
 * - TWO:    18
 * - THREE:  19
 * - FOUR:   20
 */
void leds_init(void)
{
    /* set LED pins to function as output */
    NRF_P0->DIRSET = (LED_ONE_PIN | LED_TWO_PIN | LED_THREE_PIN | LED_FOUR_PIN);

    /* turn all LEDs off (low active) */
    NRF_P0->OUTSET = (LED_ONE_PIN | LED_TWO_PIN | LED_THREE_PIN | LED_FOUR_PIN);
}
