/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the CPU initialization
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 * @}
 */

#include "cpu.h"
#include "irq.h"
#include "kernel.h"
#include "kernel_internal.h"
#include "sched.h"
#include "thread.h"
#include "arch/thread_arch.h"
#include "arch/irq_arch.h"

extern void (* const interrupt_vector[])(void);

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initializes vector table */
    MAP_IntVTableBaseSet((unsigned long)&interrupt_vector[0]);

    /* enable processor */
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);
    PRCMCC3200MCUInit();
}
