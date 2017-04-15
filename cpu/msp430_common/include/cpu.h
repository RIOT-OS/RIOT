/*
 * Copyright (C) 2014, Freie Universitaet Berlin (FUB) & INRIA.
 * All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef CPU_H
#define CPU_H

/**
 * @defgroup    cpu_msp430_common TI MSP430
 * @ingroup     cpu
 * @brief       Texas Instruments MSP430 specific code

<h2>First steps</h2>
\li See the <a href="../manual/index.html">manual</a> for toolchain and ide setup

 * @{
 */

#include <stdio.h>

#include <msp430.h>
#include "board.h"

#include "sched.h"
#include "thread.h"
#include "msp430_types.h"
#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Wordsize in bit for MSP430 platforms
 */
#define WORDSIZE 16

/**
 * @brief   Macro for defining interrupt service routines
 */
#define ISR(a,b)        void __attribute__((naked, interrupt (a))) b(void)

/**
 * @brief Globally disable IRQs
 */
static inline void __attribute__((always_inline)) __disable_irq(void)
{
    __asm__ __volatile__("bic  %0, r2" : : "i"(GIE));
    /* this NOP is needed to handle a "delay slot" that all MSP430 MCUs
       impose silently after messing with the GIE bit, DO NOT REMOVE IT! */
    __asm__ __volatile__("nop");
}

/**
 * @brief Globally enable IRQs
 */
static inline void __attribute__((always_inline)) __enable_irq(void)
{
    __asm__ __volatile__("bis  %0, r2" : : "i"(GIE));
    /* this NOP is needed to handle a "delay slot" that all MSP430 MCUs
       impose silently after messing with the GIE bit, DO NOT REMOVE IT! */
    __asm__ __volatile__("nop");
}

/**
 * @brief   The current ISR state (inside or not)
 */
extern volatile int __irq_is_in;

/**
 * @brief   Memory used as stack for the interrupt context
 */
extern char __isr_stack[ISR_STACKSIZE];

/**
 * @brief   Save the current thread context from inside an ISR
 */
static inline void __attribute__((always_inline)) __save_context(void)
{
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

    __asm__("mov.w r1,%0" : "=r"(sched_active_thread->sp));
}

/**
 * @brief   Restore the thread context from inside an ISR
 */
static inline void __attribute__((always_inline)) __restore_context(void)
{
    __asm__("mov.w %0,r1" : : "m"(sched_active_thread->sp));

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
    __asm__("reti");
}

/**
 * @brief   Run this code on entering interrupt routines
 */
static inline void __attribute__((always_inline)) __enter_isr(void)
{
    __save_context();
    __asm__("mov.w %0,r1" : : "i"(__isr_stack + ISR_STACKSIZE));
    __irq_is_in = 1;
}

/**
 * @brief   Run this code on exiting interrupt routines
 */
static inline void __attribute__((always_inline)) __exit_isr(void)
{
    __irq_is_in = 0;

    if (sched_context_switch_request) {
        sched_run();
    }

    __restore_context();
}

/**
 * @brief   Initialize the cpu
 */
void msp430_cpu_init(void);

/**
 * @brief   Print the last instruction's address
 *
 * @todo:   Not supported
 */
static inline void cpu_print_last_instruction(void)
{
    puts("n/a");
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CPU_H */
