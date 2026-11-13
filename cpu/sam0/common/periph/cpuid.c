/*
 * SPDX-FileCopyrightText: 2014-2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_cpuid
 * @{
 *
 * @file
 * @brief       Low-level CPUID driver implementation
 *
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "periph/cpuid.h"

#ifdef CPU_COMMON_SAMD5X
#define WORD0               (*(volatile uint32_t *)0x008061FC)
#define WORD1               (*(volatile uint32_t *)0x00806010)
#define WORD2               (*(volatile uint32_t *)0x00806014)
#define WORD3               (*(volatile uint32_t *)0x00806018)
#else
#define WORD0               (*(volatile uint32_t *)0x0080A00C)
#define WORD1               (*(volatile uint32_t *)0x0080A040)
#define WORD2               (*(volatile uint32_t *)0x0080A044)
#define WORD3               (*(volatile uint32_t *)0x0080A048)
#endif

void cpuid_get(void *id)
{
    /* Use memcpy to prevent unaligned access, which is not supported on
     * cortex-m0+, to id */
    uint32_t addr[] = { WORD0, WORD1, WORD2, WORD3 };
    memcpy(id, &addr[0], CPUID_LEN);
}
