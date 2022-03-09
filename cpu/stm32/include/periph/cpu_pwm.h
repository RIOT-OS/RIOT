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
 * @brief           PWM CPU specific definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#ifndef PERIPH_CPU_PWM_H
#define PERIPH_CPU_PWM_H

#include <stdint.h>

#include "cpu.h"
#include "periph/cpu_gpio.h"
#include "periph/cpu_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   PWM channel
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin mapped to this channel */
    uint8_t cc_chan;        /**< capture compare channel used */
} pwm_chan_t;

/**
 * @brief   PWM configuration
 */
typedef struct {
    TIM_TypeDef *dev;               /**< Timer used */
    uint32_t rcc_mask;              /**< bit in clock enable register */
    pwm_chan_t chan[TIMER_CHANNEL_NUMOF]; /**< channel mapping
                                           *   set to {GPIO_UNDEF, 0}
                                           *   if not used */
    gpio_af_t af;                   /**< alternate function used */
    uint8_t bus;                    /**< APB bus */
} pwm_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_PWM_H */
/** @} */
