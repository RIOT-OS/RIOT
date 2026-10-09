/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_rpx0xx
 * @{
 *
 * @file
 * @brief           Compatibility layer with the RP2350 for code shared by both CPUs
 *
 * @author          Christopher Büchse
 */

#include <stdint.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     Enable the given IRQ
 * @param[in] irq_no IRQ number to enable
 */
static inline void rp_irq_enable(uint32_t irq_no)
{
    NVIC_EnableIRQ(irq_no);
}

/**
 * @brief   Called upon the end of an ISR
 */
static inline void rp_end_isr(void)
{
    cortexm_isr_end();
}

#ifdef __cplusplus
}
#endif

/** @} */
