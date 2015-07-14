/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Cortex-M specific configuration and initialization options
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"

/**
 * @name   Pattern to write into the co-processor Access Control Register to
 *         allow full FPU access
 */
#define FULL_FPU_ACCESS         (0x00f00000)

void cortexm_init(void)
{
    /* initialize the FPU on Cortex-M4F CPUs */
#ifdef CPU_ARCH_CORTEX_M4F
    /* give full access to the FPU */
    SCB->CPACR |= (uint32_t)FULL_FPU_ACCESS;
#endif

    /* configure the vector table location to internal flash */
#if defined(CPU_ARCH_CORTEX_M3) || defined(CPU_ARCH_CORTEX_M4) || \
    defined(CPU_ARCH_CORTEX_M4F)
    SCB->VTOR = CPU_FLASH_BASE;
#endif

    /* initialize the interrupt priorities */
    /* set pendSV interrupt to lowest possible priority */
    NVIC_SetPriority(PendSV_IRQn, 0xff);
    /* set SVC interrupt to same priority as the rest */
    NVIC_SetPriority(SVCall_IRQn, CPU_DEFAULT_IRQ_PRIO);
    /* initialize all vendor specific interrupts with the same value */
    for (IRQn_Type i = 0; i < CPU_IRQ_NUMOF; i++) {
        NVIC_SetPriority(i, CPU_DEFAULT_IRQ_PRIO);
    }
}
