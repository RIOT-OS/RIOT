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


#include "cpu.h"
#include "irq.h"

unsigned disableIRQ(void)
{
	return(irq_arch_disable());
}

unsigned enableIRQ(void)
{
	return(irq_arch_enable());
}

void restoreIRQ(unsigned state)
{
	irq_arch_restore(state);
}

int inISR(void)
{
	return(irq_arch_in());
}

