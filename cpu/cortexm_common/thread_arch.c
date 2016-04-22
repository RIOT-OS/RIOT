/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernel's architecture dependent thread
 *              interface
 *
 * Members of the Cortex-M family know stacks and are able to handle register
 * backups partly, so we make use of that.
 *
 * Cortex-M3 and Cortex-M4 use the
 * following register layout when saving their context onto the stack:
 *
 * -------- highest address (bottom of stack)
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
 * | R0   | <- the registers from xPSR to R0 are handled by hardware
 * --------
 * | R11  |
 * --------
 * | R10  |
 * --------
 * | R9   |
 * --------
 * | R8   |
 * --------
 * | R7   |
 * --------
 * | R6   |
 * --------
 * | R5   |
 * --------
 * | R4   |
 * --------
 * | RET  | <- exception return code
 * -------- lowest address (top of stack)
 *
 * For the Cortex-M0 and Cortex-M0plus we use a slightly different layout by
 * switching the blocks R11-R8 and R7-R4. This allows more efficient code when
 * saving/restoring the context:
 *
 * ------------- highest address (bottom of stack)
 * | xPSR - R0 | <- same as for Cortex-M3/4
 * -------------
 * | R7   |
 * --------
 * | R6   |
 * --------
 * | R5   |
 * --------
 * | R4   |
 * --------
 * | R11  |
 * --------
 * | R10  |
 * --------
 * | R9   |
 * --------
 * | R8   |
 * --------
 * | RET  | <- exception return code
 * -------- lowest address (top of stack)
 *
 * TODO: Implement handling of FPU registers for Cortex-M4 CPUs
 *
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdio.h>

#include "arch/thread_arch.h"
#include "sched.h"
#include "thread.h"
#include "irq.h"
#include "cpu.h"

/**
 * @brief   Noticeable marker marking the beginning of a stack segment
 *
 * This marker is used e.g. by *thread_arch_start_threading* to identify the
 * stacks beginning.
 */
#define STACK_MARKER                (0x77777777)

/**
 * @brief   Initial program status register value for a newly created thread
 *
 * In the initial state, only the Thumb mode-bit is set
 */
#define INITIAL_XPSR                (0x01000000)

/**
 * @brief   ARM Cortex-M specific exception return value, that triggers the
 *          return to the task mode stack pointer
 */
#define EXCEPT_RET_TASK_MODE        (0xfffffffd)

char *thread_arch_stack_init(thread_task_func_t task_func,
                             void *arg,
                             void *stack_start,
                             int stack_size)
{
    uint32_t *stk;
    stk = (uint32_t *)((uintptr_t)stack_start + stack_size);

    /* adjust to 32 bit boundary by clearing the last two bits in the address */
    stk = (uint32_t *)(((uint32_t)stk) & ~((uint32_t)0x3));

    /* stack start marker */
    stk--;
    *stk = STACK_MARKER;

    /* make sure the stack is double word aligned (8 bytes) */
    /* This is required in order to conform with Procedure Call Standard for the
     * ARM® Architecture (AAPCS) */
    /* http://infocenter.arm.com/help/topic/com.arm.doc.ihi0042e/IHI0042E_aapcs.pdf */
    if (((uint32_t) stk & 0x7) != 0) {
        /* add a single word padding */
        --stk;
        *stk = ~((uint32_t)STACK_MARKER);
    }

#ifdef CPU_ARCH_CORTEX_M4F
    /* TODO: fix FPU handling for Cortex-M4f */
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
#endif

    /* ****************************** */
    /* Automatically popped registers */
    /* ****************************** */

    /* The following eight stacked registers are popped by the hardware upon
     * return from exception. (bx instruction in context_restore) */

    /* xPSR - initial status register */
    stk--;
    *stk = (uint32_t)INITIAL_XPSR;
    /* pc - initial program counter value := thread entry function */
    stk--;
    *stk = (uint32_t)task_func;
    /* lr - contains the return address when the thread exits */
    stk--;
    *stk = (uint32_t)sched_task_exit;
    /* r12 */
    stk--;
    *stk = 0;
    /* r3 - r1 */
    for (int i = 3; i >= 1; i--) {
        stk--;
        *stk = i;
    }
    /* r0 - contains the thread function parameter */
    stk--;
    *stk = (uint32_t)arg;

    /* ************************* */
    /* Manually popped registers */
    /* ************************* */

    /* The following registers are not handled by hardware in return from
     * exception, but manually by context_restore.
     * For the Cortex-M0(plus) we write registers R11-R4 in two groups to allow
     * for more efficient context save/restore code.
     * For the Cortex-M3 and Cortex-M4 we write them continuously onto the stack
     * as they can be read/written continuously by stack instructions. */

#if defined(CPU_ARCH_CORTEX_M0) || defined(CPU_ARCH_CORTEX_M0PLUS)
    /* start with r7 - r4 */
    for (int i = 7; i >= 4; i--) {
        stk--;
        *stk = i;
    }
    /* and put r11 - r8 on top of them */
    for (int i = 11; i >= 8; i--) {
        stk--;
        *stk = i;
    }
#else
    /* r11 - r4 */
    for (int i = 11; i >= 4; i--) {
        stk--;
        *stk = i;
    }
#endif

    /* exception return code  - return to task-mode process stack pointer */
    stk--;
    *stk = (uint32_t)EXCEPT_RET_TASK_MODE;

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

    printf("printing the current stack of thread %" PRIkernel_pid "\n",
           thread_getpid());
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
    __asm__ volatile (
    "bl     irq_arch_enable               \n" /* enable IRQs to make the SVC
                                           * interrupt is reachable */
    "svc    #1                            \n" /* trigger the SVC interrupt */
    "unreachable%=:                       \n" /* this loop is unreachable */
    "b      unreachable%=                 \n" /* loop indefinitely */
    :::);
}

