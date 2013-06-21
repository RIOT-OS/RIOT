/******************************************************************************
Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This file subject to the terms and conditions of the GNU Lesser General Public
License. See the file LICENSE in the top level directory for more details.
*******************************************************************************/
#ifdef CC430
#include <cc430f6137.h>
#else
#include <msp430x16x.h>
#endif
#include "cpu.h"
#include "kernel.h"
#include "kernel_intern.h"
#include "sched.h"

volatile int __inISR = 0;

char __isr_stack[MSP430_ISR_STACK_SIZE];

void thread_yield()
{
    __save_context();

    dINT();
    /* have active_thread point to the next thread */
    sched_run();
    eINT();

    __restore_context();
}

void cpu_switch_context_exit(void)
{
    active_thread = sched_threads[0];
    sched_run();

    __restore_context();
}

//----------------------------------------------------------------------------
// Processor specific routine - here for MSP
//----------------------------------------------------------------------------
char *thread_stack_init(void *task_func, void *stack_start, int stack_size)
{
    unsigned short *stk;
    stk = (unsigned short *)(stack_start + stack_size);

    *stk = (unsigned short) sched_task_exit;
    --stk;

    *stk = (unsigned short) task_func;
    --stk;

    /* initial value for SR */

    *stk = GIE;
    --stk;

    /* Space for registers. */
    for(unsigned int i = 15; i > 4; i--) {
        *stk = i;
        --stk;
    }

    //stk -= 11;

    return (char *) stk;
}

int inISR()
{
    return __inISR;
}
