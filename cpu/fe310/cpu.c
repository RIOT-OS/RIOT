/*
 * Copyright (C) 2017 Ken Rabold, JP Bonn
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the CPU initialization for SiFive FE310
 *
 * @author      Ken Rabold
 * @}
 */

#include <stdio.h>
#include <errno.h>

#include "thread.h"
#include "irq.h"
#include "sched.h"
#include "thread.h"
#include "irq.h"
#include "cpu.h"
#include "context_frame.h"
#include "periph_cpu.h"
#include "panic.h"
#include "vendor/encoding.h"
#include "vendor/platform.h"
#include "vendor/plic_driver.h"

/* Default state of mstatus register */
#define MSTATUS_DEFAULT     (MSTATUS_MPP | MSTATUS_MPIE)

volatile int __in_isr = 0;

void trap_entry(void);
void thread_start(void);

/* PLIC external ISR function list */
static external_isr_ptr_t _ext_isrs[PLIC_NUM_INTERRUPTS];

/* NULL interrupt handler */
void null_isr(int num)
{
    (void) num;
}

/**
 * @brief Initialize the CPU, set IRQ priorities, clocks
 */
void cpu_init(void)
{
    volatile uint64_t *mtimecmp =
        (uint64_t *) (CLINT_CTRL_ADDR + CLINT_MTIMECMP);

    /* Setup trap handler function */
    write_csr(mtvec, &trap_entry);

    /* Enable FPU if present */
    if (read_csr(misa) & (1 << ('F' - 'A'))) {
        write_csr(mstatus, MSTATUS_FS); /* allow FPU instructions without trapping */
        write_csr(fcsr, 0);             /* initialize rounding mode, undefined at reset */
    }

    /* Clear all interrupt enables */
    write_csr(mie, 0);

    /* Initial PLIC external interrupt controller */
    PLIC_init(PLIC_CTRL_ADDR, PLIC_NUM_INTERRUPTS, PLIC_NUM_PRIORITIES);

    /* Initialize ISR function list */
    for (int i = 0; i < PLIC_NUM_INTERRUPTS; i++) {
        _ext_isrs[i] = null_isr;
    }

    /* Set mtimecmp to largest value to avoid spurious timer interrupts */
    *mtimecmp = 0xFFFFFFFFFFFFFFFF;

    /* Enable SW, timer and external interrupts */
    set_csr(mie, MIP_MSIP);
    set_csr(mie, MIP_MTIP);
    set_csr(mie, MIP_MEIP);

    /*  Set default state of mstatus */
    set_csr(mstatus, MSTATUS_DEFAULT);
}

/**
 * @brief Enable all maskable interrupts
 */
unsigned int irq_enable(void)
{
    /* Enable all interrupts */
    set_csr(mstatus, MSTATUS_MIE);
    return read_csr(mstatus);
}

/**
 * @brief Disable all maskable interrupts
 */
unsigned int irq_disable(void)
{
    unsigned int state = read_csr(mstatus);

    /* Disable all interrupts */
    clear_csr(mstatus, MSTATUS_MIE);
    return state;
}

/**
 * @brief Restore the state of the IRQ flags
 */
void irq_restore(unsigned int state)
{
    /* Restore all interrupts to given state */
    write_csr(mstatus, state);
}

/**
 * @brief See if the current context is inside an ISR
 */
int irq_is_in(void)
{
    return __in_isr;
}

/**
 * @brief   Set External ISR callback
 */
void set_external_isr_cb(int intNum, external_isr_ptr_t cbFunc)
{
    if ((intNum > 0) && (intNum < PLIC_NUM_INTERRUPTS)) {
        _ext_isrs[intNum] = cbFunc;
    }
}

/**
 * @brief External interrupt handler
 */
void external_isr(void)
{
    plic_source intNum = PLIC_claim_interrupt();

    if ((intNum > 0) && (intNum < PLIC_NUM_INTERRUPTS)) {
        _ext_isrs[intNum]((uint32_t) intNum);
    }

    PLIC_complete_interrupt(intNum);
}

/**
 * @brief Global trap and interrupt handler
 */
void handle_trap(unsigned int mcause)
{
    /*  Tell RIOT to set sched_context_switch_request instead of
     *  calling thread_yield(). */
    __in_isr = 1;

    /* Check for INT or TRAP */
    if ((mcause & MCAUSE_INT) == MCAUSE_INT) {
        /* Cause is an interrupt - determine type */
        switch (mcause & MCAUSE_CAUSE) {
#ifdef MODULE_PERIPH_TIMER
            case IRQ_M_TIMER:
                /* Handle timer interrupt */
                timer_isr();
                break;
#endif
            case IRQ_M_EXT:
                /* Handle external interrupt */
                external_isr();
                break;

            default:
                /* Unknown interrupt */
                core_panic(PANIC_GENERAL_ERROR, "Unhandled interrupt");
                break;
        }
    }

    /* ISR done - no more changes to thread states */
    __in_isr = 0;
}

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
    uint32_t *reg;
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
    stk_top = (uint32_t *)(((uint32_t)stk_top) & ~((uint32_t)0xf));

    /* reserve space for the stack frame. */
    stk_top = (uint32_t *)((uint8_t *) stk_top - sizeof(*sf));

    /* populate the stack frame with default values for starting the thread. */
    sf = (struct context_switch_frame *) stk_top;

    /* a7 is register with highest memory address in frame */
    reg = &sf->a7;
    while (reg != &sf->pc) {
        *reg-- = 0;
    }
    sf->pc = (uint32_t) task_func;
    sf->a0 = (uint32_t) arg;

    /* if the thread exits go to sched_task_exit() */
    sf->ra = (uint32_t) sched_task_exit;

    return (char *) stk_top;
}

void thread_print_stack(void)
{
    int count = 0;
    uint32_t *sp = (uint32_t *) sched_active_thread->sp;

    printf("printing the current stack of thread %" PRIkernel_pid "\n",
           thread_getpid());

#ifdef DEVELHELP
    printf("thread name: %s\n", sched_active_thread->name);
    printf("stack start: 0x%08x\n", (unsigned int)(sched_active_thread->stack_start));
    printf("stack end  : 0x%08x\n", (unsigned int)(sched_active_thread->stack_start + sched_active_thread->stack_size));
#endif

    printf("  address:      data:\n");

    do {
        printf("  0x%08x:   0x%08x\n", (unsigned int) sp, (unsigned int) *sp);
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
 * @brief Start or resume threading by loading a threads initial information
 * from the stack.
 *
 * This is called is two situations: 1) after the initial main and idle threads
 * have been created and 2) when a thread exits.
 *
 * sched_active_thread is not valid when cpu_switch_context_exit() is
 * called.  sched_run() must be called to determine the next valid thread.
 * This is exploited in the context switch code.
 */
void cpu_switch_context_exit(void)
{
    /* enable interrupts */
    irq_enable();

    /* start the thread */
    thread_yield();
    UNREACHABLE();
}

void thread_yield_higher(void)
{
    /* Use SW intr to schedule context switch */
    CLINT_REG(CLINT_MSIP) = 1;
}
