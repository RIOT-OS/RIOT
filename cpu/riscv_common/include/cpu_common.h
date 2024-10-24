/*
 * Copyright (C) 2020, Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_riscv_common RISC-V common
 * @ingroup     cpu
 * @brief       Common implementations and headers for the RISC-V CPU
 * @{
 *
 * @file
 * @brief       Basic definitions for the RISC-V CPU module
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include <stdint.h>

#include "irq_arch.h"

#ifndef CPU_COMMON_H
#define CPU_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize rv32i specific core parts of the CPU
 *
 * Initialized the interrupt controller and the enables the FPU if present
 */
void riscv_init(void);

/**
 * @brief   Enable the rv32i FPU when present
 */
void riscv_fpu_init(void);

/**
 * @brief   Initialization of the interrupt controller
 */
void riscv_irq_init(void);

/**
 * @brief   Gets the last instruction's address
 *
 * @todo:   Not supported
 */
static inline uintptr_t cpu_get_caller_pc(void)
{
    /* This function must exist else RIOT won't compile */
    return 0;
}

/**
 * @brief   Convenience function to set bit flags in a register
 *
 * @param   reg     register to set bits in
 * @param   mask    bits to set in the register
 */
static inline __attribute__((always_inline))
void cpu_reg_enable_bits(volatile uint32_t *reg, uint32_t mask)
{
    __atomic_fetch_or(reg, mask, __ATOMIC_RELAXED);
}

/**
 * @brief   Convenience function to clear bit flags in a register
 *
 * @param   reg     register to clear bits in
 * @param   mask    bits to clear in the register
 */
static inline __attribute__((always_inline))
void cpu_reg_disable_bits(volatile uint32_t *reg, uint32_t mask)
{
    __atomic_fetch_and(reg, ~mask, __ATOMIC_RELAXED);
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CPU_COMMON_H */
