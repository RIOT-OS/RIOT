/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cc3200
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * @author      Attilio Dona' <@attiliodona>
 *
 * @}
 */

#include <stdlib.h>

#include <thread.h>
#include "panic.h"
#include <sys/types.h>

#include "periph_conf.h"
#include "periph/timer.h"

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"

#include "inc/hw_timer.h"

#include "driverlib/timer.h"
#include "driverlib/prcm.h"

#define MAX_TIMERS TIMER_UNDEFINED

#define EMPTY_VALUE -1

#define MAX_TIMER_VALUE 0xFFFFFFFF;

#define CALIBRATION 111

#define TIMER2_SW_CHANNELS
#define TIMER3_SW_CHANNELS

// ticks calibration for time elapsed between TAR read and TIMER_MATCH value written to register
#define ELAPSED_TICKS 20

#define TIM2_CHANNELS TIMER_NUM_CHANNELS
#define TIM3_CHANNELS TIMER_NUM_CHANNELS

/** Type for timer state */
typedef struct {
	void (*cb)(int);
} timer_conf_t;

/** Timer state memory */
timer_conf_t config[MAX_TIMERS];


typedef struct timer_queue_item {
	unsigned long long value;
	int channel;
	struct timer_queue_item *next;
} timer_queue_item;

timer_queue_item timer2_queue[TIM2_CHANNELS];

timer_queue_item *freeq2;
timer_queue_item *busyq2;

timer_queue_item timer3_queue[TIM3_CHANNELS];

timer_queue_item *freeq3;
timer_queue_item *busyq3;

void irq_timer0_handler(void) {
	timer_clear(TIMER_0, 0);
	config[TIMER_0].cb(0); // timer has one hw channel
	if(sched_context_switch_request) {
		thread_yield();
	}
}

void irq_timer1_handler(void) {
	timer_clear(TIMER_1, 0);
	config[TIMER_1].cb(0); // timer has one hw channel
	if(sched_context_switch_request) {
		thread_yield();
	}
}

void irq_timer2_handler(void) {
#ifdef TIMER2_SW_CHANNELS
	unsigned long sts = HWREG(TIMERA2_BASE + TIMER_O_MIS);

	timer_queue_item *curr_ptr = busyq2;

	if (sts & TIMER_MIS_TATOMIS) {
		// clears the flag
		MAP_TimerIntClear(TIMERA2_BASE, TIMER_TIMA_TIMEOUT);

		// timeout, re-evaluate counters
		while (curr_ptr) {
			curr_ptr->value = curr_ptr->value % MAX_TIMER_VALUE;
			curr_ptr = curr_ptr->next;
		}

		return;
	} else if (sts & TIMER_MIS_TAMMIS) {

		timer_queue_item *active = busyq2;

		int ch = active->channel;

		busyq2 = active->next;
		active->next = freeq2;
		freeq2 = active;

		timer_clear(TIMER_2, 0);
		config[TIMER_2].cb(ch);

		sts = HWREG(TIMERA2_BASE + TIMER_O_TAR);

		while (busyq2 && sts > (busyq2->value - ELAPSED_TICKS)) {
			ch = busyq2->channel;
			active = busyq2;
			busyq2 = active->next;
			active->next = freeq2;
			freeq2 = active;

			config[TIMER_2].cb(ch);

			sts = HWREG(TIMERA2_BASE + TIMER_O_TAR);
		}
		if(busyq2) {
			HWREG(TIMERA2_BASE + TIMER_O_TAMATCHR) = busyq2->value;
		}
	}
#else
	timer_clear(TIMER_2, 0);
	config[TIMER_2].cb(0); // timer has one hw channel
#endif
	if(sched_context_switch_request) {
		thread_yield();
	}

}

