/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortex-m3
 * @{
 *
 * @file        thread_arch.c
 * @brief       Implementation of the kernel's architecture dependent thread interface
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "arch/thread_arch.h"
#include "thread.h"
#include "sched.h"
#include "irq.h"
#include "cpu.h"
#include "kernel_internal.h"

//TODO: Hack to fix travis build failures for other boards
//Decide how to resolve this properly
// 22/10/14 - ryankurte
#ifndef SVC_ISR
#define SVC_ISR       isr_svc  
#endif

#ifndef PEND_SV_ISR
#define PEND_SV_ISR   isr_pendsv
#endif
//End hack

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

static void context_save(void);
static void context_restore(void) NORETURN;

/**
 * Cortex-M3 knows stacks and handles register backups, so use different stack frame layout
 *
 * Layout without floating point registers:
 * --------------------------------------
 * | R0 | R1 | R2 | R3 | LR | PC | xPSR |
 * --------------------------------------
 *
 */
char *thread_arch_stack_init(void *(*task_func)(void *), void *arg, void *stack_start, int stack_size)
{
    uint32_t *stk;
    stk = (uint32_t *)((uintptr_t)stack_start + stack_size);

    /* marker */
    stk--;
    *stk = (uint32_t)STACK_MARKER;

    /* FIXME xPSR */
    stk--;
    *stk = (uint32_t)0x01000200;

    /* program counter */
    stk--;
    *stk = (uint32_t)task_func;

    /* link register, jumped to when thread exits */
    stk--;
    *stk = (uint32_t)sched_task_exit;

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
    *stk = (unsigned int) arg;

    /* r11 - r4 */
    for (int i = 11; i >= 4; i--) {
        stk--;
        *stk = i;
    }

    /* put LR to trigger return to thread stack pointer */
    stk--;
    *stk = (uint32_t)EXCEPT_RET_TASK_MODE;

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

__attribute__((naked)) void thread_arch_start_threading(void)
{
    /* enable IRQs to make sure the SVC interrupt is reachable */
    enableIRQ();
    /* trigger the SVC interrupt which will get and execute the next thread */
    asm("svc    0x01");
}

void thread_arch_yield(void)
{
    /* trigger the PENDSV interrupt to run scheduler and schedule new thread if applicable */
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

__attribute__((always_inline)) static __INLINE void context_save(void)
{
    /* {r0-r3,r12,LR,PC,xPSR} are saved automatically on exception entry */

    /* save unsaved registers onto the stack */
    asm("mrs    r0, psp"                    );  /* get stack pointer from user mode */
    asm("stmdb  r0!,{r4-r11}"               );  /* save regs */
    asm("stmdb  r0!,{lr}"                   );  /* exception return value */
    asm("ldr    r1, =sched_active_thread"   );  /* load address of current TCB */
    asm("ldr    r1, [r1]"                   );  /* dereference TCB */
    asm("str    r0, [r1]"                   );  /* write r0 to tcb->sp */
}

__attribute__((always_inline)) static __INLINE void context_restore(void)
{
    /* restore registers from stack */
    asm("ldr    r0, =sched_active_thread"   );  /* load address of current TCB */
    asm("ldr    r0, [r0]"                   );  /* dereference TCB */
    asm("ldr    r1, [r0]"                   );  /* load tcb->sp to register 1 */
    asm("ldmia  r1!, {r0}"                  );  /* restore exception return value from stack */
    asm("ldmia  r1!, {r4-r11}"              );  /* restore other registers */
    asm("msr    psp, r1"                    );  /* restore PSP register (user mode SP) */
    asm("bx     r0"                         );  /* load exception return value to PC */

    /* {r0-r3,r12,LR,PC,xPSR} are restored automatically on exception return */

    UNREACHABLE();
}

/**
 * @brief The svc is used for running the scheduler and scheduling a new task during start-up or
 *        after a thread has exited
 */
__attribute__((naked)) void SVC_ISR(void)
{
    sched_run();
    context_restore();
}

/**
 * @brief All task switching activity is carried out int the pendSV interrupt
 */
__attribute__((naked)) void PEND_SV_ISR(void)
{
    context_save();
    sched_run();
    context_restore();
}
