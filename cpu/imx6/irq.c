/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_imx6
 * @{
 *
 * @file        irq.c
 * @brief       Implementation of the kernels irq interface
 *
 * TODO needs to be revised
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Leon George <l.george@fu-berlin.de>
 *
 * @}
 */

#include "irq.h"
#include "cortex_a9.h"

unsigned enableIRQ(void) {
	return arm_set_interrupt_state(true);
}

unsigned disableIRQ(void) {
	return arm_set_interrupt_state(false);
}

void restoreIRQ(unsigned state) {
	arm_set_interrupt_state(state);
}

/*
 * TODO this is taken from arm_common
 * maybe imx6 can also use that
 */
#define IRQ32Mode   0x12
#define FIQ32Mode   0x11
#define INTMode         (FIQ32Mode | IRQ32Mode)

int inISR(void) {
	/* TODO also taken from arm_common/VIC.c */
	int retval;
	asm volatile(" mrs  %0, cpsr" : "=r"(retval) : /* no inputs */);
	return (retval & INTMode) == 18;
}
