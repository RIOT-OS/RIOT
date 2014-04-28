/*
 * Copyright (C) 2014 Freie Universitaet Berlin (FUB) and INRIA. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpu
 * @{
 */

/**
 * @file
 * @brief       msp430 hardware timer driver generic functions
 *
 * @author      Freie Universitaet Berlin, Computer Systems and Telematics group
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 *
 */

#include <stdint.h>

#include "cpu.h"
#include "crash.h"
#include "hwtimer.h"
#include "arch/hwtimer_arch.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


void (*int_handler)(int);
extern void timerA_init(void);

extern volatile msp430_timer_t msp430_timer[HWTIMER_MAXTIMERS];

/*
 * the 3 following functions handle the diversity of timers
 *  we can encounter in the various MCUs in the MSP430 family
 */

static volatile unsigned int *get_control_reg_for_msp430_timer(int index)
{
    volatile unsigned int *ptr = NULL;
#ifndef CC430
    switch (msp430_timer[index].base_timer) {
    case TIMER_A:
        ptr = &TACCTL0;
        break;
    case TIMER_B:
        ptr = &TBCCTL0;
        break;
    default:
        core_panic(0x0, "Wrong timer kind for MSP430");
    }
#else
    ptr = &TA0CCTL0;
#endif
    ptr += msp430_timer[index].ccr_num;
    return ptr;
}

static volatile unsigned int *get_comparator_reg_for_msp430_timer(int index)
{
    volatile unsigned int *ptr = NULL;
#ifndef CC430
    switch (msp430_timer[index].base_timer) {
    case TIMER_A:
        ptr = &TACCR0;
        break;
    case TIMER_B:
        ptr = &TBCCR0;
        break;
    default:
        core_panic(0x0, "Wrong timer kind for MSP430");
    }
#else
    ptr = &TA0CCR0;
#endif
    ptr += msp430_timer[index].ccr_num;
    return ptr;
}

#ifdef CC430
  /* CC430 have "TimerA0", "TimerA1" and so on... */
  #define TIMER_VAL_REG (TA0R)
#else
  /* ... while other MSP430 MCUs have "TimerA", "TimerB".
         Cheers for TI and its consistency! */
  #define TIMER_VAL_REG (TAR)
#endif

/* hardware-dependent functions */

static void timer_disable_interrupt(short timer)
{
    volatile unsigned int *ptr = get_control_reg_for_msp430_timer(timer);
    *ptr &= ~(CCIFG);
    *ptr &= ~(CCIE);
}

static void timer_enable_interrupt(short timer)
{
    volatile unsigned int *ptr = get_control_reg_for_msp430_timer(timer);
    *ptr |= CCIE;
    *ptr &= ~(CCIFG);
}

static void timer_set_nostart(uint32_t value, short timer)
{
    volatile unsigned int *ptr = get_comparator_reg_for_msp430_timer(timer);
    /* ensure we won't set the timer to a "past" tick */
    if (value <= hwtimer_arch_now()) {
        value = hwtimer_arch_now() + 2;
    }
    *ptr = (value & 0xFFFF);
}

static void timer_set(uint32_t value, short timer)
{
    DEBUG("Setting timer %u to %lu\n", timer, value);
    timer_set_nostart(value, timer);
    timer_enable_interrupt(timer);
}

void timer_unset(short timer)
{
    volatile unsigned int *ptr = get_comparator_reg_for_msp430_timer(timer);
    timer_disable_interrupt(timer);
    *ptr = 0;
}

unsigned long hwtimer_arch_now(void)
{
    return (TIMER_VAL_REG & 0xffff);
}

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    (void) fcpu;
    timerA_init();
    int_handler = handler;
}

void hwtimer_arch_enable_interrupt(void)
{
    for (int i = 0; i < HWTIMER_MAXTIMERS; i++) {
        timer_enable_interrupt(i);
    }
}

void hwtimer_arch_disable_interrupt(void)
{
    for (int i = 0; i < HWTIMER_MAXTIMERS; i++) {
        timer_disable_interrupt(i);
    }
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
    uint32_t value = hwtimer_arch_now() + offset;
    hwtimer_arch_set_absolute(value, timer);
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    timer_set(value, timer);
}

void hwtimer_arch_unset(short timer)
{
    timer_unset(timer);
}
