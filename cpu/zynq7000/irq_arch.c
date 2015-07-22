/*
 * Copyright (C) 2015 Xsilon Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_zynq7000
 * @{
 *
 * @file
 * @brief       Implementation of the kernels irq interface
 *
 * @author      Simon Vincent <simon.vincent@xsilon.com>
 *
 * @}
 */

#include <stdint.h>
#include "arch/irq_arch.h"
#include "cpu.h"

volatile int _in_isr = 0;

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
    return _in_isr;
}
