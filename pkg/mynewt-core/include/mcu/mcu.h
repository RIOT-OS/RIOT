/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     pkg_mynewt_core
 * @{
 *
 * @file
 * @brief       Abstraction layer for RIOT adaption
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set nrf5x radio ISR callback
 *
 * @param[in]   irqn    IRQ number
 * @param[in]   addr    the ISR callback
 */
void nrf5x_hw_set_isr(int irqn, void (*addr)(void));

/**
 * @name    Entering and exiting critical section defines
 * @{
 */
#define __HAL_DISABLE_INTERRUPTS(x)                     \
    do {                                                \
        x = irq_disable();                              \
    } while (0);

#define __HAL_ENABLE_INTERRUPTS(x)                      \
    do {                                                \
        if (x) {                                        \
            irq_restore(x);                             \
        }                                               \
        else {                                          \
            irq_enable();                              \
        }                                               \
    } while (0);
/** @} */

#ifdef __cplusplus
}
#endif
