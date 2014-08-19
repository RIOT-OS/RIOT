/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_stm32f3discovery
 * @{
 *
 * @file
 * @brief       Board specific implementations for the STM32F3Discovery evaluation board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

#include "em_device.h"
#include "em_gpio.h"

static void leds_init(void);

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs */
    leds_init();
}

/**
 * @brief Initialize the boards on-board LEDs (LD3 to LD10)
 *
 * The LED initialization is hard-coded in this function. As the LEDs are soldered
 * onto the board they are fixed to their CPU pins.
 *
 */
static void leds_init(void)
{
    GPIO_PinModeSet(LED_0_PORT, LED_0_PIN, gpioModePushPull, 1);
    GPIO_PinModeSet(LED_1_PORT, LED_1_PIN, gpioModePushPull, 1);
}
