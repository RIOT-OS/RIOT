/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include <stdio.h>
#include "cpu_conf.h"
#include "panic.h"

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
