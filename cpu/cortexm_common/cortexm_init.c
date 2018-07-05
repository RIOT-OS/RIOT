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

/**
 * Interrupt vector base address, defined by the linker
 */
extern const void *_isr_vectors;

/* 0x41 is ARM */
#define EXPECTED_CPUID_IMPLEMENTER 0x41
#if (__CORTEX_M == 0)
#define EXPECTED_CPUID_PARTNO 0xc20
#elif (__CORTEX_M == 3)
#define EXPECTED_CPUID_PARTNO 0xc23
#elif (__CORTEX_M == 4)
#define EXPECTED_CPUID_PARTNO 0xc24
#elif (__CORTEX_M == 7)
#define EXPECTED_CPUID_PARTNO 0xc27
#else
#error Unsupported Cortex-M core!
#endif

int cortexm_check_core(void)
{
    uint32_t cpuid = SCB->CPUID;
    if ((cpuid & (SCB_CPUID_IMPLEMENTER_Msk | SCB_CPUID_PARTNO_Msk)) !=
        ((EXPECTED_CPUID_IMPLEMENTER << SCB_CPUID_IMPLEMENTER_Pos) |
         (EXPECTED_CPUID_PARTNO << SCB_CPUID_PARTNO_Pos))) {
        return 1;
    }
    return 0;
}

void cortexm_init(void)
{
    /* Check that the core we are running on is the same kind as the one this
     * binary was compiled for */
    if (cortexm_check_core() != 0)
    {
        /* This binary was built for a different CPU, continuing on will not end
         * well, so we reset */
#if DEVELHELP
        __asm__ volatile ("BKPT #34\n");
        while(1) {}
#else
        NVIC_SystemReset();
#endif
    }
    /* initialize the FPU on Cortex-M4F CPUs */
#if defined(CPU_ARCH_CORTEX_M4F) || defined(CPU_ARCH_CORTEX_M7)
    /* give full access to the FPU */
    SCB->CPACR |= (uint32_t)FULL_FPU_ACCESS;
#endif

    /* configure the vector table location to internal flash */
#if defined(CPU_ARCH_CORTEX_M3) || defined(CPU_ARCH_CORTEX_M4) || \
    defined(CPU_ARCH_CORTEX_M4F) || defined(CPU_ARCH_CORTEX_M7) || \
    (defined(CPU_ARCH_CORTEX_M0PLUS) && (__VTOR_PRESENT == 1))
    SCB->VTOR = (uint32_t)&_isr_vectors;
#endif

    /* initialize the interrupt priorities */
    /* set pendSV interrupt to same priority as the rest */
    NVIC_SetPriority(PendSV_IRQn, CPU_DEFAULT_IRQ_PRIO);
    /* set SVC interrupt to same priority as the rest */
    NVIC_SetPriority(SVCall_IRQn, CPU_DEFAULT_IRQ_PRIO);
    /* initialize all vendor specific interrupts with the same value */
    for (unsigned i = 0; i < CPU_IRQ_NUMOF; i++) {
        NVIC_SetPriority((IRQn_Type) i, CPU_DEFAULT_IRQ_PRIO);
    }

    /* enable wake up on events for __WFE CPU sleep */
    SCB->SCR |= SCB_SCR_SEVONPEND_Msk;

    /* for Cortex-M3 r1p0 and up the STKALIGN option was added, but not automatically
     * enabled until revision r2p0. For 64bit function arguments to work properly this
     * needs to be enabled.
     */
#ifdef SCB_CCR_STKALIGN_Msk
    SCB->CCR |= SCB_CCR_STKALIGN_Msk;
#endif
}
