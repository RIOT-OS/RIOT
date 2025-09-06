/*
 * SPDX-FileCopyrightText: 2023 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_arm7tdmi_gba
 * @{
 */

#include <stdint.h>
#include "cpu.h"
#include "kernel_init.h"

/**
 * @brief Initialize the CPU, set IRQ priorities, clocks
 */
void cpu_init(void)
{
    extern void board_init(void);

    /* board specific setup of i/o pins */
    board_init();

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();
}
