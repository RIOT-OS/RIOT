/*
 * Copyright (C) 2023 HAW Hamburg. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
