/*
 * Copyright (C) 2020 Sören Tempel <tempel@uni-bremen.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief Test application for the mpu_noexec_stack pseudo-module
 *
 * @author Sören Tempel <tempel@uni-bremen.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "cpu.h"
#include "mpu.h"

/* RIOT's MPU headers gracefully fail when no MPU is present.
 * Use this to catch if RIOT's features are correctly gating MPU use.
 */
#if !__MPU_PRESENT
#error "(!__MPU_PRESENT)"
#endif

#define JMPBUF_SIZE 3

int main(void)
{
    uint32_t buf[JMPBUF_SIZE];

    /* Fill the buffer with invalid instructions */
    for (unsigned i = 0; i < JMPBUF_SIZE; i++) {
        buf[i] = UINT32_MAX;
    }

    puts("Attempting to jump to stack buffer ...\n");
    __asm__ volatile ("BX %0"
                      : /* no output operands */
                      : "r" ((uint8_t*)&buf + 1)); /* LSB must be set for thumb2 */

    return 0;
}
