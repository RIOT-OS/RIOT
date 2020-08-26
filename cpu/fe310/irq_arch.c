/*
 * Copyright (C) 2017, 2019 Ken Rabold, JP Bonn
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
 * @brief       Implementation of the CPU IRQ management for SiFive FE310
 *
 * @author      Ken Rabold
 * @}
 */

#include <stdio.h>
#include <assert.h>
#include <inttypes.h>

#include "macros/xtstr.h"
#include "cpu.h"
#include "context_frame.h"
#include "irq.h"
#include "irq_arch.h"
#include "panic.h"
#include "sched.h"

#include "vendor/encoding.h"
#include "vendor/platform.h"
#include "vendor/plic_driver.h"

/* Default state of mstatus register */
#define MSTATUS_DEFAULT     (MSTATUS_MPP | MSTATUS_MPIE)

volatile int fe310_in_isr = 0;

/* PLIC external ISR function list */
static external_isr_ptr_t _ext_isrs[PLIC_NUM_INTERRUPTS];

/**
 * @brief   ISR trap vector
 */
static void trap_entry(void);

/**
 * @brief   Timer ISR
 */
void timer_isr(void);

void irq_init(void)
{
    /* Setup trap handler function */
    write_csr(mtvec, &trap_entry);

    /* Clear all interrupt enables */
    write_csr(mie, 0);

    /* Initial PLIC external interrupt controller */
    PLIC_init(PLIC_CTRL_ADDR, PLIC_NUM_INTERRUPTS, PLIC_NUM_PRIORITIES);

    /* Enable SW and external interrupts */
    set_csr(mie, MIP_MSIP);
    set_csr(mie, MIP_MEIP);

    /*  Set default state of mstatus */
    set_csr(mstatus, MSTATUS_DEFAULT);
}

/**
 * @brief   Set External ISR callback
 */
void set_external_isr_cb(int intNum, external_isr_ptr_t cbFunc)
{
    assert((intNum > 0) && (intNum < PLIC_NUM_INTERRUPTS));

    _ext_isrs[intNum] = cbFunc;
}

/**
 * @brief External interrupt handler
 */
void external_isr(void)
{
    uint32_t intNum = (uint32_t)PLIC_claim_interrupt();

    if ((intNum > 0) && (intNum < PLIC_NUM_INTERRUPTS) && (_ext_isrs[intNum] != NULL)) {
        _ext_isrs[intNum](intNum);
    }

    PLIC_complete_interrupt(intNum);
}

/**
 * @brief Global trap and interrupt handler
 */
