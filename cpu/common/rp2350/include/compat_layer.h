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
 * @brief           Compatibility layer for different CPU architectures and
 *                  families
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include "periph_cpu.h"
#include <stdint.h>

#ifdef RP2350_USE_RISCV
#   include "xh3irq.h"
    /** See RP2350 Datasheet 3.8.6.3.1 */
#   define __h3_block() __asm__("slt x0, x0, x0")
    /** See RP2350 Datasheet 3.8.6.3.2 */
#   define __h3_unblock() __asm__("slt x0, x0, x1")
#else
    /** Defined in the linker script */
    extern uint32_t _isr_vectors;
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
 * @brief     Disable the given IRQ
 * @param[in] irq_no IRQ number to disable
 */
static inline void rp_irq_disable(uint32_t irq_no)
{
#ifdef RP2350_USE_RISCV
    xh3irq_disable_irq(irq_no);
#else
    NVIC_DisableIRQ(irq_no);
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

/**
 * @brief Initialize the architecture-specific components
 */
static inline void rp_arch_init(void)
{
#ifdef RP2350_USE_RISCV
    riscv_init();
#else
    cortexm_init();
#endif
}

/**
 * @brief Wait For Interrupt abstraction
 */
static inline void rp_block_core(void)
{
#ifdef RP2350_USE_RISCV
    __h3_block();
#else
    __WFI();
#endif
}

/**
 * @brief Set Event abstraction
 */
static inline void rp_unblock_core(void)
{
#ifdef RP2350_USE_RISCV
    __h3_unblock();
#else
    __SEV();
#endif
}

/**
 * @brief   Get a pointer to the CPU specific interrupt vector table
 * @return  Pointer to the CPU specific interrupt vector table
 */
static inline uint32_t* rp_get_vector_poiner(void)
{
#ifdef RP2350_USE_RISCV
    return (uint32_t*)vector_cpu;
#else
    return &_isr_vectors;
#endif
}

/**
 * @name    Helper functions with the names used by the RP2040 port, for code
 *          shared by both CPUs
 * @{
 */
/**
 * @brief   Atomically set the bits of @p mask in the register @p reg
 */
static inline void io_reg_atomic_set(volatile uint32_t *reg, uint32_t mask)
{
    atomic_set(reg, mask);
}

/**
 * @brief   Atomically clear the bits of @p mask in the register @p reg
 */
static inline void io_reg_atomic_clear(volatile uint32_t *reg, uint32_t mask)
{
    atomic_clear(reg, mask);
}

/**
 * @brief   Put the hardware components in @p components into reset
 */
static inline void periph_reset(uint32_t components)
{
    atomic_set(&RESETS->RESET, components);
}

/**
 * @brief   Take the hardware components in @p components out of reset and
 *          wait until the reset is done
 */
static inline void periph_reset_done(uint32_t components)
{
    reset_component(components, components);
}
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
