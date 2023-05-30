/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           Timer CPU specific definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#ifndef PERIPH_CPU_TIMER_H
#define PERIPH_CPU_TIMER_H

#include <stdint.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   All STM timers have at most 4 capture-compare channels
 */
#define TIMER_CHANNEL_NUMOF (4U)

/**
 * @brief   The driver provides a relative set function
 */
#define PERIPH_TIMER_PROVIDES_SET

/**
 * @brief   Define a macro for accessing a timer channel
 */
#define TIM_CHAN(tim, chan) *(&dev(tim)->CCR1 + chan)

/**
 * @brief   Timer configuration
 */
typedef struct {
    TIM_TypeDef *dev;       /**< timer device */
    uint32_t max;           /**< maximum value to count to (16/32 bit) */
    uint32_t rcc_mask;      /**< corresponding bit in the RCC register */
    uint8_t bus;            /**< APBx bus the timer is clock from */
    uint8_t irqn;           /**< global IRQ channel */
    uint8_t channel_numof;  /**< number of channels, 0 is alias for
                                 @ref TIMER_CHANNEL_NUMOF */
} timer_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_TIMER_H */
/** @} */
