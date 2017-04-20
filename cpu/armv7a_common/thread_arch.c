/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @author Dang Minh Phuong <kamejoko80@yahoo.com>
 */

/**
  * @ingroup armv7a stack frame
  * @{
  *
  * @file
  * @brief   Implementation of the kernel's architecture dependent thread
  *          interface
  *
  * Use the following register layout when saving context onto the stack:
  *
  * -------- highest address (bottom of stack)
  * | CPSR |
  * --------
  * | PC   |
  * --------
  * | LR   |
  * --------
  * | R12  |
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
  * | R3   |
  * --------
  * | R2   |
  * --------
  * | R1   |
  * --------
  * | R0   |
  * -------- lowest address (top of stack)
  *
  * @author  Dang Minh Phuong <kamejoko80@yahoo.com>
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
 * @brief Indicate start/end of IRQ stack
 */
extern uint32_t __stack_irq_start;
extern uint32_t __stack_irq_end;

/**
 * @brief Indicate nested interrupt dept
 */
volatile uint32_t nesting_level = 0UL;

/**
 * @brief Noticeable marker marking the beginning of a stack segment
 */
#define STACK_MARKER                (0xA5A5A5A5)

/**
 * @brief Initial program status register value for a newly created thread
 *
 * System mode, IRQ, FIQ enabled
 */
#define INITIAL_CPSR                (0x0000001F) /* System mode, ARM mode, IRQ enabled FIQ enabled */
#define THUMB_MODE_ADDRESS          (0x00000001)
#define THUMB_MODE_BIT              (0x00000020)

/**
 * @brief Initialize thread stack frame
 */
