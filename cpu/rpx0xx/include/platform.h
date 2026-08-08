/*
 * Copyright (C) 2024 Frank Engelhardt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_rpx0xx
 * @{
 *
 * @file
 * @brief           RP2040 specific platform definitions
 *
 * @author          Frank Engelhardt <frank@f9e.de>
 */


/**
 * @brief   Wait for a specified number of clock cycles
 *
 * @note    Borrowed from https://www.raspberrypi.com/documentation/pico-sdk/runtime.html#group_pico_platform_1ga49c4fc3b5d3fbe9f2f6f7bc500b9db4f
 */
static inline void busy_wait_at_least_cycles(uint32_t minimum_cycles)
{
    __asm volatile (
        ".syntax unified\n" 
        "1: subs %0, #3\n"
        "bcs 1b\n"
        : "+l" (minimum_cycles) : : "cc", "memory"
    );
}
