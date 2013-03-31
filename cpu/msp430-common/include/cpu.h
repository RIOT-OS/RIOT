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

#ifndef _CPU_H
#define _CPU_H

/**
 * @defgroup	msp430		TI MSP430
 * @ingroup		cpu

<h2>First steps</h2>
\li	See the <a href="../manual/index.html">manual</a> for toolchain and ide setup

 * @{
 */

#include <sched.h>
#include <stdio.h>
#include <legacymsp430.h>
#include <cpu-conf.h>

#define WORDSIZE 16

/* not used(?) */
#define F_CPU 10000000

extern volatile int __inISR;
extern char __isr_stack[MSP430_ISR_STACK_SIZE];

//#define eINT()  eint()
//#define dINT()  dint()

inline void __save_context_isr(void) {
    __asm__("push r15");
    __asm__("push r14");
    __asm__("push r13");
    __asm__("push r12");
    __asm__("push r11");
    __asm__("push r10");
    __asm__("push r9");
    __asm__("push r8");
    __asm__("push r7");
    __asm__("push r6");
    __asm__("push r5");
    __asm__("push r4");

    __asm__("mov.w r1,%0" : "=r" (active_thread->sp));
}

inline void __restore_context_isr(void) {
    __asm__("mov.w %0,r1" : : "m" (active_thread->sp));

    __asm__("pop r4");
    __asm__("pop r5");
    __asm__("pop r6");
    __asm__("pop r7");
    __asm__("pop r8");
    __asm__("pop r9");
    __asm__("pop r10");
    __asm__("pop r11");
    __asm__("pop r12");
    __asm__("pop r13");
    __asm__("pop r14");
    __asm__("pop r15");
}

inline void __enter_isr(void) {
    __save_context_isr();
    __asm__("mov.w %0,r1" : : "i" (__isr_stack+MSP430_ISR_STACK_SIZE));
    __inISR = 1;
}

inline void __exit_isr(void) {
    __inISR = 0;
    if (sched_context_switch_request) sched_run();
    __restore_context_isr();
    __asm__("reti");
}

inline void __save_context(void) {
    __asm__("push r2"); /* save SR */
    __save_context_isr();
}

inline void __restore_context(void) {
    __restore_context_isr();
    __asm__("reti");
}

inline void eINT(void) {
//    puts("+");
    eint();
}

inline void dINT(void) {
//    puts("-");
    dint();
}

#define lpm_set(...)

void thread_yield(void);


int inISR(void);

/** @} */
#endif // _CPU_H

