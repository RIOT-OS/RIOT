/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef ARM_CPU_H
#define ARM_CPU_H

#include <stdint.h>
#include "VIC.h"
#include "arm7_common.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define NEW_TASK_CPSR 0x1F
#define WORDSIZE 32

uint32_t get_system_speed(void);
void cpu_clock_scale(uint32_t source, uint32_t target, uint32_t *prescale);

void arm_reset(void);

/**
 * @brief Interrupt stack canary value
 *
 * @note 0xeafffffe is the ARM machine code equivalent of asm("b #0") or
 * 'while (1);', i.e. an infinite loop.
 * @internal
 */
#define STACK_CANARY_WORD   (0xEAFFFFFEu)

/**
 * @brief   Select fastest bitarithm_lsb implementation
 * @{
 */
#ifdef __ARM_FEATURE_CLZ
#define BITARITHM_LSB_BUILTIN
#define BITARITHM_HAS_CLZ
#else
#define BITARITHM_LSB_LOOKUP
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ARM_CPU_H */
