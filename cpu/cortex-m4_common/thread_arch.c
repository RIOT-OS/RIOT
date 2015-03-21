/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cortexm4_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernel's architecture dependent thread interface
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "arch/thread_arch.h"
#include "sched.h"
#include "thread.h"
#include "irq.h"
#include "cpu.h"
#include "kernel_internal.h"

/**
 * @name noticeable marker marking the beginning of a stack segment
 *
 * This marker is used e.g. by *thread_arch_start_threading* to identify the stacks start.
 */
#define STACK_MARKER                (0x77777777)

/**
 * @name ARM Cortex-M specific exception return value, that triggers the return to the task mode
 *       stack pointer
 */
#define EXCEPT_RET_TASK_MODE        (0xfffffffd)

/**
 * Cortex-M knows stacks and handles register backups, so use different stack frame layout
 *
 * TODO: How to handle different Cortex-Ms? Code is so far valid for M3 and M4 without FPU
 *
 * Layout with storage of floating point registers (applicable for Cortex-M4):
 * ------------------------------------------------------------------------------------------------------------------------------------
 * | R0 | R1 | R2 | R3 | LR | PC | xPSR | S0 | S1 | S2 | S3 | S4 | S5 | S6 | S7 | S8 | S9 | S10 | S11 | S12 | S13 | S14 | S15 | FPSCR |
 * ------------------------------------------------------------------------------------------------------------------------------------
 *
 * Layout without floating point registers:
 * --------------------------------------
 * | R0 | R1 | R2 | R3 | LR | PC | xPSR |
 * --------------------------------------
 *
 */
char *thread_arch_stack_init(thread_task_func_t task_func,
                             void *arg,
                             void *stack_start,
                             int stack_size)
{
    uint32_t *stk;
    stk = (uint32_t *)((uintptr_t)stack_start + stack_size);

    /* marker */
    stk--;
    *stk = STACK_MARKER;

    /* TODO: fix FPU handling for Cortex-M4 */
    /*
    stk--;
    *stk = (unsigned int) 0;
    */

    /* S0 - S15 */
    /*
    for (int i = 15; i >= 0; i--) {
        stk--;
        *stk = i;
    }
    */

    /* FIXME xPSR */
    stk--;
    *stk = (uint32_t) 0x01000200;

    /* program counter */
    stk--;
    *stk = (uint32_t) task_func;

    /* link register, jumped to when thread exits */
    stk--;
    *stk = (uint32_t) sched_task_exit;

    /* r12 */
    stk--;
    *stk = (uint32_t) 0;

    /* r1 - r3 */
    for (int i = 3; i >= 1; i--) {
        stk--;
        *stk = i;
    }

    /* r0 -> thread function parameter */
    stk--;
    *stk = (uint32_t) arg;

    /* r11 - r4 */
    for (int i = 11; i >= 4; i--) {
        stk--;
        *stk = i;
    }

    /* lr means exception return code  */
    stk--;
    *stk = EXCEPT_RET_TASK_MODE; /* return to task-mode main stack pointer */

    return (char*) stk;
}

void thread_arch_stack_print(void)
{
    int count = 0;
    uint32_t *sp = (uint32_t *)sched_active_thread->sp;

    printf("printing the current stack of thread %" PRIkernel_pid "\n", thread_getpid());
    printf("  address:      data:\n");

    do {
        printf("  0x%08x:   0x%08x\n", (unsigned int)sp, (unsigned int)*sp);
        sp++;
        count++;
    } while (*sp != STACK_MARKER);

    printf("current stack size: %i byte\n", count);
}

__attribute__((naked)) void NORETURN thread_arch_start_threading(void)
{
    /* enable IRQs to make sure the SVC interrupt is reachable */
    __ASM volatile ("bl irq_arch_enable\n");

    /* trigger the SVC interrupt which will get and execute the next thread */
    __ASM volatile ("svc #1\n");

    /* This line is unreachable, infinite loop */
    __ASM volatile (
        "unreachable%=:\n"
        "b unreachable%=\n"
        :::);
}

void thread_arch_yield(void)
{
    /* trigger the PENDSV interrupt to run scheduler and schedule new thread if applicable */
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

/**
 * @brief The svc is used for running the scheduler and scheduling a new task during start-up or
 *        after a thread has exited
 */
void isr_svc(void);

/**
 * @brief All task switching activity is carried out in the PendSV interrupt
 */
void isr_pendsv(void);

__attribute__((naked)) void arch_context_switch(void)
{
    /* {r0-r3,r12,LR,PC,xPSR} are saved automatically on exception entry */
    __ASM volatile (
    /* PendSV handler entry point */
    ".global isr_pendsv               \n"
    ".thumb_func                      \n"
    "isr_pendsv:                      \n"
    /* Save the context */
    /* save unsaved registers */
    ".thumb_func                      \n"
    "context_save:"
    "mrs    r0, psp                   \n" /* get stack pointer from user mode */
    "stmdb  r0!,{r4-r11}              \n" /* save regs */
    "stmdb  r0!,{lr}                  \n" /* exception return value */
/*  "vstmdb sp!, {s16-s31}            \n" */ /* TODO save FPU registers if needed */
    "ldr    r1, =sched_active_thread  \n" /* load address of current tcb */
    "ldr    r1, [r1]                  \n" /* dereference pdc */
    "str    r0, [r1]                  \n" /* write r0 to pdc->sp means current threads stack pointer */
    /* SVC handler entry point */
    /* PendSV will continue from above and through this part as well */
    ".global isr_svc                  \n"
    ".thumb_func                      \n"
    "isr_svc:                         \n"
    /* perform scheduling */
    "bl     sched_run                 \n"
    /* Restore context and return from exception */
    ".thumb_func                      \n"
    "context_restore:                 \n"
    "ldr    r0, =sched_active_thread  \n" /* load address of current TCB */
    "ldr    r0, [r0]                  \n" /* dereference TCB */
    "ldr    r1, [r0]                  \n" /* load tcb->sp to register 1 */
    "ldmia  r1!, {r0}                 \n" /* restore exception return value from stack */
/*  "pop    {s16-s31}                 \n" */ /* TODO load FPU register if needed depends on r0 exret */
    "ldmia  r1!, {r4-r11}             \n" /* restore other registers */
    "msr    psp, r1                   \n" /* restore PSP register (user mode SP)*/
    "bx     r0                        \n" /* load exception return value to PC causes end of exception*/
    /* {r0-r3,r12,LR,PC,xPSR} are restored automatically on exception return */
    );
}