void irq_timer3_handler(void) {
#ifdef TIMER3_SW_CHANNELS
	unsigned long sts = HWREG(TIMERA3_BASE + TIMER_O_MIS);

	timer_queue_item *curr_ptr = busyq3;

	if (sts & TIMER_MIS_TATOMIS) {
		// clears the flag
		MAP_TimerIntClear(TIMERA3_BASE, TIMER_TIMA_TIMEOUT);

		// timeout, re-evaluate counters
		while (curr_ptr) {
			curr_ptr->value = curr_ptr->value % MAX_TIMER_VALUE;
			curr_ptr = curr_ptr->next;
		}

		return;
	} else if (sts & TIMER_MIS_TAMMIS) {

		timer_queue_item *active = busyq3;

		int ch = active->channel;

		busyq3 = active->next;
		active->next = freeq3;
		freeq3 = active;

		timer_clear(TIMER_3, 0);
		config[TIMER_3].cb(ch);

		sts = HWREG(TIMERA3_BASE + TIMER_O_TAR);

		while (busyq3 && sts > (busyq3->value - ELAPSED_TICKS)) {
			ch = busyq3->channel;
			active = busyq3;
			busyq3 = active->next;
			active->next = freeq3;
			freeq3 = active;

			config[TIMER_3].cb(ch);

			sts = HWREG(TIMERA3_BASE + TIMER_O_TAR);
		}
		if (busyq3) {
			HWREG(TIMERA3_BASE + TIMER_O_TAMATCHR) = busyq3->value;
		}
	}
#else
	timer_clear(TIMER_3, 0);
	config[TIMER_3].cb(0); // timer has one hw channel
#endif

	if(sched_context_switch_request) {
		thread_yield();
	}

}


int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int)) {

#if defined(TIMER3_SW_CHANNELS) || defined(TIMER2_SW_CHANNELS)
	int j;
#endif
	switch (dev) {
	case TIMER_0:
		//
		// Enable and Reset the timer block
		//
		MAP_PRCMPeripheralClkEnable(PRCM_TIMERA0, PRCM_RUN_MODE_CLK);
		MAP_PRCMPeripheralReset(PRCM_TIMERA0);

		MAP_TimerConfigure(TIMERA0_BASE, TIMER_CFG_PERIODIC_UP);

		MAP_TimerControlStall(TIMERA0_BASE, TIMER_A, true);
		// register the handler
		MAP_TimerIntRegister(TIMERA0_BASE, TIMER_A, irq_timer0_handler);

		config[TIMER_0].cb = callback;

		MAP_IntPrioritySet(INT_TIMERA0A, INT_PRIORITY_LVL_2);

		// enable the timer
		MAP_TimerEnable(TIMERA0_BASE, TIMER_A);

		break;
	case TIMER_1:
		//
		// Enable and Reset the timer block
		//
		MAP_PRCMPeripheralClkEnable(PRCM_TIMERA1, PRCM_RUN_MODE_CLK);
		MAP_PRCMPeripheralReset(PRCM_TIMERA1);

		MAP_TimerConfigure(TIMERA1_BASE, TIMER_CFG_PERIODIC_UP);

		MAP_TimerControlStall(TIMERA1_BASE, TIMER_A, true);

		// register the handler
		MAP_TimerIntRegister(TIMERA1_BASE, TIMER_A, irq_timer1_handler);

		config[TIMER_1].cb = callback;

		MAP_IntPrioritySet(INT_TIMERA1A, INT_PRIORITY_LVL_2);

		// enable the timer
		MAP_TimerEnable(TIMERA1_BASE, TIMER_A);

		break;
	case TIMER_2:
#ifdef TIMER2_SW_CHANNELS
		j = TIM2_CHANNELS;

		while (--j) {
			timer2_queue[j - 1].next = &timer2_queue[j];
		}

		freeq2 = timer2_queue;
		busyq2 = NULL;
#endif
		MAP_PRCMPeripheralClkEnable(PRCM_TIMERA2, PRCM_RUN_MODE_CLK);
		MAP_PRCMPeripheralReset(PRCM_TIMERA2);

		MAP_TimerConfigure(TIMERA2_BASE, TIMER_CFG_PERIODIC_UP);

		MAP_TimerControlStall(TIMERA2_BASE, TIMER_A, true);

		// register the handler
		MAP_TimerIntRegister(TIMERA2_BASE, TIMER_A, irq_timer2_handler);

		config[TIMER_2].cb = callback;

		MAP_IntPrioritySet(INT_TIMERA2A, INT_PRIORITY_LVL_2);

#ifdef TIMER2_SW_CHANNELS
		// enable the timeout interrupt
		MAP_TimerIntEnable(TIMERA2_BASE, TIMER_TIMA_TIMEOUT);
#endif

		// enable the timer
		MAP_TimerEnable(TIMERA2_BASE, TIMER_A);

		break;
	case TIMER_3:
#ifdef TIMER3_SW_CHANNELS
		j = TIM3_CHANNELS;

		while (--j) {
			timer3_queue[j - 1].next = &timer3_queue[j];
		}

		freeq3 = timer3_queue;
		busyq3 = NULL;
#endif
		MAP_PRCMPeripheralClkEnable(PRCM_TIMERA3, PRCM_RUN_MODE_CLK);
		MAP_PRCMPeripheralReset(PRCM_TIMERA3);

		MAP_TimerConfigure(TIMERA3_BASE, TIMER_CFG_PERIODIC_UP);

		MAP_TimerControlStall(TIMERA3_BASE, TIMER_A, true);

		// register the handler
		MAP_TimerIntRegister(TIMERA3_BASE, TIMER_A, irq_timer3_handler);

		config[TIMER_3].cb = callback;

		MAP_IntPrioritySet(INT_TIMERA3A, INT_PRIORITY_LVL_2);

#ifdef TIMER3_SW_CHANNELS
		// enable the timeout interrupt
		MAP_TimerIntEnable(TIMERA3_BASE, TIMER_TIMA_TIMEOUT);
#endif
		// enable the timer
		MAP_TimerEnable(TIMERA3_BASE, TIMER_A);

		break;
	default:
		break;
	}

	return 0;
}

