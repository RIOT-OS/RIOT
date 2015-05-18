/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm3_common
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
char *thread_arch_stack_init(thread_task_func_t task_func, void *arg, void *stack_start, int stack_size)
{
    uint32_t *stk;
    stk = (uint32_t *)((uintptr_t)stack_start + stack_size);

    /* adjust to 32 bit boundary by clearing the last two bits in the address */
    stk = (uint32_t *)(((uint32_t)stk) & ~((uint32_t)0x3));

    /* Stack start marker */
    stk--;
    *stk = STACK_MARKER;

    /* Make sure the stack is double word aligned (8 bytes) */
    /* This is required in order to conform with Procedure Call Standard for the
     * ARM® Architecture (AAPCS) */
    /* http://infocenter.arm.com/help/topic/com.arm.doc.ihi0042e/IHI0042E_aapcs.pdf */
    if (((uint32_t) stk % 8) != 0) {
        /* add a single word padding */
        --stk;
        *stk = ~((uint32_t)STACK_MARKER);
    }

    /* ****************************** */
    /* Automatically popped registers */
    /* ****************************** */

    /* The following eight stacked registers are popped by the hardware upon
     * return from exception. (bx instruction in context_restore) */

    /* xPSR */
    stk--;
    /* Setting bit 9 (0x200) of xPSR will cause the initial stack pointer for
     * the process to be aligned on a 32-bit, non-64-bit, boundary. Don't do that. */
    /* Default xPSR, only the Thumb mode-bit is set */
    *stk = 0x01000000;

    /* pc */
    stk--;
    /* initial program counter */
    *stk = (uint32_t) task_func;

    /* lr */
    stk--;
    /* link register, return address when a thread exits.  */
    *stk = (uint32_t) sched_task_exit;

    /* r12 */
    stk--;
    *stk = 0;

    /* r1 - r3 */
    for (int i = 3; i >= 1; i--) {
        stk--;
        *stk = i;
    }

    /* r0 */
    stk--;
    /* thread function parameter */
    *stk = (uint32_t) arg;

    /* 8 hardware-handled registers in total */

    /* ************************* */
    /* Manually popped registers */
    /* ************************* */

    /* The following registers are not handled by hardware in return from
     * exception, but manually by context_restore. */

    /* r11 - r4 */
    for (int i = 11; i >= 4; i--) {
        stk--;
        *stk = i;
    }

    /* exception return code  */
    stk--;
    *stk = EXCEPT_RET_TASK_MODE; /* return to task-mode process stack pointer */

    /* 9 manually handled registers in total. */

    /* The returned stack pointer will be aligned on a 32 bit boundary not on a
     * 64 bit boundary because of the odd number of registers above (8+9).
     * This is not a problem since the initial stack pointer upon process entry
     * _will_ be 64 bit aligned (because of the cleared bit 9 in the stacked
     * xPSR and aligned stacking of the hardware-handled registers). */

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
__attribute__((naked)) void isr_svc(void)
{
    sched_run();
    context_restore();
}

/**
 * @brief All task switching activity is carried out int the pendSV interrupt
 */
__attribute__((naked)) void isr_pendsv(void)
{
    context_save();
    sched_run();
    context_restore();
}
