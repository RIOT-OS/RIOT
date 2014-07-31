/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup msba2
 * @{
 */

/**
 * @file
 * @brief       msba2 benchmark functions
 *
 * @author      Heiko Will <heiko.will@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 */

#include <stdint.h>
#include <stdio.h>
#include "lpc2387.h"

void  benchmark_init(void)
{
	PCLKSEL1 = (PCLKSEL1 & ~(BIT14|BIT15)) | (1 << 14);	// CCLK to PCLK divider
	PCONP |= PCTIM3;
	T3TCR = 0;											// disable timer
	T3MCR = 0;											// disable interrupt
	T3CCR = 0;											// capture is disabled.
	T3EMR = 0;											// no external match output.
	T3PR = 0;											// set prescaler
	T3TC = 0;											// reset counter
}

void  benchmark_reset_start(void)
{
	T3TCR = 0;												// disable timer
	T3TC = 0;												// reset counter
	T3TCR = BIT0;
}

unsigned int  benchmark_read_stop(void)
{
	T3TCR = 0;												// disable timer
	return T3TC;
}
