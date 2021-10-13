/*
 * Copyright (C) 2014-2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels irq interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef IRQ_ARCH_H
#define IRQ_ARCH_H

#include <stdint.h>
#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Disable all maskable interrupts
 */
static inline __attribute__((always_inline)) unsigned int irq_disable(void)
{
    uint32_t mask = __get_PRIMASK();

    __disable_irq();
    return mask;
}

/**
 * @brief Enable all maskable interrupts
 */
static inline __attribute__((always_inline)) __attribute__((used)) unsigned int
irq_enable(void)
{
    unsigned result = __get_PRIMASK();

    __enable_irq();
    return result;
}

/**
 * @brief Restore the state of the IRQ flags
 */
static inline __attribute__((always_inline)) void irq_restore(
    unsigned int state)
{
    __set_PRIMASK(state);
}

/**
 * @brief See if IRQs are currently enabled
 */
static inline __attribute__((always_inline)) int irq_is_enabled(void)
{
    /* so far, all existing Cortex-M are only using the least significant bit
     * in the PRIMARK register. If ever any other bit is used for different
     * purposes, this function will not work properly anymore. */
    return (__get_PRIMASK() == 0);
}

/**
 * @brief See if the current context is inside an ISR
 */
static inline __attribute__((always_inline)) int irq_is_in(void)
{
    return (__get_IPSR() & 0xFF);
}

#ifdef __cplusplus
}
#endif

#endif /* IRQ_ARCH_H */
/** @} */
