/******************************************************************************
Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This file is subject to the terms and conditions of the LGPLv2.
See the file LICENSE in the top level directory for more details.
*******************************************************************************/

#include "cpu.h"
#include "kernel.h"
#include "kernel_internal.h"
#include "sched.h"
#include "thread.h"

volatile int __inISR = 0;

char __isr_stack[MSP430_ISR_STACK_SIZE];

void thread_yield(void)
{
    __save_context();

    dINT();
    /* have sched_active_thread point to the next thread */
    sched_run();
    eINT();

    __restore_context();
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

/*------------------------------------------------------------------------------*/
/* Processor specific routine - here for MSP */
/*------------------------------------------------------------------------------*/
char *thread_stack_init(thread_task_func_t task_func, void *arg, void *stack_start, int stack_size)
{
    unsigned short stk = (unsigned short)(stack_start + stack_size);

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

int inISR(void)
{
    return __inISR;
}

/******************************************************************************/

/* System reboot */
int reboot_arch(int mode)
{
    (void) mode;

    /* force an hardware reboot ("Power-Up Clear"), by writing
       an illegal value to the watchdog control register */
    while (1) {
        WDTCTL = 0x0000;
    }

    return -1;
}
