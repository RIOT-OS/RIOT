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

uint32_t xh3irq_has_pending(void);
void xh3irq_handler(void);

void xh3irq_enable_irq(uint32_t irq_no);

void xh3irq_disable_irq(uint32_t irq_no);

void xh3irq_force_irq(uint32_t irq_no);

#ifdef __cplusplus
}
#endif

/** @} */
