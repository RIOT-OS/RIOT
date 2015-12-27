/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <stdint.h>
#include "cpu.h"
#include "board.h"
#include "periph/init.h"

/**
 * @ingroup     cpu_k60
 * @{
 *
 * @file
 * @brief       Implementation of K60 CPU initialization.
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

/**
 * @brief Check the running CPU identification to find if we are running on the
 *        wrong hardware.
 */
static void check_running_cpu_revision(void);

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
    /* Check that we are running on the CPU that this code was built for */
    check_running_cpu_revision();
    /* trigger static peripheral initialization */
    periph_init();
}

static void check_running_cpu_revision(void)
{
    /* Check that the running CPU revision matches the compiled revision */
    if (SCB->CPUID != K60_EXPECTED_CPUID) {
        uint32_t CPUID = SCB->CPUID; /* This is only to ease debugging, type
                   * "print /x CPUID" in gdb */
        uint32_t SILICON_REVISION = (SCB->CPUID & SCB_CPUID_REVISION_Msk) + 1;
        (void)CPUID; /* prevents compiler warnings about an unused variable. */
        (void)SILICON_REVISION;

        /* Running on the wrong CPU, the clock initialization is different
         * between silicon revision 1.x and 2.x (LSB of CPUID) */
        /* If you unexpectedly end up on this line when debugging:
         * Rebuild the code using the correct value for K60_CPU_REV */
        __asm__ volatile ("bkpt #99\n");

        while (1);
    }
}

/** @} */
