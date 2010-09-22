/******************************************************************************
Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of FeuerWare.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

FeuerWare is distributed in the hope that it will be useful, but WITHOUT
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
#include <board.h>
#include <hwtimer.h>
#include <hwtimer_arch.h>

#include "debug.h"

static uint32_t ticks = 0;

static void (*int_handler)(int);

static void timerA_init()
{
  	ticks = 0;								 // Set tick counter value to 0
	TA0CTL = TASSEL_1 + TACLR;   	  		 // Clear the timer counter, set SMCLK
	TA0CTL &= ~TAIFG;						 // Clear the IFG
	TA0CTL &= ~TAIE;						 // Clear the IFG

    volatile unsigned int *ccr = &TA0CCR0;
    volatile unsigned int *ctl = &TA0CCTL0;

    for (int i = 0; i < ARCH_MAXTIMERS; i++) {
        *ccr = 0;
        *ctl &= ~(CCIFG);
        *ctl &= ~(CCIE);
    }
	
    TA0CTL |= MC_2;
}


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
//    printf("Setting timer %u to %lu\n", timer, value);
    TA0_set_nostart(value, timer);
    TA0_enable_interrupt(timer);
}

static void TA0_unset(short timer) {
    volatile unsigned int *ptr = &TA0CCR0 + (timer);
    TA0_disable_interrupt(timer);
    *ptr = 0;
}

unsigned long hwtimer_arch_now() {
	return TA0R;
}

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu) {
    timerA_init();
    int_handler = handler;
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
    unsigned int value = hwtimer_arch_now() + offset;
    hwtimer_arch_set_absolute(value, timer);
}

void hwtimer_arch_set_absolute(unsigned long value, short timer) {
    TA0_set(value,timer);
}

void hwtimer_arch_unset(short timer) {
    TA0_unset(timer);
}

interrupt(TIMERA0_VECTOR) __attribute__ ((naked)) timer_isr_ccr0(void)
{
    __enter_isr();

    TA0_unset(0);
    int_handler(0);
   
    __exit_isr();
}

interrupt(TIMERA1_VECTOR) __attribute__ ((naked)) timer_isr(void)
{
    __enter_isr();
    
    short taiv = TA0IV;

    if (taiv & TAIFG) {
        puts("msp430/hwtimer_cpu TAIFG set!");
    //    TA0CTL &= ~TAIFG;
    //    ticks += 0xFFFF;
    } else {

        short timer = (taiv/2);
        TA0_unset(timer);
        int_handler(timer);
    }
    
    __exit_isr();
}
