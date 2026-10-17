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
#include <stdint.h>

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
 * @brief     Disable the given IRQ
 * @param[in] irq_no IRQ number to disable
 */
static inline void rp_irq_disable(uint32_t irq_no)
{
    NVIC_DisableIRQ(irq_no);
}

/**
 * @brief   Called upon the end of an ISR
 */
static inline void rp_end_isr(void)
{
    cortexm_isr_end();
}

/**
 * @brief Initialize the architecture-specific components
 */
static inline void rp_arch_init(void)
{
    cortexm_init();
}

/**
 * @brief Wait For Interrupt abstraction
 */
static inline void rp_block_core(void)
{
    __WFI();
}

/**
 * @brief Set Event abstraction
 */
static inline void rp_unblock_core(void)
{
    __SEV();
}

/**
 * @brief   Get a pointer to the CPU specific interrupt vector table
 * @return  Pointer to the CPU specific interrupt vector table
 */
static inline uint32_t* rp_get_vector_poiner(void)
{
    return &_isr_vectors;
}

#ifdef __cplusplus
}
#endif

/** @} */
