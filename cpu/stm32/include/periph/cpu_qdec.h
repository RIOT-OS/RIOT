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
 * @brief           QDEC CPU specific definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#ifndef PERIPH_CPU_QDEC_H
#define PERIPH_CPU_QDEC_H

#include <stdint.h>

#include "cpu.h"
#include "periph/cpu_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   All STM QDEC timers have 2 capture channels
 */
#define QDEC_CHAN           (2U)

/**
 * @brief   QDEC channel
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin mapped to this channel */
    uint8_t cc_chan;        /**< capture compare channel used */
} qdec_chan_t;

/**
 * @brief   QDEC configuration
 */
typedef struct {
    TIM_TypeDef *dev;               /**< Timer used */
    uint32_t max;                   /**< Maximum counter value */
    uint32_t rcc_mask;              /**< bit in clock enable register */
    qdec_chan_t chan[QDEC_CHAN];    /**< channel mapping, set to {GPIO_UNDEF, 0}
                                     *   if not used */
#ifndef CPU_FAM_STM32F1
    gpio_af_t af;                   /**< alternate function used */
#endif
    uint8_t bus;                    /**< APB bus */
    uint8_t irqn;                   /**< global IRQ channel */
} qdec_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_QDEC_H */
/** @} */
