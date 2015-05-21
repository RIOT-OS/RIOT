/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file
 * @brief       Implementation of the kernels irq interface
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 *
 * @}
 */

#include <stdint.h>
#include "arch/irq_arch.h"
#include "cpu.h"

/**
 * @brief Disable all maskable interrupts
 */
unsigned int irq_arch_disable(void)
{
    unsigned long mask = IntPriorityMaskGet();
    IntMasterDisable();
    return ((unsigned int)mask);
}

/**
 * @brief Enable all maskable interrupts
 */
unsigned int irq_arch_enable(void)
{
	IntMasterEnable();
	unsigned long mask = IntPriorityMaskGet(); 
    return ((unsigned int)mask);
}

/**
 * @brief Restore the state of the IRQ flags
 */
void irq_arch_restore(unsigned int state)
{
    IntPriorityMaskSet((unsigned long) state);
}

/**
 * @brief See if the current context is inside an ISR
 */
int irq_arch_in(void)
{
    return ((int)CPUipsrGet() & 0xFF);
}
