/*
 * SPDX-FileCopyrightText: 2019 Robert Olsson <roolss@kth.se>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_avr-rss2
 * @{
 *
 * @file
 * @brief       Board specific LED initialization
 * @author      Robert Olsson <roolss@kth.se>
 *
 *
 * @}
 */

#include "board.h"

void led_init(void)
{
    /* Ports Pins as Output */
    LED_PORT_DDR |= LED1_MASK | LED0_MASK;
    /* All LEDs OFF */
    LED_PORT |= LED1_MASK | LED0_MASK;
}
