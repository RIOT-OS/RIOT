/*
 * SPDX-FileCopyrightText: 2017, 2019 Ken Rabold, JP Bonn
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_riscv_common
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the CPU panic for RISC-V
 *
 * @author      Ken Rabold
 * @}
 */

#include "panic.h"

void panic_arch(void)
{
#ifdef DEVELHELP
    while (1) {}
#endif
}