static void handle_trap(unsigned int mcause, unsigned int mepc, unsigned int mtval)
{
#ifndef DEVELHELP
    (void) mepc;
    (void) mtval;
#endif
    /*  Tell RIOT to set sched_context_switch_request instead of
     *  calling thread_yield(). */
    fe310_in_isr = 1;

    /* Check for INT or TRAP */
    if ((mcause & MCAUSE_INT) == MCAUSE_INT) {
        /* Cause is an interrupt - determine type */
        switch (mcause & MCAUSE_CAUSE) {
            case IRQ_M_SOFT:
                /* Handle software interrupt - flag for context switch */
                sched_context_switch_request = 1;
                CLINT_REG(0) = 0;
                break;

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
    else {
#ifdef DEVELHELP
        printf("Unhandled trap:\n");
        printf("  mcause: 0x%08x\n", mcause);
        printf("  mepc:   0x%08x\n", mepc);
        printf("  mtval:  0x%08x\n", mtval);
#endif
        /* Unknown trap */
        core_panic(PANIC_GENERAL_ERROR, "Unhandled trap");
    }

    /* Check if context change was requested */
    if (sched_context_switch_request) {
        sched_run();
    }

    /* ISR done - no more changes to thread states */
    fe310_in_isr = 0;
}

/* Must be naked to allow for returning with mret
 * Aligned to 4-byte boundary as per RISC-V spec  */
static void __attribute((aligned(4))) __attribute__((naked)) trap_entry(void) {
    unsigned int mepc_val;
    __asm__ volatile (
    /* Save registers to stack */
    "addi sp, sp, -"XTSTR(CONTEXT_FRAME_SIZE)"         \n"

    "sw s0,  "XTSTR(s0_OFFSET)"(sp)                    \n"
    "sw s1,  "XTSTR(s1_OFFSET)"(sp)                    \n"
    "sw s2,  "XTSTR(s2_OFFSET)"(sp)                    \n"
    "sw s3,  "XTSTR(s3_OFFSET)"(sp)                    \n"
    "sw s4,  "XTSTR(s4_OFFSET)"(sp)                    \n"
    "sw s5,  "XTSTR(s5_OFFSET)"(sp)                    \n"
    "sw s6,  "XTSTR(s6_OFFSET)"(sp)                    \n"
    "sw s7,  "XTSTR(s7_OFFSET)"(sp)                    \n"
    "sw s8,  "XTSTR(s8_OFFSET)"(sp)                    \n"
    "sw s9,  "XTSTR(s9_OFFSET)"(sp)                    \n"
    "sw s10, "XTSTR(s10_OFFSET)"(sp)                   \n"
    "sw s11, "XTSTR(s11_OFFSET)"(sp)                   \n"
    "sw ra,  "XTSTR(ra_OFFSET)"(sp)                    \n"
    "sw t0,  "XTSTR(t0_OFFSET)"(sp)                    \n"
    "sw t1,  "XTSTR(t1_OFFSET)"(sp)                    \n"
    "sw t2,  "XTSTR(t2_OFFSET)"(sp)                    \n"
    "sw t3,  "XTSTR(t3_OFFSET)"(sp)                    \n"
    "sw t4,  "XTSTR(t4_OFFSET)"(sp)                    \n"
    "sw t5,  "XTSTR(t5_OFFSET)"(sp)                    \n"
    "sw t6,  "XTSTR(t6_OFFSET)"(sp)                    \n"
    "sw a0,  "XTSTR(a0_OFFSET)"(sp)                    \n"
    "sw a1,  "XTSTR(a1_OFFSET)"(sp)                    \n"
    "sw a2,  "XTSTR(a2_OFFSET)"(sp)                    \n"
    "sw a3,  "XTSTR(a3_OFFSET)"(sp)                    \n"
    "sw a4,  "XTSTR(a4_OFFSET)"(sp)                    \n"
    "sw a5,  "XTSTR(a5_OFFSET)"(sp)                    \n"
    "sw a6,  "XTSTR(a6_OFFSET)"(sp)                    \n"
    "sw a7,  "XTSTR(a7_OFFSET)"(sp)                    \n"


    /* Get the interrupt PC */
    "csrr %[mepc_reg], mepc                            \n"

    /* Save return PC in stack frame */
    "sw %[mepc_reg], "XTSTR(pc_OFFSET)"(sp)            \n"

    /*  Get the active thread (could be NULL) */
    "lw tp, sched_active_thread                        \n"
    /* Skips the stack pointer save if no active thread */
    "beqz tp, null_thread                              \n"

    /* Save stack pointer of current thread */
    "sw sp, "XTSTR(SP_OFFSET_IN_THREAD)"(tp)           \n"

    "null_thread:                                      \n"
    /* Switch to ISR stack.  Interrupts are not nested so use fixed
     *  starting address and just abandon stack when finished. */
    "la  sp, _sp                                       \n"
    : [mepc_reg] "=r" (mepc_val)
    :
    :
    );

    /* Give the compiler some freedom to inline this */
    handle_trap(read_csr(mcause), mepc_val, read_csr(mtval));

    __asm__ volatile (
    /*  Get the active thread (guaranteed to be non NULL) */
    "lw tp, sched_active_thread                        \n"

    /*  Load the thread SP of scheduled thread */
    "lw sp, "XTSTR(SP_OFFSET_IN_THREAD)"(tp)           \n"

    /*  Set return PC */
    "lw a1, "XTSTR(pc_OFFSET)"(sp)                     \n"
    "csrw mepc, a1                                     \n"

    /* Restore registers from stack */
    "lw s0,  "XTSTR(s0_OFFSET)"(sp)                    \n"
    "lw s1,  "XTSTR(s1_OFFSET)"(sp)                    \n"
    "lw s2,  "XTSTR(s2_OFFSET)"(sp)                    \n"
    "lw s3,  "XTSTR(s3_OFFSET)"(sp)                    \n"
    "lw s4,  "XTSTR(s4_OFFSET)"(sp)                    \n"
    "lw s5,  "XTSTR(s5_OFFSET)"(sp)                    \n"
    "lw s6,  "XTSTR(s6_OFFSET)"(sp)                    \n"
    "lw s7,  "XTSTR(s7_OFFSET)"(sp)                    \n"
    "lw s8,  "XTSTR(s8_OFFSET)"(sp)                    \n"
    "lw s9,  "XTSTR(s9_OFFSET)"(sp)                    \n"
    "lw s10, "XTSTR(s10_OFFSET)"(sp)                   \n"
    "lw s11, "XTSTR(s11_OFFSET)"(sp)                   \n"
    "lw ra,  "XTSTR(ra_OFFSET)"(sp)                    \n"
    "lw t0,  "XTSTR(t0_OFFSET)"(sp)                    \n"
    "lw t1,  "XTSTR(t1_OFFSET)"(sp)                    \n"
    "lw t2,  "XTSTR(t2_OFFSET)"(sp)                    \n"
    "lw t3,  "XTSTR(t3_OFFSET)"(sp)                    \n"
    "lw t4,  "XTSTR(t4_OFFSET)"(sp)                    \n"
    "lw t5,  "XTSTR(t5_OFFSET)"(sp)                    \n"
    "lw t6,  "XTSTR(t6_OFFSET)"(sp)                    \n"
    "lw a0,  "XTSTR(a0_OFFSET)"(sp)                    \n"
    "lw a1,  "XTSTR(a1_OFFSET)"(sp)                    \n"
    "lw a2,  "XTSTR(a2_OFFSET)"(sp)                    \n"
    "lw a3,  "XTSTR(a3_OFFSET)"(sp)                    \n"
    "lw a4,  "XTSTR(a4_OFFSET)"(sp)                    \n"
    "lw a5,  "XTSTR(a5_OFFSET)"(sp)                    \n"
    "lw a6,  "XTSTR(a6_OFFSET)"(sp)                    \n"
    "lw a7,  "XTSTR(a7_OFFSET)"(sp)                    \n"

    "addi sp, sp, "XTSTR(CONTEXT_FRAME_SIZE)"          \n"
    "mret                                              \n"
    :
    :
    :
    );
}
