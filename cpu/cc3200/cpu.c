/*
 * Copyright (C) 2015
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc3200
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Attilio Dona'
 * @}
 */

#include <assert.h>

#include "cpu.h"


#if 0
#define BIT(n)          ( 1UL << (n) )

/* Select CLOCK_CTRL register bit masks: */
#define OSC32K          BIT(24) /**< 32-kHz clock oscillator selection */
#define OSC_PD          BIT(17) /**< Oscillator power-down */
#define OSC             BIT(16) /**< System clock oscillator selection */

/* CLOCK_CTRL register field offsets: */
#define IO_DIV_SHIFT    8
#define SYS_DIV_SHIFT   0

#define CLOCK_STA_MASK ( OSC32K | OSC )
#endif

static void cpu_clock_init(void);

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{

	//
	// Enable Processor
	//
	MAP_IntMasterEnable();
	MAP_IntEnable(FAULT_SYSTICK);

	PRCMCC3200MCUInit();


	/* initialize the interrupt priorities */
	/* set pendSV interrupt to same priority as the rest */
	IntPrioritySet(FAULT_PENDSV, CPU_DEFAULT_IRQ_PRIO);
	/* set SVC interrupt to same priority as the rest */
	IntPrioritySet(FAULT_SVCALL, CPU_DEFAULT_IRQ_PRIO);
	/* initialize all vendor specific interrupts with the same value */
	for (unsigned long i = INT_GPIOA0; i < NUM_INTERRUPTS; i++) {
		IntPrioritySet(i, CPU_DEFAULT_IRQ_PRIO);
	}

	cpu_clock_init();
}

/**
 * @brief Configure the controllers clock system
 */
static void cpu_clock_init(void)
{
}
