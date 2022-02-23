/*
 * Copyright (C) 2017 DAI Labor Technische Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_firefly
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Firefly board
 *
 * @author      Anon Mall <anon.mall@gt-arc.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "fancy_leds.h"

/**
 * @brief Initialize the boards on-board LEDs
 *
 * The LED initialization is hard-coded in this function. As the LED (RGB) are
 * soldered onto the board they are fixed to their CPU pins.
 */
void led_init(void)
{
    /* Shoot rainbows */
    LED_RAINBOW();
}
