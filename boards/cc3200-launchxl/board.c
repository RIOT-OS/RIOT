/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_cc3200
 * @{
 *
 * @file
 * @brief       Board specific implementations for the CC3200 launchpad board
 *
 * @author      Attilio Dona'
 */
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <stdio.h>

#include "driverlib/interrupt.h"
#include "driverlib/utils.h"

#include "board.h"
#include "lpm.h"

#include "periph/gpio.h"


/**
 * @brief Initialize on-board LEDs
 */
void led_init(void) {

    gpio_init(LED_RED, GPIO_OUT);
    gpio_init(LED_GREEN, GPIO_OUT);
    gpio_init(LED_YELLOW, GPIO_OUT);

    gpio_clear(LED_RED);
    gpio_clear(LED_GREEN);
    gpio_clear(LED_YELLOW);
}

extern const void *interrupt_vector[];


/**
 * @brief Initialize the board
 */
void board_init(void) {

    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA2, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA3, PRCM_RUN_MODE_CLK);

    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs */
    led_init();
}

/** @} */
