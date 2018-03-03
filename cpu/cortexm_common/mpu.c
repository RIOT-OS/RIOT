/*
 * Copyright (C) 2016 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file        mpu.c
 * @brief       Cortex-M Memory Protection Unit (MPU) Driver
 *
 * @author      Ian Martin <ian@locicontrols.com>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "mpu.h"

int mpu_disable(void) {
#if __MPU_PRESENT
    MPU->CTRL &= ~MPU_CTRL_ENABLE_Msk;
    return 0;
#else
    return -1;
#endif
}

int mpu_enable(void) {
#if __MPU_PRESENT
    MPU->CTRL |= MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_ENABLE_Msk;

#ifdef SCB_SHCSR_MEMFAULTENA_Msk
    /* Enable the memory fault exception if SCB SHCSR (System Handler Control
     * and State Register) has a separate bit for mem faults. That is the case
     * on ARMv7-M. ARMv6-M does not support separate exception enable for mem
     * faults and all fault conditions cause a HardFault. */
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
#endif

    return 0;
#else
    return -1;
#endif
}

bool mpu_enabled(void) {
#if __MPU_PRESENT
    return (MPU->CTRL & MPU_CTRL_ENABLE_Msk) != 0;
#else
    return false;
#endif
}

int mpu_configure(uint_fast8_t region, uintptr_t base, uint_fast32_t attr) {
#if __MPU_PRESENT
    assert(region < MPU_NUM_REGIONS);

    MPU->RNR  = region;
    MPU->RBAR = base & MPU_RBAR_ADDR_Msk;
    MPU->RASR = attr | MPU_RASR_ENABLE_Msk;

    if (!mpu_enabled()) {
        mpu_enable();
    }

    return 0;
#else
    (void)region;
    (void)base;
    (void)attr;
    return -1;
#endif
}
