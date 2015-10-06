/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc3200
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Attilio Dona' <@attiliodona>
 * @}
 */

#include <assert.h>
#include "cpu.h"

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void) {
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();

    /* 1 priority group */
    MAP_IntPriorityGroupingSet(0);

    /* initialize the interrupt priorities */
    /* set pendSV interrupt to lower priority as the rest */
    MAP_IntPrioritySet(FAULT_PENDSV, INT_PRIORITY_LVL_7);

    /* set SVC interrupt to same priority as the rest */
    MAP_IntPrioritySet(FAULT_SVCALL, CPU_DEFAULT_IRQ_PRIO);

    /* initialize all vendor specific interrupts with the same value */
    for (unsigned long i = INT_GPIOA0; i < NUM_INTERRUPTS; i++) {
        MAP_IntPrioritySet(i, CPU_DEFAULT_IRQ_PRIO);
    }

}
