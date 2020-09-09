/*
 * Copyright (C) 2017, 2019 Ken Rabold, JP Bonn
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the CPU panic for SiFive FE310
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
