/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_rp2350_riscv
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the CPU initialization for RP2350
 *
 * @author      Tom Hert <git@annsann.eu>
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "clock_conf.h"
#include "kernel_init.h"
#include "periph/init.h"
#include "periph/uart.h"
#include "periph_conf.h"

#include <sys/unistd.h>

void gpio_reset(void)
{
    reset_component(RESET_PADS_BANK0, RESET_PADS_BANK0);
    reset_component(RESET_IO_BANK0, RESET_IO_BANK0);
}

/**
 * @brief Initialize the CPU, set IRQ priorities, clocks, peripheral
 */
void cpu_init(void)
{
    /* Reset GPIO state */
    gpio_reset();

    /* Reset clock to default state */
    clock_reset();

    /* initialize the CPU clock */
    cpu_clock_init();

    /* initialize the RISC-V core */
    riscv_init();

    /* initialize the early peripherals */
    early_init();

    /* trigger static peripheral initialization */
    periph_init();

    /* initialize the board */
    board_init();
}
