/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_mbed_lpc1768
 * @{
 *
 * @file
 * @brief       Board specific implementations for the mbed LPC1768 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "board.h"

#include "periph/gpio.h"

extern void SystemInit(void);

/**
 * @brief Initialize the on-board LEDs.
 */
static void leds_init(void)
{
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_init(LED3_PIN, GPIO_OUT);

    LED0_OFF;
    LED1_OFF;
    LED2_OFF;
    LED3_OFF;
}

void board_init(void)
{
    /* initialize core clocks via CMSIS function */
    SystemInit();
    /* initialize the boards LEDs */
    leds_init();
}