int set_absolute(tim_t dev, int channel, unsigned long long value) {
#if defined(TIMER3_SW_CHANNELS) || defined(TIMER2_SW_CHANNELS)
	unsigned long long abstimeout;
#endif
	switch (dev) {
	case TIMER_0:
		MAP_TimerMatchSet(TIMERA0_BASE, TIMER_A, value);
		HWREG(TIMERA0_BASE + TIMER_O_IMR) |= TIMER_TIMA_MATCH; // enable the match timer
		break;
	case TIMER_1:
		MAP_TimerMatchSet(TIMERA1_BASE, TIMER_A, value);
		HWREG(TIMERA1_BASE + TIMER_O_IMR) |= TIMER_TIMA_MATCH; // enable the match timer
		break;
	case TIMER_2:
#ifdef TIMER2_SW_CHANNELS

		// critical section, disable interrupt
		MAP_TimerIntDisable(TIMERA2_BASE, TIMER_TIMA_MATCH);

		if (freeq2 == NULL) {
			// no free timer left
			core_panic(SW_TIMERS_EXAUSTED, "TIM2: timer no space left");
			break;
		}
		abstimeout = value - CALIBRATION;

		if (busyq2) {
			// some timers already activated
			timer_queue_item *ptr = busyq2;
			timer_queue_item *prev = NULL;
			while (ptr && ptr->value < abstimeout) {
				prev = ptr;
				ptr = ptr->next;
			}
			if (prev) {
				// not the shortest timer
				prev->next = freeq2;
				freeq2->channel = channel;
				freeq2->value = abstimeout;
				freeq2 = freeq2->next;

				prev->next->next = ptr;
			} else {
				// insert into the first position
				busyq2 = freeq2;
				freeq2->channel = channel;
				freeq2->value = abstimeout;
				freeq2 = freeq2->next;

				busyq2->next = ptr;

				MAP_TimerMatchSet(TIMERA2_BASE, TIMER_A, abstimeout);
			}

		} else {
			// insert the first one

			MAP_TimerMatchSet(TIMERA2_BASE, TIMER_A, abstimeout);
			HWREG(TIMERA2_BASE + TIMER_O_IMR) |= TIMER_TIMA_MATCH; // enable the match timer

			busyq2 = freeq2;
			freeq2 = freeq2->next;
			busyq2->next = NULL;
			busyq2->channel = channel;
			busyq2->value = abstimeout;

		}
		// end of critical section, enable interrupt
		MAP_TimerIntEnable(TIMERA2_BASE, TIMER_TIMA_MATCH);


#else
		MAP_TimerMatchSet(TIMERA2_BASE, TIMER_A, value);
		HWREG(TIMERA2_BASE + TIMER_O_IMR) |= TIMER_TIMA_MATCH; // enable the match timer
#endif
		break;
	case TIMER_3:
#ifdef TIMER3_SW_CHANNELS
		// critical section, disable interrupt
		MAP_TimerIntDisable(TIMERA3_BASE, TIMER_TIMA_MATCH);

		if (freeq3 == NULL) {
			// no free timer left
			core_panic(SW_TIMERS_EXAUSTED, "TIM3: timer no space left");
			break;
		}
		abstimeout = value - CALIBRATION;

		if (busyq3) {
			// some timers already activated
			timer_queue_item *ptr = busyq3;
			timer_queue_item *prev = NULL;
			while (ptr && ptr->value < abstimeout) {
				prev = ptr;
				ptr = ptr->next;
			}
			if (prev) {
				// not the shortest timer
				prev->next = freeq3;
				freeq3->channel = channel;
				freeq3->value = abstimeout;
				freeq3 = freeq3->next;

				prev->next->next = ptr;
			} else {
				// insert into the first position
				busyq3 = freeq3;
				freeq3->channel = channel;
				freeq3->value = abstimeout;
				freeq3 = freeq3->next;

				busyq3->next = ptr;

				MAP_TimerMatchSet(TIMERA3_BASE, TIMER_A, abstimeout);
			}

		} else {
			// insert the first one

			MAP_TimerMatchSet(TIMERA3_BASE, TIMER_A, abstimeout);
			HWREG(TIMERA3_BASE + TIMER_O_IMR) |= TIMER_TIMA_MATCH; // enable the match timer

			busyq3 = freeq3;
			freeq3 = freeq3->next;
			busyq3->next = NULL;
			busyq3->channel = channel;
			busyq3->value = abstimeout;
		}
		// end of critical section, enable interrupt
		MAP_TimerIntEnable(TIMERA3_BASE, TIMER_TIMA_MATCH);

#else
		MAP_TimerMatchSet(TIMERA3_BASE, TIMER_A, value);
		HWREG(TIMERA3_BASE + TIMER_O_IMR) |= TIMER_TIMA_MATCH; // enable the match timer
#endif
		break;
	default:
		break;
	}

	return 0;

}

