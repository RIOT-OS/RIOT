/******************************************************************************
Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This file subject to the terms and conditions of the GNU Lesser General Public
License. See the file LICENSE in the top level directory for more details.
*******************************************************************************/

#include <stdint.h>

#include <cpu.h>
#include <hwtimer.h>
#include <hwtimer_arch.h>

#define ENABLE_DEBUG (0)
#include <debug.h>


void (*int_handler)(int);
extern void timerB_init(void);
uint16_t overflow_interrupt[ARCH_MAXTIMERS+1];
uint16_t timer_round;

static void TB_disable_interrupt(short timer)
{
    volatile unsigned int *ptr = &TBCCTL0 + (timer);
    *ptr &= ~(CCIFG);
    *ptr &= ~(CCIE);
}

static void TB_enable_interrupt(short timer)
{
    volatile unsigned int *ptr = &TBCCTL0 + (timer);
    *ptr |= CCIE;
    *ptr &= ~(CCIFG);
}

static void TB_set_nostart(unsigned long value, short timer)
{
    volatile unsigned int *ptr = &TBCCR0 + (timer);
    *ptr = value;
}

static void TB_set(unsigned long value, short timer)
{
    DEBUG("Setting timer %u to %lu\n", timer, value);
    TB_set_nostart(value, timer);
    TB_enable_interrupt(timer);
}

void TB_unset(short timer)
{
    volatile unsigned int *ptr = &TBCCR0 + (timer);
    TB_disable_interrupt(timer);
    *ptr = 0;
}

unsigned long hwtimer_arch_now()
{
    return TBR;
}

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    (void) fcpu;
    timerB_init();
    int_handler = handler;
    TB_enable_interrupt(0);
}

void hwtimer_arch_enable_interrupt(void)
{
    for (int i = 0; i < ARCH_MAXTIMERS; i++) {
        TB_enable_interrupt(i);
    }
}

void hwtimer_arch_disable_interrupt(void)
{
    for (int i = 0; i < ARCH_MAXTIMERS; i++) {
        TB_disable_interrupt(i);
    }
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
    unsigned int value = hwtimer_arch_now() + offset;
    hwtimer_arch_set_absolute(value, timer);
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    uint16_t small_value = value % 0xFFFF;
    overflow_interrupt[timer] = (uint16_t)(value >> 16);
    TB_set(small_value,timer);
}

void hwtimer_arch_unset(short timer)
{
    TB_unset(timer);
}
