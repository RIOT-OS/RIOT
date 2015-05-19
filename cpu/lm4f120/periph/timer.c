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
 * @brief       Implementation of the low-level timer driver for the LM4F120
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph_conf.h"
#include "periph/timer.h"

/* guard file in case no timers are defined */
#if TIMER_0_EN

/**
 * @brief Struct holding the configuration data 
 */
typedef struct {
    void (*cb)(int);            /**< timeout callback */
} timer_conf_t;

static timer_conf_t config[TIMER_NUMOF];

int timer_init(tim_t dev, unsigned int us_per_tick, void (*callback)(int))
{
    if (dev == TIMER_0) {
        /* save callback */
        config[TIMER_0].cb = callback;
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); //Activate Timer0

		//Disable TIMER0A during setup
		ROM_TimerDisable(TIMER0_BASE, TIMER_A);
		// Configure for 16-bit mode
		// Configure for Periodic mode
		ROM_TimerConfigure(TIMER0_BASE, 
				TIMER_CFG_SPLIT_PAIR| TIMER_CFG_A_PERIODIC);
		ROM_TimerPrescaleSet(TIMER0_BASE,
				TIMER_A, TIMER_0_PRESCALER);
		// Configure and enable timer interrupt.
		ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);	// TimerA timeout interrupt
		// Set the priority of an interrupt.
		ROM_IntPrioritySet(INT_TIMER0A,TIMER_IRQ_PRIO);
		// Enable interrupt
		ROM_IntEnable(INT_TIMER0A);
		// Enable Timer0A
		ROM_TimerEnable(TIMER0_BASE, TIMER_A);

        return 0;
    }
    return -1;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    if (dev == TIMER_0) {
        unsigned int now = timer_read(dev);
        timer_set_absolute(dev, channel, now + timeout);
        return 1;
    }
    return -1;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    if (dev == TIMER_0) {
		ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, (unsigned long)value);
        return 1;
    }
    return -1;
}

int timer_clear(tim_t dev, int channel)
{
    if (dev == TIMER_0){
		ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, 0);
        return 1;
    }
    return -1;
}

unsigned int timer_read(tim_t dev)
{
    if (dev == TIMER_0) {
		unsigned int currTimer0Val = (unsigned int)ROM_TimerValueGet(TIMER0_BASE, TIMER_A);
        return (currTimer0Val);
    }
    return 0;
}

void timer_start(tim_t dev)
{
    if (dev == TIMER_0) {
		ROM_TimerEnable(TIMER0_BASE, TIMER_A);
    }
}

void timer_stop(tim_t dev)
{
    if (dev == TIMER_0) {
		ROM_TimerDisable(TIMER0_BASE, TIMER_A);
    }
}

void timer_irq_enable(tim_t dev)
{
    if (dev == TIMER_0) {
		ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    }
}

void timer_irq_disable(tim_t dev)
{
    if (dev == TIMER_0) {
		ROM_TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    }
}

void timer_reset(tim_t dev)
{
    if (dev == TIMER_0) {
		// Performs a software reset of a peripheral
		ROM_SysCtlPeripheralReset(SYSCTL_PERIPH_TIMER0);
    }
}

#if TIMER_0_EN
void TIMER0IntHandler(void)
{
	// Clear the timer Interrupt
	ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	// Execute the User Task;
    config[TIMER_0].cb(0);

	if (sched_context_switch_request){
		thread_yield();
	}
}
#endif

#endif /* TIMER_0_EN */
