/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include <stdio.h>

#include "panic.h"
#include "cpu_conf.h"

/**
 * @ingroup     cpu_riscv_common
 * @{
 *
 * @file
 * @brief       xh3irq.h interrupt controller support
 *
 * @author      Tom Hert <git@annsann.eu>
 */

#ifdef __cplusplus
extern "C" {
#endif

/** CPU specific interrupt vector table
 * @see 3.2 Interrupts and IRQn_Type in RP2350.h
 */
extern const void *vector_cpu[CPU_IRQ_NUMOF];

/**
 * Hazard3 has internal registers to individually filter which
 * external IRQs appear in meip. When meip is 1,
 * this indicates there is at least one external interrupt
 * which is asserted (hence pending in mieipa), enabled in meiea,
 * and of priority greater than or equal to the current
 * preemption level in meicontext.preempt.
 */
#define MEIP_OFFSET 11
/** Mask to extract the MEIP bit from MIP/MISE */
#define MEIP_MASK 0x1
/**
 * Index of the highest-priority active external interrupt.
 * Zero when no external interrupts with sufficient priority
 * are both pending and enabled
 */
#define MEINEXT_IRQ_OFFSET 2
/** Mask to extract the IRQ number from MEINEXT */
#define MEINEXT_MASK 0x1FF
/** XH3IRQ Interrupt arrays have a window to access the IRQ bits */
#define INTERRUPT_ARRAY_MASK_OFFSET 16

/**
 * @brief  Check if there are any pending interrupts
 * @return 1 if there are pending interrupts, 0 otherwise
 */
uint32_t xh3irq_has_pending(void);

/**
 * @brief  The main IRQ handler, called from the assembly IRQ handler
 * @note   This function must clear the pending interrupt in the interrupt controller
 */
void xh3irq_handler(void);

/**
 * @brief  Enable the given IRQ number
 * @param  irq_no The IRQ number to enable
 */
void xh3irq_enable_irq(uint32_t irq_no);

/**
 * @brief  Disable the given IRQ number
 * @param  irq_no The IRQ number to disable
 */
void xh3irq_disable_irq(uint32_t irq_no);

/**
 * @brief  Force the given IRQ number to be pending
 * @param  irq_no The IRQ number to force
 * @note   The IRQ still must be enabled to be handled
 */
void xh3irq_force_irq(uint32_t irq_no);

#ifdef __cplusplus
}
#endif

/** @} */
