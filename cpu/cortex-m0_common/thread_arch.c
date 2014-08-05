/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cortexm0_common
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
 * @name Initial program status register value for a newly created thread
 */
#define INITIAL_XPSR                (0x01000000)

/**
 * @name ARM Cortex-M specific exception return value, that triggers the return to the task mode
 *       stack pointer
 */
#define EXCEPT_RET_TASK_MODE        (0xfffffffd)


static void context_save(void);
static void context_restore(void);

/**
 * THe cortex-M0 knows stacks and handles register backups, so use the following layout:
 *
 * ----------------------------------------------------------------------------------------------------
 * | LR | R8 | R9 | R10 | R11 | R4 | R5 | R6 | R7 | R0 | R1 | R2 | R3 | R12 | LR | PC | xPSR | MARKER |
 * ----------------------------------------------------------------------------------------------------
 *   |                                                                                           |
 * lowest address                                                                    highest address
 *
 */
char *thread_arch_stack_init(void *(*task_func)(void *),
                             void *arg,
                             void *stack_start,
                             int stack_size)
{
    uint32_t *stk;
    stk = (uint32_t *)((uint32_t)stack_start + stack_size);

    /* marker */
    stk--;
    *stk = (uint32_t)STACK_MARKER;

    /* FIXME xPSR */
    stk--;
    *stk = (uint32_t)INITIAL_XPSR;

    /* program counter */
    stk--;
    *stk = (uint32_t)task_func;

    /* link register, jumped to when thread exits */
    stk--;
    *stk = (uint32_t)sched_task_exit;

    /* r12 */
    stk--;
    *stk = (uint32_t)0;

    /* r3 - r1 */
    for (int i = 3; i >= 1; i--) {
        stk--;
        *stk = i;
    }

    /* r0 -> thread function parameter */
    stk--;
    *stk = (uint32_t)arg;

    /* r7 - r4 */
    for (int i = 7; i >= 4; i--) {
        stk--;
        *stk = i;
    }

    /* r11 - r8 */
    for (int i = 11; i >= 8; i--) {
        stk--;
        *stk = i;
    }

    /* lr means exception return code  */
     stk--;
    *stk = (uint32_t)EXCEPT_RET_TASK_MODE;  /*return to task-mode main stack pointer */

    return (char*) stk;
}

void thread_arch_stack_print(void)
{
    int count = 0;
    uint32_t *sp = (uint32_t *)sched_active_thread->sp;

    printf("printing the current stack of thread %u\n", thread_getpid());
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
    /* enable IRQs to make sure the SVC interrupt can be triggered */
    enableIRQ();
    /* trigger the SVC interrupt that will schedule and load the next thread */
    asm("svc    0x01");

    UNREACHABLE();
}

void thread_arch_yield(void)
{
    /* trigger the PENDSV interrupt, which runs the scheduler */
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

/**
 * @brief save the current thread's context to the current thread's stack
 *
 * This function is always called in interrupt context. For this the initial state is the following:
 *
 * active stack-pointer: MSP
 *
 * top of application stack:
 * -------- highest address
 * | xPSR |
 * --------
 * | PC   |
 * --------
 * | LR   |
 * --------
 * | R12  |
 * --------
 * | R3   |
 * --------
 * | R2   |
 * --------
 * | R1   |
 * --------
 * | R0   | <- current value of PSP
 * -------- lowest address
 *
 * With other words, registers R0-R3, R12, LR, PC and xPSR are already saved to the thread's stack.
 * This leaves registers R4-R11 to be pushed to the thread's stack for a complete context save.
 *
 * This function now further pushes the remaining registers to the application stack (PSP)
 */
__attribute__((always_inline)) static __INLINE void context_save(void)
{
    /* {r0-r3,r12,LR,PC,xPSR} were saved automatically on exception entry */

    /* set stack pointer to PSP while keeping the MSP value */
    asm("mrs    r0, psp");
    asm("mov    r12, sp");
    asm("mov    sp, r0");
    /* save registers R11-R4 */
    asm("mov    r0, r8");
    asm("mov    r1, r9");
    asm("mov    r2, r10");
    asm("mov    r3, r11");
    asm("push   {r0-r7}");
    /* save link register */
    asm("mov    r0, lr");
    asm("push   {r0}");
    /* switch back stack pointers */
    asm("mov    r0, sp");
    asm("mov    sp, r12");
    /* store the new psp to the tcb->sp */
    asm("ldr    r1, =sched_active_thread"   );
    asm("ldr    r1, [r1]");
    asm("str    r0, [r1]");
}

__attribute__((always_inline)) static __INLINE void context_restore(void)
{
    /* save MSR stack pointer for later restore */
    asm("mov    lr, sp");
    /* get the PSP stack pointer of the current thread */
    asm("ldr    r0, =sched_active_thread");
    asm("ldr    r0, [r0]");
    asm("ldr    r0, [r0]");
    asm("mov    sp, r0");
    /* restore exception return value (LR) from stack */
    asm("pop    {r0}");
    asm("mov    r12, r0");
    /* restore registers R4-R11 from the PSP stack */
    asm("pop    {r0-r7}");
    asm("mov    r8, r0");
    asm("mov    r9, r1");
    asm("mov    r10, r2");
    asm("mov    r11, r3");
    /* restore the application mode stack pointer PSP */
    asm("mov    r0, sp");
    asm("msr    psp, r0");
    asm("mov    sp, lr");
    /* return from exception mode to application mode */
    asm("bx     r12");

    /* {r0-r3,r12,LR,PC,xPSR} are restored automatically on exception return */
}

/**
 * @brief The SVC interrupt is used for dispatching a thread if no context exists.
 *
 * Starting a thread from non-existing context is needed in two situations:
 * 1) after system initialization for running the main thread
 * 2) after exiting from a thread
 */
__attribute__((naked)) void isr_svc(void)
{
    sched_run();
    context_restore();
}

/**
 * @brief The PENDSV interrupt is used for context switching
 *
 * This interrupt saves the context, runs the scheduler and restores the context of the thread
 * that is run next.
 */
__attribute__((naked)) void isr_pendsv(void)
{
    context_save();
    sched_run();
    context_restore();
}
