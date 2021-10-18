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

static inline void leds_init(void);

void board_init(void)
{
    /* initialize the boards LEDs */
    leds_init();
}

/**
 * @brief Initialize the boards on-board LEDs
 *
 * The LED initialization is hard-coded in this function. As the LED (RGB) are
 * soldered onto the board they are fixed to their CPU pins.
 */
static inline void leds_init(void)
{
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);

    /* Shoot rainbows */
    LED_RAINBOW();
}
