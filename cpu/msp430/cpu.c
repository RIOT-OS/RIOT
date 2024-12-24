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
 * This function can both be called from ISR and from thread context.
 *
 * In both cases, the caller will use "CALL", which pushes the return
 * address on the stack before executing the function's first instruction.
 *
 * If called within ISR, just set sched_context_switch_request and
 * directly return to the call site. A regular function return does this.
 *
 * If called from stack context, it needs to prepare the stack so it looks exactly
 * as if the thread had been interrupted by an ISR, which requires the SR to be on
 * stack right after the return address. So we do this manually.
 * __save_context() will then save the remaining registers, then store the stack
 * pointer in the thread's thread_t.
 *
 * At this point, the thread context is properly saved. sched_run (possibly) changes
 * the currently active thread, which __restore_context() then restores, resuming
 * execution at the call site using reti.
 *
 */
void thread_yield_higher_arch(void)
{
    if (irq_is_in()) {
        sched_context_switch_request = 1;
    }
    else {
        __asm__ volatile (
            "push r2"   "\n\t"  /* save SR */
            "dint"      "\n\t"  /* reti will restore SR, and thus, IRQ state */
            "nop"       "\n\t"  /* dint takes an additional CPU cycle to take into effect */
            :                   /* no outputs */
            :                   /* no inputs */
            :                   /* no clobbers */
            );

        __save_context();

        /* have thread_get_active() point to the next thread */
        sched_run();

        __restore_context();

        UNREACHABLE();
    }
}

/* This function calculates the ISR_usage */
int thread_isr_stack_usage(void)
{
    /* TODO */
    return -1;
}

void *thread_isr_stack_pointer(void)
{
    /* TODO */
    return (void *)-1;
}

void *thread_isr_stack_start(void)
{
    /* TODO */
    return (void *)-1;
}

NORETURN void cpu_switch_context_exit(void)
{
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

/* ------------------------------------------------------------------------- */
/*  Processor specific routine - here for MSP */
/* ------------------------------------------------------------------------- */
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

    /* Space for registers. */
    for (unsigned int i = 15; i > 4; i--) {
        *stackptr = i;
        --stackptr;
    }

    /* set arg to R12
       this was R15 in mspgcc, see https://www.ti.com/lit/an/slaa664/slaa664.pdf
       stackptr points to R3, so write arg 9 words after that.
    */
    stackptr[8] = (intptr_t)arg;

    return (char *) stackptr;
}
