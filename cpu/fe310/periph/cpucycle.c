/*
 * Copyright 2017  JP Bonn
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
#include "vendor/encoding.h"


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
