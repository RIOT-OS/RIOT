/*
 * SPDX-FileCopyrightText: 2023 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_arm7tdmi_gba
 * @{
 */

#include <stdint.h>

#include "sched.h"
#include "cpu.h"
#include "irq_arch.h"
#include "kernel_defines.h"
#include "kernel_init.h"
#include "periph_gba.h"

/* Stub implementation used when timer are not used.
 * Actual implementation is in periph/timer.c */
#ifndef MODULE_PERIPH_TIMER
void gba_timer_isr(uint16_t args)
{
    (void)args;
}
#endif

/* Stub implementation used when uart is not used.
 * Actual implementation is in periph/uart.c */
#ifndef MODULE_PERIPH_UART
void gba_uart_isr(void)
{
}
#endif

/* Clear and reset all timers. On a hardboot, the regs always read zero.
 * But on on softboot, you never know! */
void gba_timer_init(void)
{
    GBA_TM0_CNT_L = 0;
    GBA_TM0_CNT_H = 0;
    GBA_TM1_CNT_L = 0;
    GBA_TM1_CNT_H = 0;
    GBA_TM2_CNT_L = 0;
    GBA_TM2_CNT_H = 0;
    GBA_TM3_CNT_L = 0;
    GBA_TM3_CNT_H = 0;

    if (IS_USED(MODULE_PERIPH_TIMER)) {
        GBA_IE |= GBA_IE_TIMERS; /* enable timer interrupt */
    }
    else {
        GBA_IE &= ~GBA_IE_TIMERS; /* disable timer interrupt */
    }
}

/* Configure GBA Com port into UART */
void gba_uart_init(void)
{
    /* Disable Joy bus and GPIO mode */
    GBA_SIO_RCNT &= ~(1 << 15);
    /* Select UART mode */
    GBA_SIO_SCNT |= (0b11 << 12);

    if (IS_USED(MODULE_PERIPH_UART)) {
        GBA_IE |= GBA_IE_UART; /* enable uart interrupt */
    }
    else {
        GBA_IE &= ~GBA_IE_UART; /* disable uart interrupt */
    }
}

static void gba_irq_handler(void)
{
    /* pull the interrupt flag register into a cpu register */
    uint16_t IF = GBA_IF;

    if (IF & GBA_IF_TIMERS) {
        gba_timer_isr(IF & GBA_IF_TIMERS);
    }
    if (IF & GBA_IF_UART) {
        gba_uart_isr();
    }

    /* Clear all pending interrupts */
    GBA_IF = GBA_IF_CLEAR_ALL;
}

static __attribute((naked)) void gba_irq_trap(void)
{
    __asm__ volatile (
        /* Restore regs from what the bios did */
        "ldmfd sp!,{r0,r1,r2,r3,r12,lr}\n"

        /* adjust return address, as by CPU spec */
        "sub lr, lr, #4\n"

        /* save interrupted tasks PC onto stack */
        "stmfd sp!, {lr}\n"

        /* save all registers on isr stack */
        "stmfd sp!, {r0-r12}\n"

        /* save spsr on stack */
        "mrs r0, spsr\n"
        "stmfd sp!, {r0}\n"

        /* ?? */
        "mrs r1, cpsr\n"
        "msr spsr, r1\n"
    );

    /* jump to irq handler */
    gba_irq_handler();

    __asm__ volatile (
        /* restore spsr from stack */
        "ldmfd sp!, {r0}\n"
        "msr spsr, r0\n"
    );

    if (sched_context_switch_request == 0) {
        __asm__ volatile (
            /* recover general purpose registers */
            "ldmfd sp!, {r0-r12}\n"

            /* recover tasks PC into lr */
            "ldmfd sp!, {lr}\n"

            /* now jump back into task, the 's' is important to switch the cpu state */
            "movs pc, lr\n" /* does not return */
        );
    } else {
        __asm__ volatile (
            /* recover general purpose registers */
            "ldmfd  sp!, {r0-r12}\n"
            "b ctx_switch_from_irq\n" /* does not return */
        );
    }
}

static inline void gba_irq_init(void)
{
    irq_disable();
    /* Ensure all maskable interrupts are disabled, clean state. */
    GBA_IE = 0;

    /* Clear all pending interrupts */
    GBA_IF = GBA_IF_CLEAR_ALL;

    GBA_PTR_TO_IRQ_HANDLER = (uint32_t) gba_irq_trap;

    /* Ensure master interrupt is enabled.
     * Otherwise the "msr cpsr" isr enable won't have an effect. */
    GBA_IME = 1;
    irq_enable();
}

/**
 * @brief Initialize the CPU
 */
void cpu_init(void)
{
    gba_irq_init();
    gba_uart_init();
    early_init(); /* brings up stdio early, so debug printing can be used. */
    gba_timer_init();
}
