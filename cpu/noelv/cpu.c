/*
 * SPDX-FileCopyrightText: 2026 Matvii Ivashchenko
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_noelv
 * @{
 *
 * @file
 * @brief       NOEL-V CPU initialization
 *
 * @}
 */

#include "kernel_init.h"
#include "periph/init.h"
#include "cpu.h"

void cpu_init(void)
{
    riscv_init();
    early_init();
    periph_init();
}
