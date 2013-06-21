/**
 * ARM architecture common support functions
 *
 * Copyright (C) 2008, 2009  Heiko Will <hwill@inf.fu-berlin.de>
 * Copyright (C) 2009  Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup arch
 * @{
 * @file
 * @author Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
 * @author Heiko Will <heiko.will@fu-berlin.de>
 * @}
 */
/**
 *
 *
 *
 */

#include <stdio.h>
#include "arm_cpu.h"
#include "sched.h"
#include "kernel_intern.h"

#define STACK_MARKER    (0x77777777)
#define REGISTER_CNT    (12)

void thread_yield(void)
{
    asm("svc 0\n");
}

/*----------------------------------------------------------------------------
 * Processor specific routine - here for ARM7
 * sizeof(void*) = sizeof(int)
 *--------------------------------------------------------------------------*/
char *thread_stack_init(void *task_func, void *stack_start, int stack_size)
{
    unsigned int *stk;
    stk = (unsigned int *)(stack_start + stack_size);
    stk--;

    *stk = STACK_MARKER;

    /* set the return address (LR) */
    stk--;
    *stk = (unsigned int) sched_task_exit;

    /* set the stack pointer (SP) */
    stk--;
    *stk = (unsigned int)(stack_start + stack_size) - 4;

    /* build base stack */
    for(int i = REGISTER_CNT; i >= 0 ; i--) {
        stk--;
        *stk = i;
    }

    /* set the entry point */
    stk--;
    *stk = ((unsigned int) task_func);
    /* set the saved program status register */
    stk--;
    *stk = (unsigned int) NEW_TASK_CPSR;

    return (char *)stk;
}

void thread_print_stack(void)
{
    register void *stack = 0;
    asm("mov %0, sp" : "=r"(stack));

    register unsigned int *s = (unsigned int *)stack;
    printf("task: %X SP: %X\n", (unsigned int) active_thread, (unsigned int) stack);
    register int i = 0;
    s += 5;

    while(*s != STACK_MARKER) {
        printf("STACK (%u) addr=%X = %X \n", i, (unsigned int) s, (unsigned int) *s);
        s++;
        i++;
    }

    printf("STACK (%u)= %X \n", i, *s);
}

__attribute__((naked, noreturn)) void arm_reset(void)
{
    dINT();
    WDTC   = 0x00FFF;
    WDMOD  = 0x03;
    WDFEED = 0xAA;
    WDFEED = 0x55;

    while(1);
}
