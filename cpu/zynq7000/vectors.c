/*
 * Copyright (C) 2015 Xsilon Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_zynq7000
 * @{
 *
 * @file
 * @brief       Cortex A9 interrupt vectors
 *
 * @author      Simon Vincent <simon.vincent@xsilon.com>
 *
 * @}
 */

#include "sched.h"
#include "thread.h"
#include "zynq_interrupts.h"
#include "stdio.h"

void IRQInterrupt(void) {
    _in_isr = 1;
	handle_interrupt();

	if (sched_context_switch_request) {
		thread_yield();
	}
	_in_isr = 0;
}

void FIQInterrupt(void) {
	/* TODO */
	printf("TODO: FIQ not implemented\n");
}

void SWInterrupt(void) {
	/* TODO */
	printf("TODO: SW interrupt not implemented\n");
}

void DataAbortInterrupt(void) {
	/* TODO */
	printf("Data Abort! Stopping...\n");
	while (1) {
	}
}

void PrefetchAbortInterrupt(void) {
	/* TODO */
	printf("Prefetch Abort! Stopping...\n");
	while (1) {
	}
}

