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

#ifndef CPU_H
#define CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialization of the CPU
 */
void cpu_init(void);

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
 * @brief   Print the last instruction's address
 *
 * @todo:   Not supported
 */
static inline void cpu_print_last_instruction(void)
{
    /* This function must exist else RIOT won't compile */
}

#ifdef __cplusplus
}
#endif

#endif /* CPU_H */
/** @} */
