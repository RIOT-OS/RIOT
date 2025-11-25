/*
 * Copyright (C) 2017, 2019 Ken Rabold, JP Bonn
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_riscv_common
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the CPU thread management for RISC-V
 *
 * @author      Ken Rabold
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "irq.h"
#include "thread.h"
#include "sched.h"
#include "context_frame.h"
#include "architecture.h"

/**
 * @brief   Noticeable marker marking the beginning of a stack segment
 *
 * This marker is used e.g. by *thread_start_threading* to identify the
 * stacks beginning.
 */
#define STACK_MARKER                (0x77777777)

/**
 * @brief Initialize a thread's stack
 *
 * RIOT saves the tasks registers on the stack, not in the task control
 * block.  thread_stack_init() is responsible for allocating space for
 * the registers on the stack and adjusting the stack pointer to account for
 * the saved registers.
 *
 * The stack_start parameter is the bottom of the stack (low address).  The
 * return value is the top of stack: stack_start + stack_size - space reserved
 * for thread context save - space reserved to align stack.
 *
 * thread_stack_init is called for each thread.
 *
 * RISCV ABI is here: https://github.com/riscv/riscv-elf-psabi-doc
 * From ABI:
 * The stack grows downwards and the stack pointer shall be aligned to a
 * 128-bit boundary upon procedure entry, except for the RV32E ABI, where it
 * need only be aligned to 32 bits. In the standard ABI, the stack pointer
 * must remain aligned throughout procedure execution. Non-standard ABI code
 * must realign the stack pointer prior to invoking standard ABI procedures.
 * The operating system must realign the stack pointer prior to invoking a
 * signal handler; hence, POSIX signal handlers need not realign the stack
 * pointer. In systems that service interrupts using the interruptee's stack,
 * the interrupt service routine must realign the stack pointer if linked
 * with any code that uses a non-standard stack-alignment discipline, but
 * need not realign the stack pointer if all code adheres to the standard ABI.
 *
 * @param[in] task_func     pointer to the thread's code
 * @param[in] arg           argument to task_func
 * @param[in] stack_start   pointer to the start address of the thread
 * @param[in] stack_size    the maximum size of the stack
 *
 * @return                  pointer to the new top of the stack (128bit aligned)
 *
 */
char *thread_stack_init(thread_task_func_t task_func,
                        void *arg,
                        void *stack_start,
                        int stack_size)
{
    struct context_switch_frame *sf;
    uint32_t *stk_top;

    /* calculate the top of the stack */
    stk_top = (uint32_t *)((uintptr_t)stack_start + stack_size);

    /* Put a marker at the top of the stack.  This is used by
     * thread_stack_print to determine where to stop dumping the
     * stack.
     */
    stk_top--;
    *stk_top = STACK_MARKER;

    /* per ABI align stack pointer to 16 byte boundary. */
    stk_top = (uint32_t *)(((uintptr_t)stk_top) & ~((uintptr_t)0xf));

    /* reserve space for the stack frame. */
    stk_top = (uint32_t *)((uintptr_t)stk_top - sizeof(*sf));

    /* populate the stack frame with default values for starting the thread. */
    sf = (struct context_switch_frame *)stk_top;

    /* Clear stack frame */
    memset(sf, 0, sizeof(*sf));

    /* set initial reg values */
    sf->pc = (uword_t)task_func;
    sf->a0 = (uword_t)arg;

    /* if the thread exits go to sched_task_exit() */
    sf->ra = (uword_t)sched_task_exit;

    return (char *)stk_top;
}

void thread_print_stack(void)
{
    int count = 0;
    thread_t *active_thread = thread_get_active();

    if (!active_thread) {
        return;
    }

    /* thread_init aligns stack pointer to 16 byte boundary, the cast below
     * is therefore safe. Use intermediate cast to uintptr_t to silence
     * -Wcast-align */
    uint32_t *sp = (uint32_t *)(uintptr_t)active_thread->sp;

    printf("printing the current stack of thread %" PRIkernel_pid "\n",
           thread_getpid());

#ifdef DEVELHELP
    printf("thread name: %s\n", active_thread->name);
    printf("stack start: 0x%08x\n", (unsigned)(active_thread->stack_start));
    printf("stack end  : 0x%08x\n",
           (unsigned)(active_thread->stack_start + active_thread->stack_size));
#endif

    printf("  address:      data:\n");

    do {
        printf("  0x%08x:   0x%08x\n", (unsigned)sp, (unsigned)*sp);
        sp++;
        count++;
    } while (*sp != STACK_MARKER);

    printf("current stack size: %i words\n", count);
}

int thread_isr_stack_usage(void)
{
    return 0;
}

void *thread_isr_stack_pointer(void)
{
    return NULL;
}

void *thread_isr_stack_start(void)
{
    return NULL;
}

/**
 * @brief Call context switching at thread exit
 *
 * This is called is two situations: 1) after the initial main and idle threads
 * have been created and 2) when a thread exits.
 *
 */
void cpu_switch_context_exit(void)
{
    /* enable interrupts */
    irq_enable();

    /* force a context switch to another thread */
    thread_yield_higher();
    UNREACHABLE();
}

/**
 * @brief Print heap statistics
 */
void heap_stats(void)
{
    extern char _sheap;     /* defined in linker script */
    extern char _eheap;     /* defined in linker script */

    /* cppcheck-suppress comparePointers */
    long int heap_size = &_eheap - &_sheap;
    struct mallinfo minfo = mallinfo();

    printf("heap: %ld (used %u, free %ld) [bytes]\n",
           heap_size, minfo.uordblks, heap_size - minfo.uordblks);
}
