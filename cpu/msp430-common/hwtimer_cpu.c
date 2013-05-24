/******************************************************************************
Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

RIOT is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/ .
--------------------------------------------------------------------------------
For further information and questions please use the web site
	http://scatterweb.mi.fu-berlin.de
and the mailinglist (subscription via web site)
	scatterweb@lists.spline.inf.fu-berlin.de
*******************************************************************************/

#include <stdint.h>

#include <cpu.h>
#include <hwtimer.h>
#include <hwtimer_arch.h>

// #define ENABLE_DEBUG (1)
#include <debug.h>


void (*int_handler)(int);
extern void timerA_init(void);
uint16_t overflow_interrupt[ARCH_MAXTIMERS+1];
uint16_t timer_round;

static void TA0_disable_interrupt(short timer) {
    volatile unsigned int *ptr = &TA0CCTL0 + (timer);
    *ptr &= ~(CCIFG);
    *ptr &= ~(CCIE);
}

static void TA0_enable_interrupt(short timer) {
    volatile unsigned int *ptr = &TA0CCTL0 + (timer);
    *ptr |= CCIE;
    *ptr &= ~(CCIFG);
}

static void TA0_set_nostart(unsigned long value, short timer) {
    volatile unsigned int *ptr = &TA0CCR0 + (timer);
    *ptr = value;
}

static void TA0_set(unsigned long value, short timer) {
    DEBUG("Setting timer %u to %lu\n", timer, value);
    TA0_set_nostart(value, timer);
    TA0_enable_interrupt(timer);
}

void TA0_unset(short timer) {
    volatile unsigned int *ptr = &TA0CCR0 + (timer);
    TA0_disable_interrupt(timer);
    *ptr = 0;
}

unsigned long hwtimer_arch_now() {
	return ((uint32_t)timer_round << 16)+TA0R;
}

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu) {
    timerA_init();
    int_handler = handler;
    TA0_enable_interrupt(0);
}

void hwtimer_arch_enable_interrupt(void) {
    for (int i = 0; i < ARCH_MAXTIMERS; i++) {
        TA0_enable_interrupt(i);
    }
}

void hwtimer_arch_disable_interrupt(void) {
    for (int i = 0; i < ARCH_MAXTIMERS; i++) {
        TA0_disable_interrupt(i);
    }
}

void hwtimer_arch_set(unsigned long offset, short timer) {
    unsigned long value = hwtimer_arch_now() + offset;
    hwtimer_arch_set_absolute(value, timer);
}

void hwtimer_arch_set_absolute(unsigned long value, short timer) {
	uint16_t small_value = value % 0xFFFF;
	overflow_interrupt[timer] = (uint16_t)(value >> 16);
    TA0_set(small_value,timer);
}

void hwtimer_arch_unset(short timer) {
    TA0_unset(timer);
}
