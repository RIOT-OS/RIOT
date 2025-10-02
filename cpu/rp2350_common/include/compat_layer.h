/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_rp2350
 * @{
 *
 * @file
 * @brief           Compatibility layer for different CPU architectures
 *
 * @author          Tom Hert <git@annsann.eu>
 */


#include "periph_cpu.h"

#ifdef RP2350_USE_RISCV
#  include "xh3irq.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     Enable the given IRQ
 * @param[in] irq_no IRQ number to enable
 */
static inline void rp_irq_enable(uint32_t irq_no)
{
#ifdef RP2350_USE_RISCV
    xh3irq_enable_irq(irq_no);
#else
    NVIC_EnableIRQ(irq_no);
#endif
}

/**
 * @brief   Called upon the end of an ISR
 */
static inline void rp_end_isr(void)
{
#ifdef RP2350_USE_ARM
    cortexm_isr_end();
#endif
}

#ifdef __cplusplus
}
#endif

/** @} */
