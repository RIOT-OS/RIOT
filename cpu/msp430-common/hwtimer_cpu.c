/*
 * Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>

#include "cpu.h"
#include "hwtimer.h"
#include "arch/hwtimer_arch.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


void (*int_handler)(int);
extern void timerA_init(void);
volatile uint16_t overflow_interrupt[HWTIMER_MAXTIMERS+1];
volatile uint16_t timer_round;

#ifdef CC430
  /* CC430 have "TimerA0", "TimerA1" and so on... */
  #define CNT_CTRL_BASE_REG  (TA0CCTL0)
  #define CNT_COMP_BASE_REG  (TA0CCR0)
  #define TIMER_VAL_REG      (TA0R)
#else
  /* ... while other MSP430 MCUs have "TimerA", "TimerB".
     Cheers for TI and its consistency! */
  #define CNT_CTRL_BASE_REG  (TACCTL0)
  #define CNT_COMP_BASE_REG  (TACCR0)
  #define TIMER_VAL_REG      (TAR)
#endif

static void timer_disable_interrupt(short timer)
{
    volatile unsigned int *ptr = &CNT_CTRL_BASE_REG + (timer);
    *ptr &= ~(CCIFG);
    *ptr &= ~(CCIE);
}

static void timer_enable_interrupt(short timer)
{
    volatile unsigned int *ptr = &CNT_CTRL_BASE_REG + (timer);
    *ptr |= CCIE;
    *ptr &= ~(CCIFG);
}

static void timer_set_nostart(uint32_t value, short timer)
{
    volatile unsigned int *ptr = &CNT_COMP_BASE_REG + (timer);
    /* ensure we won't set the timer to a "past" tick */
    if (value <= hwtimer_arch_now()) {
        value = hwtimer_arch_now() + 2;
    }
    overflow_interrupt[timer] = (uint16_t)(value >> 16);
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
    volatile unsigned int *ptr = &CNT_COMP_BASE_REG + (timer);
    timer_disable_interrupt(timer);
    *ptr = 0;
}

unsigned long hwtimer_arch_now(void)
{
    return ((uint32_t)timer_round << 16) + TIMER_VAL_REG;
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
