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

// #define ENABLE_DEBUG (1)
#include <debug.h>


void (*int_handler)(int);
extern void timerA_init(void);

static void TA0_disable_interrupt(short timer)
{
    volatile unsigned int *ptr = &TA0CCTL0 + (timer);
    *ptr &= ~(CCIFG);
    *ptr &= ~(CCIE);
}

static void TA0_enable_interrupt(short timer)
{
    volatile unsigned int *ptr = &TA0CCTL0 + (timer);
    *ptr |= CCIE;
    *ptr &= ~(CCIFG);
}

static void TA0_set_nostart(unsigned long value, short timer)
{
    volatile unsigned int *ptr = &TA0CCR0 + (timer);
    *ptr = value;
}

static void TA0_set(unsigned long value, short timer)
{
    DEBUG("Setting timer %u to %lu\n", timer, value);
    TA0_set_nostart(value, timer);
    TA0_enable_interrupt(timer);
}

void TA0_unset(short timer)
{
    volatile unsigned int *ptr = &TA0CCR0 + (timer);
    TA0_disable_interrupt(timer);
    *ptr = 0;
}

unsigned long hwtimer_arch_now()
{
    return TA0R;
}

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    timerA_init();
    int_handler = handler;
}

void hwtimer_arch_enable_interrupt(void)
{
    for(int i = 0; i < ARCH_MAXTIMERS; i++) {
        TA0_enable_interrupt(i);
    }
}

void hwtimer_arch_disable_interrupt(void)
{
    for(int i = 0; i < ARCH_MAXTIMERS; i++) {
        TA0_disable_interrupt(i);
    }
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
    unsigned int value = hwtimer_arch_now() + offset;
    hwtimer_arch_set_absolute(value, timer);
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    TA0_set(value, timer);
}

void hwtimer_arch_unset(short timer)
{
    TA0_unset(timer);
}
