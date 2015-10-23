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

#include "driverlib/rom_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/utils.h"

#include "board.h"
#include "cpu.h"
#include "lpm.h"

#include "periph/gpio.h"


/**
 * @brief Initialize on-board LEDs
 */
void led_init(void) {
    gpio_init(RED_LED, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_init(GREEN_LED, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_init(YELLOW_LED, GPIO_DIR_OUT, GPIO_NOPULL);

    gpio_clear(RED_LED);
    gpio_clear(GREEN_LED);
    gpio_clear(YELLOW_LED);
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