int timer_set(tim_t dev, int channel, unsigned int timeout) {

	switch (dev) {
	case TIMER_0:
		return timer_set_absolute(dev, channel, HWREG(TIMERA0_BASE + TIMER_O_TAR) + timeout);
	case TIMER_1:
		return timer_set_absolute(dev, channel, HWREG(TIMERA1_BASE + TIMER_O_TAR) + timeout);
	case TIMER_2:
		return set_absolute(dev, channel, HWREG(TIMERA2_BASE + TIMER_O_TAR) + (unsigned long long)timeout);
	case TIMER_3:
		return set_absolute(dev, channel, HWREG(TIMERA3_BASE + TIMER_O_TAR) + (unsigned long long)timeout);
	default:
		break;
	}

	return 0;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value) {
	return set_absolute(dev,channel,value);
}


int timer_clear(tim_t dev, int channel) {
	switch (dev) {
	case TIMER_0:
		MAP_TimerIntClear(TIMERA0_BASE, TIMER_TIMA_MATCH);
		HWREG(TIMERA0_BASE + TIMER_O_IMR) &= ~(TIMER_TIMA_MATCH); // disable the match timer
		break;
	case TIMER_1:
		MAP_TimerIntClear(TIMERA1_BASE, TIMER_TIMA_MATCH);
		HWREG(TIMERA1_BASE + TIMER_O_IMR) &= ~(TIMER_TIMA_MATCH); // disable the match timer
		break;
	case TIMER_2:
		MAP_TimerIntClear(TIMERA2_BASE, TIMER_TIMA_MATCH);
#ifdef TIMER2_SW_CHANNELS
		if (busyq2 == NULL) {
			// no timer left into buffer
			MAP_TimerIntDisable(TIMERA2_BASE, TIMER_TIMA_MATCH);
		}
#else
		HWREG(TIMERA2_BASE + TIMER_O_IMR) &= ~(TIMER_TIMA_MATCH); // disable the match timer
#endif
		break;
	case TIMER_3:
		MAP_TimerIntClear(TIMERA3_BASE, TIMER_TIMA_MATCH);
#ifdef TIMER3_SW_CHANNELS
		if (busyq3 == NULL) {
			// no timer left into buffer
			MAP_TimerIntDisable(TIMERA3_BASE, TIMER_TIMA_MATCH);
		}
#else
		HWREG(TIMERA3_BASE + TIMER_O_IMR) &= ~(TIMER_TIMA_MATCH); // disable the match timer
#endif
		break;
	default:
		break;
	}
	return 0;
}

