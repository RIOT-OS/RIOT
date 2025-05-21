/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_cc26xx_cc13xx_definitions
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx WDT register definitions
 */


#include <cc26xx_cc13xx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * WDT registers
 */
typedef struct {
    reg32_t LOAD; /**< config */
    reg32_t VALUE; /**< current count value */
    reg32_t CTL; /**< control */
    reg32_t ICR; /**< interrupt clear */
    reg32_t RIS; /**< raw interrupt status */
    reg32_t MIS; /**< masked interrupt status */
    reg32_t __reserved1[0x100]; /**< meh */
    reg32_t TEST; /**< test mode */
    reg32_t INT_CAUS; /**< interrupt cause test mode */
    reg32_t __reserved2[0x1f9]; /**< meh */
    reg32_t LOCK; /**< lock */
} wdt_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define WDT_BASE      0x40080000 /**< WDT base address */
/** @} */

#define WDT           ((wdt_regs_t *) (WDT_BASE)) /**< WDT register bank */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
