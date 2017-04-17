/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @author Dang Minh Phuong <kamejoko80@yahoo.com>
 */

#include <stdint.h>
#include <stdio.h>
#include "arch/irq_arch.h"
#include "arch/thread_arch.h"
#include "sched.h"
#include "thread.h"

extern volatile uint32_t nesting_level;

/**
 * @brief Disable all maskable interrupts
 */
unsigned int irq_arch_disable(void)
{
    unsigned int cpsr;

    __asm__ __volatile__
    (
        "mrs    %0, cpsr\n"
        "cpsid  i\n"
        "cpsid  f\n"
        : "=r" (cpsr)
        :
        : "memory"
    );

    return cpsr;
}

/**
 * @brief Enable all maskable interrupts
 */
unsigned int irq_arch_enable(void)
{
    unsigned int cpsr;

    __asm__ __volatile__
    (
        "mrs    %0, cpsr\n"
        "cpsie  if\n"
        : "=r" (cpsr)
        :
        : "memory"
    );

    return cpsr;
}

/**
 * @brief Restore the state of the IRQ flags
 */
void irq_arch_restore(unsigned int cpsr)
{
    __asm__ __volatile__
    (
        "msr    cpsr_c, %0"
        :
        : "r" (cpsr)
        : "memory"
    );
}

/**
 * @brief See if the current context is inside an ISR
 */
int irq_arch_in(void)
{
    return nesting_level;
}
