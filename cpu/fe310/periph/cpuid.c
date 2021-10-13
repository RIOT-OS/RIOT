/*
 * Copyright 2017 Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file        cpuid.c
 * @brief       Low-level CPUID driver implementation
 *
 * @author      Ken Rabold
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "periph/cpuid.h"
#include "cpu.h"
#include "vendor/riscv_csr.h"
#include "vendor/platform.h"

void cpuid_get(void *id)
{
    uint32_t result[5];

    /* Read RISC-V CSRs for CPU ID */
    result[0] = read_csr(mvendorid);
    result[1] = read_csr(marchid);
    result[2] = read_csr(mimpid);

    memcpy(id, &result[0], CPUID_LEN);
}
