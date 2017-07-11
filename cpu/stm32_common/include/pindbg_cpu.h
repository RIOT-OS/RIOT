/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 *
 * @{
 * @file
 * @brief       CPU specific handlers for direct control of debug pins
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PINDBG_CPU_H
#define PINDBG_CPU_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PINDBG0
#define PINDBG0_PORT        ((GPIO_TypeDef *)(PINDBG0 & ~(0x0f)))
#define PINDBG0_MASK        (1 << (PINDBG0 & 0xf))
#define PINDBG0_SET         (PINDBG0_PORT->BSRR = PINDBG0_MASK)
#define PINDBG0_CLR         (PINDBG0_PORT->BSRR = (PINDBG0_MASK << 16))
#define PINDBG0_TGL         (PINDBG0_PORT->ODR ^= PINDBG0_MASK)
#endif

#ifdef PINDBG1
#define PINDBG1_PORT        ((GPIO_TypeDef *)(PINDBG1 & ~(0x0f)))
#define PINDBG1_MASK        (1 << (PINDBG1 & 0xf))
#define PINDBG1_SET         (PINDBG1_PORT->BSRR = PINDBG1_MASK)
#define PINDBG1_CLR         (PINDBG1_PORT->BSRR = (PINDBG1_MASK << 16))
#define PINDBG1_TGL         (PINDBG1_PORT->ODR ^= PINDBG1_MASK)
#endif

#ifdef __cplusplus
}
#endif

#endif /* PINDBG_CPU_H */
/** @} */
