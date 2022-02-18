/*
 * Copyright (C) 2019 Robert Olsson <roolss@kth.se>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
