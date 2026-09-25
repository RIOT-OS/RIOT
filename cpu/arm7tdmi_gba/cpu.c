/*
 * SPDX-FileCopyrightText: 2023 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_arm7tdmi_gba
 * @{
 */

#include <stdint.h>

#include "cpu.h"
#include "irq_arch.h"
#include "kernel_init.h"
#include "periph_gba.h"

/* Surprisingly, this does not need special handling of function envelope */
/* No need to write pure asm either. */
/* This is because the GBA comes with a BIOS that pre-handles the IRQ! */
static void _gba_irq_handler(void)
{
    /* pull the interrupt flag register into a cpu register */
    uint16_t IF = GBA_IF;

    if (IF & GBA_IF_TIMERS) {
        gba_timer_isr(IF & GBA_IF_TIMERS);
    }

    /* Clear all pending interrupts */
    GBA_IF = 0x3FFF;
}

void gba_irq_init(void)
{
    irq_disable();
    /* Ensure all maskable interrupts are disabled, clean state */
    GBA_IE = 0;

    /* Clear all pending interrupts */
    GBA_IF = 0x3FFF;

    GBA_PTR_TO_IRQ_HANDLER = (uint32_t)_gba_irq_handler;

    /* Ensure master interrupt is enabled, otherwise the "msr cpsr" isr enable won't have an effect */
    GBA_IME = 1;
    irq_enable();
}

#include "periph/uart.h"

/**
 * @brief Initialize the CPU, set IRQ priorities, clocks
 */
void cpu_init(void)
{
    gba_irq_init();
    early_init(); /* brings up stdio early, so debug printing can be used */
    gba_timer_init();
}
