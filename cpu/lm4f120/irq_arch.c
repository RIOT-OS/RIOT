/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com
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
    unsigned long mask = ROM_IntPriorityMaskGet();
    ROM_IntMasterDisable();
    return ((unsigned int)mask);
}

/**
 * @brief Enable all maskable interrupts
 */
unsigned int irq_arch_enable(void)
{
	ROM_IntMasterEnable();
	unsigned long mask = ROM_IntPriorityMaskGet(); 
    return ((unsigned int)mask);
}

/**
 * @brief Restore the state of the IRQ flags
 */
void irq_arch_restore(unsigned int state)
{
    ROM_IntPriorityMaskSet((unsigned long) state);
}

/**
 * @brief See if the current context is inside an ISR
 */
int irq_arch_in(void)
{
    return ((int)CPUipsrGet() & 0xFF);
}
