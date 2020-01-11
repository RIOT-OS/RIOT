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

#include "cpu.h"
#include "irq.h"
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
void trap_entry(void);

/**
 * @brief   Timer ISR
 */
void timer_isr(void);

void irq_init(void)
{
    volatile uint64_t *mtimecmp =
        (uint64_t *) (CLINT_CTRL_ADDR + CLINT_MTIMECMP);

    /* Setup trap handler function */
    write_csr(mtvec, &trap_entry);

    /* Clear all interrupt enables */
    write_csr(mie, 0);

    /* Initial PLIC external interrupt controller */
    PLIC_init(PLIC_CTRL_ADDR, PLIC_NUM_INTERRUPTS, PLIC_NUM_PRIORITIES);

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
    return fe310_in_isr;
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
void handle_trap(unsigned int mcause, unsigned int mepc, unsigned int mtval)
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