unsigned int timer_read(tim_t dev) {
	switch (dev) {
	case TIMER_0:
		return TimerValueGet(TIMERA0_BASE, TIMER_A);
		break;
	case TIMER_1:
		return TimerValueGet(TIMERA1_BASE, TIMER_A);
		break;
	case TIMER_2:
		return TimerValueGet(TIMERA2_BASE, TIMER_A);
		break;
	case TIMER_3:
		return TimerValueGet(TIMERA3_BASE, TIMER_A);
		break;
	case MAX_TIMERS:
	default:
		return 0;
	}
}

void timer_start(tim_t dev) {
	switch (dev) {
	case TIMER_0:
		MAP_TimerEnable(TIMERA0_BASE, TIMER_A);
		break;
	case TIMER_1:
		MAP_TimerEnable(TIMERA1_BASE, TIMER_A);
		break;
	case TIMER_2:
		MAP_TimerEnable(TIMERA2_BASE, TIMER_A);
		break;
	case TIMER_3:
		MAP_TimerEnable(TIMERA3_BASE, TIMER_A);
		break;
	default:
		break;
	}
}

void timer_stop(tim_t dev) {
	switch (dev) {
	case TIMER_0:
		MAP_TimerDisable(TIMERA0_BASE, TIMER_A);
		break;
	case TIMER_1:
		MAP_TimerDisable(TIMERA1_BASE, TIMER_A);
		break;
	case TIMER_2:
		MAP_TimerDisable(TIMERA2_BASE, TIMER_A);
		break;
	case TIMER_3:
		MAP_TimerDisable(TIMERA3_BASE, TIMER_A);
		break;
	default:
		break;
	}
}

void timer_irq_enable(tim_t dev) {
	switch (dev) {
	case TIMER_0:
		MAP_TimerIntEnable(TIMERA0_BASE, TIMER_TIMA_MATCH);
		break;
	case TIMER_1:
		MAP_TimerIntEnable(TIMERA1_BASE, TIMER_TIMA_MATCH);
		break;
	case TIMER_2:
		MAP_TimerIntEnable(TIMERA2_BASE, TIMER_TIMA_MATCH);
		break;
	case TIMER_3:
		MAP_TimerIntEnable(TIMERA3_BASE, TIMER_TIMA_MATCH);
		break;
	default:
		break;
	}
}

void timer_irq_disable(tim_t dev) {
	switch (dev) {
	case TIMER_0:
		MAP_TimerIntDisable(TIMERA0_BASE, TIMER_TIMA_MATCH);
		break;
	case TIMER_1:
		MAP_TimerIntDisable(TIMERA1_BASE, TIMER_TIMA_MATCH);
		break;
	case TIMER_2:
		MAP_TimerIntDisable(TIMERA2_BASE, TIMER_TIMA_MATCH);
		break;
	case TIMER_3:
		MAP_TimerIntDisable(TIMERA3_BASE, TIMER_TIMA_MATCH);
		break;
	default:
		break;
	}
}

void timer_reset(tim_t dev) {
	switch (dev) {
	case TIMER_0:
		TimerValueSet(TIMERA0_BASE, TIMER_A, 0);
		break;
	case TIMER_1:
		TimerValueSet(TIMERA1_BASE, TIMER_A, 0);
		break;
	case TIMER_2:
		TimerValueSet(TIMERA2_BASE, TIMER_A, 0);
		break;
	case TIMER_3:
		TimerValueSet(TIMERA3_BASE, TIMER_A, 0);
		break;
	default:
		break;
	}
}

