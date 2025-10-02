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

#include <sys/unistd.h>

#include "cpu.h"
#include "board.h"
#include "clock_conf.h"
#include "kernel_init.h"
#include "periph_conf.h"

#include "periph/init.h"
#include "periph/uart.h"

/**
 * @brief Initialize the CPU, set IRQ priorities, clocks, peripheral
 */
void cpu_init(void)
{
    /* initialize the RISC-V core */
    riscv_init();

    rp2350_init();
}