void thread_arch_yield(void)
{
    /* trigger the PENDSV interrupt to run scheduler and schedule new thread if
     * applicable */
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

__attribute__((naked)) void arch_context_switch(void)
{
    __asm__ volatile (
    /* PendSV handler entry point */
    ".global isr_pendsv               \n"
    ".thumb_func                      \n"
    "isr_pendsv:                      \n"
    /* save context by pushing unsaved registers to the stack */
    /* {r0-r3,r12,LR,PC,xPSR} are saved automatically on exception entry */
    ".thumb_func                      \n"
    "context_save:"
    "mrs    r0, psp                   \n" /* get stack pointer from user mode */
#if defined(CPU_ARCH_CORTEX_M0) || defined(CPU_ARCH_CORTEX_M0PLUS)
    "mov    r12, sp                   \n" /* remember the exception SP */
    "mov    sp, r0                    \n" /* set user mode SP as active SP */
    /* we can not push high registers directly, so we move R11-R8 into
     * R4-R0, as these are already saved */
    "mov    r0, r8                    \n" /* move R11-R8 into R3-R0 */
    "mov    r1, r9                    \n"
    "mov    r2, r10                   \n"
    "mov    r3, r11                   \n"
    "push   {r0-r7}                   \n" /* now push them onto the stack */
    "mov    r0, lr                    \n" /* next we save the link register */
    "push   {r0}                      \n"
    "mov    r0, sp                    \n" /* switch back to the exception SP */
    "mov    sp, r12                   \n"
#else
    "stmdb  r0!,{r4-r11}              \n" /* save regs */
    "stmdb  r0!,{lr}                  \n" /* exception return value */
#ifdef CPU_ARCH_CORTEX_M4F
/*  "vstmdb sp!, {s16-s31}            \n" */ /* TODO save FPU registers */
#endif
#endif
    "ldr    r1, =sched_active_thread  \n" /* load address of current tcb */
    "ldr    r1, [r1]                  \n" /* dereference pdc */
    "str    r0, [r1]                  \n" /* write r0 to pdc->sp */
    /* SVC handler entry point */
    /* PendSV will continue from above and through this part as well */
    ".global isr_svc                  \n"
    ".thumb_func                      \n"
    "isr_svc:                         \n"
    /* perform scheduling */
    "bl     sched_run                 \n"
    /* restore context and return from exception */
    ".thumb_func                      \n"
    "context_restore:                 \n"
#if defined(CPU_ARCH_CORTEX_M0) || defined(CPU_ARCH_CORTEX_M0PLUS)
    "mov    lr, sp                    \n" /* save MSR stack pointer for later */
    "ldr    r0, =sched_active_thread  \n" /* load address of current TCB */
    "ldr    r0, [r0]                  \n" /* dereference TCB */
    "ldr    r0, [r0]                  \n" /* load tcb-sp to R0 */
    "mov    sp, r0                    \n" /* make user mode SP active SP */
    "pop    {r0}                      \n" /* restore LR from stack */
    "mov    r12, r0                   \n" /* remember LR by parking it in R12 */
    "pop    {r0-r7}                   \n" /* get R11-R8 and R7-R4 from stack */
    "mov    r8, r0                    \n" /* move R11-R8 to correct registers */
    "mov    r9, r1                    \n"
    "mov    r10, r2                   \n"
    "mov    r11, r3                   \n"
    /* restore the application mode stack pointer PSP */
    "mov    r0, sp                    \n" /* restore the user mode SP */
    "msr    psp, r0                   \n" /* for this write it to the PSP reg */
    "mov    sp, lr                    \n" /* and get the parked MSR SP back */
    /* return from exception mode to application mode */
    "bx     r12                       \n" /* return from exception mode */
#else
    "ldr    r0, =sched_active_thread  \n" /* load address of current TCB */
    "ldr    r0, [r0]                  \n" /* dereference TCB */
    "ldr    r1, [r0]                  \n" /* load tcb->sp to register 1 */
    "ldmia  r1!, {r0}                 \n" /* restore exception return value */
#ifdef CPU_ARCH_CORTEX_M4F
/*  "pop    {s16-s31}                 \n" */ /* TODO load FPU registers */
#endif
    "ldmia  r1!, {r4-r11}             \n" /* restore other registers */
    "msr    psp, r1                   \n" /* restore user mode SP to PSP reg */
    "bx     r0                        \n" /* load exception return value to PC,
                                           * causes end of exception*/
#endif
    /* {r0-r3,r12,LR,PC,xPSR} are restored automatically on exception return */
    );
}
