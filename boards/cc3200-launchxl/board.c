/*
 * Copyright (C) 2019 Ludwig Maximilian Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     boards_cc3200-launchxl
 * @{
 *
 * @file
 * @brief       Board specific implementations for the CC3200 launchpad board
 *
 * @author      Wladislaw Meixner <wladislaw.meixner@campus.lmu.de>
 */

#include "board.h"
#include "cpu.h"

#include "periph/gpio.h"
#include "vendor/hw_gpio.h"
#include "vendor/rom.h"

/**
 * @brief Initialize on-board LEDs
 */
void led_init(void)
{
    /* enable GPIO block b timer */
    ARCM->GPIO_B.clk_gating |= PRCM_RUN_MODE_CLK;

    /* enable leads and clear them */
    gpio_init(LED_RED, GPIO_OUT);
    gpio_clear(LED_RED);

    gpio_init(LED_ORANGE, GPIO_OUT);
    gpio_clear(LED_ORANGE);

    gpio_init(LED_GREEN, GPIO_OUT);
    gpio_clear(LED_GREEN);
}

/**
 * @brief Initialize the board
 */
void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs */
    led_init();
}

/** @} */
