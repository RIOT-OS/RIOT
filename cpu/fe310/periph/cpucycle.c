/*
 * SPDX-FileCopyrightText: 2017 JP Bonn
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file        cpu_cycle.c
 * @brief       Read CPU cycle counter
 *
 * @author		JP Bonn
 * @}
 */
#include "cpucycle.h"

uint64_t get_cycle_count(void)
{
    uint32_t lo, hi, hi2;

    __asm__ __volatile__ ("1:\n\t"             \
                          "csrr %0, mcycleh\n\t"       \
                          "csrr %1, mcycle\n\t"        \
                          "csrr %2, mcycleh\n\t"        \
                          "bne  %0, %2, 1b\n\t"         \
                          : "=r" (hi), "=r" (lo), "=r" (hi2));
    return lo | ((uint64_t)hi << 32);
}
