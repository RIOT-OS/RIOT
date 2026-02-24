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

 #include "board.h"
 #include "cpu.h"
 #include "clock_conf.h"
 #include "kernel_init.h"
 #include "periph/init.h"
 #include "periph/uart.h"
 #include "periph_conf.h"
 #include "compat_layer.h"

/**
 * @brief Initialize the CPU, set IRQ priorities, clocks, peripheral
 */
void cpu_init(void)
{
    /* rp2350 common startup routine */
    rp2350_init();
    /* initialize the RISC-V core (Interrupts, FPU, etc) */
    rp_arch_init();
}