char *thread_stack_init(thread_task_func_t task_func, void *arg, void *stack_start, int stack_size)
{
    unsigned int *stk;

    stk = (unsigned int *)((uintptr_t)stack_start + stack_size);

    /* Stack marker */
    *stk = STACK_MARKER;
    stk--;

    /* Initial SPSR */
    *stk = (unsigned int) INITIAL_CPSR;
    stk--;

    /* PC */
    *stk = ((unsigned int) task_func);
    stk--;

    /* set the return address (LR) */
    *stk = (unsigned int) sched_task_exit;
    stk--;

    /* R12 */
    *stk = (unsigned int) 0x12121212;
    stk--;

    /* R11 */
    *stk = (unsigned int) 0x11111111;
    stk--;

    /* R10 */
    *stk = (unsigned int) 0x12121212;
    stk--;

    /* R9 */
    *stk = (unsigned int) 0x09090909;
    stk--;

    /* R8 */
    *stk = (unsigned int) 0x08080808;
    stk--;

    /* R7 */
    *stk = (unsigned int) 0x07070707;
    stk--;

    /* R6 */
    *stk = (unsigned int) 0x06060606;
    stk--;

    /* R5 */
    *stk = (unsigned int) 0x05050505;
    stk--;

    /* R4 */
    *stk = (unsigned int) 0x04040404;
    stk--;

    /* R3 */
    *stk = (unsigned int) 0x03030303;
    stk--;

    /* R2 */
    *stk = (unsigned int) 0x02020202;
    stk--;

    /* R1 */
    *stk = (unsigned int) 0x02020202;
    stk--;

    /* R0 */
    *stk = ((unsigned int) arg);

    return (char *)stk;
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

/* This function returns the number of bytes used on the ISR stack */
int thread_arch_isr_stack_usage(void)
{
    uint32_t *ptr = &__stack_irq_start;

    while(((*ptr) == STACK_CANARY_WORD) && (ptr < &__stack_irq_end)) {
        ++ptr;
    }

    ptrdiff_t num_used_words = &__stack_irq_end - ptr;
    return num_used_words * sizeof(*ptr);
}

static uint32_t __read_sp(void)
{
    uint32_t value;

    __asm__ __volatile__(
        "mov %0, sp"
        : "=r" (value)
        :
        : "memory");

    return value;
}

void *thread_arch_isr_stack_pointer(void)
{
    register uint32_t sp = __read_sp();
    return (void *)sp;
}

void *thread_arch_isr_stack_start(void)
{
    return (void *)&__stack_irq_start;
}

/**
 * @brief Start new thread
 */
__attribute__((naked)) void NORETURN thread_arch_start_threading(void)
{
    /* Run schedule to get new TCB pointer */
    sched_run();

    /* Load Context */
    __asm__ __volatile__(

        ".set SYS_MODE, 0x1f\n"
        ".set SVC_MODE, 0x13\n"
        ".set IRQ_MODE, 0x12\n"

        /* Change to system mode */
        "cps #SYS_MODE\n"

        /* Set the SP to point to the stack of the task being restored.    */
        "ldr r0, =sched_active_thread\n" /* r0 = &sched_active_thread      */
        "ldr r0, [r0]\n"                 /* r0 = *r0 = sched_active_thread */
        "ldr sp, [r0]\n"
        "pop {r0-r12, r14}\n"
        "rfeia sp!\n"
    );
}

/**
 * @brief Call SWI handler to switch context
 */
__attribute__ ((noinline)) void thread_arch_yield(void)
{
    /* Don't enable SWI when IRQ actived */
    if (nesting_level == 0)
    {
        __asm__("swi 0\n");
    }
    else /* Context switch pending */
    {
        sched_context_switch_request = 1;
    }
}

/**
 * @brief SWI handler function
 */
__attribute__((naked)) void NORETURN swi_handler(void)
{
    __asm__ __volatile__(

        ".set SYS_MODE,	0x1f\n"
        ".set SVC_MODE,	0x13\n"
        ".set IRQ_MODE,	0x12\n"

        /* SAVE CONTEXT */
        "srsdb sp!, #SYS_MODE\n"
        "cps #SYS_MODE\n"
        "push {r0-r12, r14}\n"
        "ldr r0, =sched_active_thread\n"
        "ldr r0, [r0]\n"
        "str sp, [r0]\n"

        /* Call the scheduler to switch TCB */
        "ldr r0, =sched_run\n"
        "blx r0\n"

        /* RESTORE CONTEXT */
        "ldr r0, =sched_active_thread\n"
        "ldr r0, [r0]\n"
        "ldr sp, [r0]\n"
        "pop {r0-r12, r14}\n"
        "rfeia sp!\n"
    );
}

/**
 * @brief IRQ handler function
 */
__attribute__((naked)) void NORETURN irq_handler(void)
{
    __asm__ __volatile__(

        ".set SYS_MODE, 0x1f\n"
        ".set SVC_MODE, 0x13\n"
        ".set IRQ_MODE, 0x12\n"

        /* Return to the interrupted instruction. */
        "sub lr, lr, #4\n"

        /* Push the return address and SPSR. */
        "push {lr}\n"
        "mrs lr, spsr\n"
        "push {lr}\n"

        /* Change to supervisor mode to allow reentry */
        "cps #SVC_MODE\n"

        /* Push used registers */
        "push {r0-r4, r12}\n"

        /* Increase IRQ nesting level */
        "ldr r0, =nesting_level\n"
        "ldr r1, [r0]\n"
        "add r1, r1, #1\n"
        "str r1, [r0]\n"

        /* Ensure stack is 8-byte aligned */
        "mov r2, sp\n"
        "and r2, r2, #4\n"
        "sub sp, sp, r2\n"

        /* Call the interrupt handler */
        "push {r0-r4, lr}\n"
        "ldr r1, =s5p4418_irq_handler\n"
        "blx r1\n"
        "pop {r0-r4, lr}\n"
        "add sp, sp, r2\n"

        /* Disable interrupt */
        "cpsid i\n"
        "dsb\n"
        "isb\n"

        /* Decrease IRQ nesting level */
        "ldr r0, =nesting_level\n"
        "ldr r1, [r0]\n"
        "sub r1, r1, #1\n"
        "str r1, [r0]\n"

        /* Don't switch context if nesting_level is not zero */
        "cmp r1, #0\n"
        "bne exit_without_switch\n"

        /* Check if RIOT kernel requires switch context */
        "ldr r1, =sched_context_switch_request\n"
        "ldr r0, [r1]\n"
        "cmp r0, #0\n"
        "bne switch_before_exit\n"

"exit_without_switch:\n"

        /* No context switch */
        "pop {r0-r4, r12}\n"
        "cps #IRQ_MODE\n"
        "pop {lr}\n"
        "msr spsr_cxsf, lr\n"
        "pop {lr}\n"
        "movs pc, lr\n"

"switch_before_exit:\n"

        /* Clear context switch pending flag */
        "mov r0, #0\n"
        "str r0, [r1]\n"

        /* Restore AAPCS registers */
        "pop {r0-r4, r12}\n"
        "cps #IRQ_MODE\n"
        "pop {lr}\n"
        "msr spsr_cxsf, lr\n"
        "pop {lr}\n"

       /* Save context */
        "srsdb sp!, #SYS_MODE\n"
        "cps #SYS_MODE\n"
        "push {r0-r12, r14}\n"
        "ldr r0, =sched_active_thread\n"
        "ldr r0, [r0]\n"
        "str sp, [r0]\n"

        /* Call the scheduler to switch TCB */
        "ldr r0, =sched_run\n"
        "blx r0\n"

        /* Restore context */
        "ldr r0, =sched_active_thread\n"
        "ldr r0, [r0]\n"
        "ldr sp, [r0]\n"
        "pop {r0-r12, r14}\n"
        "rfeia sp!\n"
    );
}
