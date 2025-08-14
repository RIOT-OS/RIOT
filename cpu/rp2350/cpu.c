/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup         cpu_rp2350
 * @{
 *
 * @file
 * @brief           CPU initialization implementation for the RP2350
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include "cpu.h"

#include "RP2350.h"
#include "board.h"
#include "clock_conf.h"
#include "cpu_conf_common.h"
#include "helpers.h"
#include "kernel_init.h"
#include "macros/units.h"
#include "periph/gpio.h"
#include "periph/uart.h"
#include "periph/init.h"
#include "periph_cpu.h"
#include "stdio_base.h"

#include <stdint.h>
#include <stdio.h>

#define DEBUG_WITH_OSC

void gpio_reset(void) {
    reset_component(RESET_PADS_BANK0, RESET_PADS_BANK0);
    reset_component(RESET_IO_BANK0, RESET_IO_BANK0);
}

void* core1_main(void* arg) {
    (void) arg;

    while (1) {
        for (volatile int i = 0; i < 500; i++) {
            __NOP();
        };
        gpio_toggle(OSC_DEBUG_PIN_ID);
    }
}

void cpu_init(void) {
    /* initialize the Cortex-M core, once UART support is moved
     * to shared driver as currently this will cause unhandled interrupts */
    cortexm_init();

    /* Reset GPIO state */
    gpio_reset();

    /* Reset clock to default state */
    clock_reset();

    /* initialize the CPU clock */
    cpu_clock_init();

    /* initialize the early peripherals */
    early_init();

    /* trigger static peripheral initialization */
    periph_init();

    /* initialize the board */
    board_init();

    /* Init Core 1 */
    core1_init(&core1_main, NULL);
    core1_init(&core1_main, NULL);

    while (1) {
        for (volatile int i = 0; i < 500; i++) {
            __NOP();
        };
        gpio_toggle(OSC_DEBUG_PIN_ID_2);
    }
}

/** @} */
