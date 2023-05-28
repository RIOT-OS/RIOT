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
 *
 * When using a general-purpose timer for a PWM device, the outputs OC<n> of
 * each of the four capture/compare channels can be used as PWM channel.
 * The respective capture/compare channel is then specified with 0...3 in
 * `cc_chan` for the outputs OC1...OC4.
 *
 * Advanced timers like TIM1 and TIM8 have additionally three complementary
 * outputs OC<n>N of the capture/compare channels, which can also be used
 * as PWM channels. These complementary outputs are defined with an offset
 * of 4, i.e. they are specified in `cc_chan` with 4...6 for OC1N...OC3N.
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin mapped to this channel */
    uint8_t cc_chan;        /**< Capture/compare channel used: 0..3 for OC1..OC4
                                 or 4..6 for OC1N..OC3N for advanced timers */
} pwm_chan_t;

/**
 * @brief   PWM configuration
 */
typedef struct {
    TIM_TypeDef *dev;               /**< Timer used */
    uint32_t rcc_mask;              /**< bit in clock enable register */
#ifdef CPU_FAM_STM32F1
    uint32_t remap;                 /**< AFIO remap mask to route periph to other
                                         pins (or zero, if not needed) */
#endif
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
