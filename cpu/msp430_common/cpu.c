/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014, Freie Universitaet Berlin (FUB) & INRIA.
 * All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "cpu.h"
#include "irq.h"
#include "sched.h"
#include "thread.h"

/*
 * we must prevent the compiler to generate a prologue or an epilogue
 * for thread_yield_higher(), since we rely on the RETI instruction at the end
 * of its execution, in the inlined __restore_context() sub-function
 */
__attribute__((naked)) void thread_yield_higher(void)
{
    __asm__("push r2"); /* save SR */
    __disable_irq();

    __save_context();

    /* have sched_active_thread point to the next thread */
    sched_run();

    __restore_context();

    UNREACHABLE();
}

/* This function calculates the ISR_usage */
int thread_arch_isr_stack_usage(void)
{
    /* TODO */
    return -1;
}

void *thread_arch_isr_stack_pointer(void)
{
    /* TODO */
    return (void *)-1;
}

void *thread_arch_isr_stack_start(void)
{
    /* TODO */
    return (void *)-1;
}

NORETURN void cpu_switch_context_exit(void)
{
    sched_active_thread = sched_threads[0];
    sched_run();

    __restore_context();

    UNREACHABLE();
}

/**
 * mspgcc handles main specially - it does not return but falls
 * through to section .fini9.
 * To "fix" this, we put a return in section .fini9 to make main
 * behave like a regular function. This enables a common
 * thread_stack_init behavior. */
__attribute__((section (".fini9"))) void __main_epilogue(void) { __asm__("ret"); }

//----------------------------------------------------------------------------
// Processor specific routine - here for MSP
//----------------------------------------------------------------------------
char *thread_stack_init(thread_task_func_t task_func, void *arg, void *stack_start, int stack_size)
{
    unsigned short stk = (unsigned short)((uintptr_t) stack_start + stack_size);

    /* ensure correct stack alignment (on 16-bit boundary) */
    stk &= 0xfffe;
    unsigned short *stackptr = (unsigned short *)stk;

    /* now make SP point on the first AVAILABLE slot in stack */
    --stackptr;

    *stackptr = (unsigned short) sched_task_exit;
    --stackptr;

    *stackptr = (unsigned short) task_func;
    --stackptr;

    /* initial value for SR */

    *stackptr = GIE;
    --stackptr;

    /* set argument to task_func */
    *stackptr = (unsigned short) arg;
    --stackptr;

    /* Space for registers. */
    for (unsigned int i = 14; i > 4; i--) {
        *stackptr = i;
        --stackptr;
    }

    return (char *) stackptr;
}
