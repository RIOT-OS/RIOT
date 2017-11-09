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

#ifndef DBGPIN_CPU_H
#define DBGPIN_CPU_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DBGPIN0
#define DBGPIN0_PORT        ((GPIO_TypeDef *)(DBGPIN0 & ~(0x0f)))
#define DBGPIN0_MASK        (1 << (DBGPIN0 & 0xf))
#define DBGPIN0_SET         (DBGPIN0_PORT->BSRR = DBGPIN0_MASK)
#define DBGPIN0_CLR         (DBGPIN0_PORT->BSRR = (DBGPIN0_MASK << 16))
#define DBGPIN0_TGL         (DBGPIN0_PORT->ODR ^= DBGPIN0_MASK)
#endif

#ifdef DBGPIN1
#define DBGPIN1_PORT        ((GPIO_TypeDef *)(DBGPIN1 & ~(0x0f)))
#define DBGPIN1_MASK        (1 << (DBGPIN1 & 0xf))
#define DBGPIN1_SET         (DBGPIN1_PORT->BSRR = DBGPIN1_MASK)
#define DBGPIN1_CLR         (DBGPIN1_PORT->BSRR = (DBGPIN1_MASK << 16))
#define DBGPIN1_TGL         (DBGPIN1_PORT->ODR ^= DBGPIN1_MASK)
#endif

#ifdef __cplusplus
}
#endif

#endif /* DBGPIN_CPU_H */
/** @} */
