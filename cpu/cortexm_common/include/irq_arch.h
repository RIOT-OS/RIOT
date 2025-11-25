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

#include <stdbool.h>
#include <stdint.h>
#include "cpu_conf.h"
#include "kernel_defines.h"
#include "debug_irq_disable.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Start SysTick timer to measure time spent with IRQ disabled
 */
static inline void _irq_debug_start_count(void)
{
    SysTick->VAL  = 0;
    SysTick->LOAD = SysTick_LOAD_RELOAD_Msk;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}

/**
 * @brief   Stop SysTick timer, return time spent with IRQ disabled
 */
static inline uint32_t _irq_debug_stop_count(void)
{
    uint32_t ticks = SysTick_LOAD_RELOAD_Msk - SysTick->VAL;
    SysTick->CTRL = 0;
    return ticks;
}

/**
 * @brief Disable all maskable interrupts
 */
static inline __attribute__((always_inline))
unsigned int irq_disable(void)
{
    uint32_t mask = __get_PRIMASK();

    if ((mask == 0) && IS_USED(MODULE_DEBUG_IRQ_DISABLE)) {
        _irq_debug_start_count();
    }

    __disable_irq();
    return mask;
}

/**
 * @brief Enable all maskable interrupts
 */
static inline __attribute__((always_inline)) __attribute__((used))
unsigned int irq_enable(void)
{
    unsigned result = __get_PRIMASK();

    __enable_irq();
    return result;
}

/**
 * @brief Restore the state of the IRQ flags
 */
static inline __attribute__((always_inline))
#if !IS_USED(MODULE_DEBUG_IRQ_DISABLE)
void irq_restore(unsigned int state)
{
    __set_PRIMASK(state);
}
#else
void _irq_restore(unsigned int state, const char *file, unsigned line)
{
    uint32_t ticks = 0;

    if (state == 0) {
        ticks = _irq_debug_stop_count();
    }

    __set_PRIMASK(state);

    if (ticks) {
        debug_irq_disable_print(file, line, ticks);
    }
}
#define irq_restore(state) _irq_restore(state, __FILE__, __LINE__);
#endif /* MODULE_DEBUG_IRQ_DISABLE */

/**
 * @brief See if IRQs are currently enabled
 */
static inline __attribute__((always_inline))
bool irq_is_enabled(void)
{
    /* so far, all existing Cortex-M are only using the least significant bit
     * in the PRIMARK register. If ever any other bit is used for different
     * purposes, this function will not work properly anymore. */
    return (__get_PRIMASK() == 0);
}

/**
 * @brief See if the current context is inside an ISR
 */
static inline __attribute__((always_inline))
bool irq_is_in(void)
{
    return (__get_IPSR() & 0xFF);
}

#ifdef __cplusplus
}
#endif

#endif /* IRQ_ARCH_H */
/** @} */
