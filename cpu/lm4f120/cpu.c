/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file
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

unsigned long __attribute__((naked))
CPUipsrGet(void)
{
    unsigned long ulRet;

    //
    // Read IPSR
    //
    __asm("    mrs     r0, IPSR\n"
          "    bx      lr\n"
          : "=r" (ulRet));

    //
    // The return is handled in the inline assembly, but the compiler will
    // still complain if there is not an explicit return here (despite the fact
    // that this does not result in any code being produced because of the
    // naked attribute).
    //
    return(ulRet);
}

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* set pendSV interrupt to lowest possible priority */
	IntPrioritySet(FAULT_PENDSV, 0x80);
}
