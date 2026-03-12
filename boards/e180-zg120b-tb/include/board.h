/*
 * SPDX-FileCopyrightText: 2015-2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_e180-zg120b-tb
 * @{
 *
 * @file
 * @brief       Board specific definitions for the E180-ZG120B-TB starter kit
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "periph/timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ztimer configuration
 *
 * The timer runs at 250 kHz to increase accuracy, or at 32.768 kHz if
 * LETIMER is used.
 * @{
 */
#if IS_ACTIVE(CONFIG_EFM32_ZTIMER_USE_LETIMER)
#  define CONFIG_ZTIMER_USEC_DEV            (TIMER_DEV(1))  /**< Timer peripheral for ztimer */
#  define CONFIG_ZTIMER_USEC_BASE_FREQ      (32768UL)       /**< Running at 32.768 kHz */
#  define CONFIG_ZTIMER_USEC_WIDTH          (16)            /**< Running on a 16-bit timer */
#  define CONFIG_ZTIMER_USEC_ADJUST_SET     (37)            /**< Overhead for ztimer_set */
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP   (37)            /**< Overhead for ztimer_sleep */
#else
#  define CONFIG_ZTIMER_USEC_DEV            (TIMER_DEV(0))  /**< Timer peripheral for ztimer */
#  define CONFIG_ZTIMER_USEC_BASE_FREQ      (250000UL)      /**< Running at 250 kHz */
#  define CONFIG_ZTIMER_USEC_WIDTH          (16)            /**< Running on a 16-bit timer */
#  define CONFIG_ZTIMER_USEC_ADJUST_SET     (24)            /**< Overhead for ztimer_set */
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP   (24)            /**< Overhead for ztimer_sleep */
#endif
/** @} */

/**
 * @name    Push button pin definitions
 * @{
 */
#define PB0_PIN             GPIO_PIN(PD, 15)
#define PB1_PIN             GPIO_PIN(PD, 13)
#define PB2_PIN             GPIO_PIN(PB, 11)
/** @} */

/**
 * @name    LED pin definitions
 * @{
 */
#define LED0_PIN            GPIO_PIN(PF, 2)
#define LED1_PIN            GPIO_PIN(PF, 3)
/** @} */

/**
 * @name    Core temperature sensor configuration
 *
 * Connection to the on-chip temperature sensor.
 * @{
 */
#define CORETEMP_ADC        ADC_LINE(0)
/** @} */

/**
 * @name    Macros for controlling the on-board LEDs
 * @{
 */
#define LED0_ON             gpio_set(LED0_PIN)
#define LED0_OFF            gpio_clear(LED0_PIN)
#define LED0_TOGGLE         gpio_toggle(LED0_PIN)
#define LED1_ON             gpio_set(LED1_PIN)
#define LED1_OFF            gpio_clear(LED1_PIN)
#define LED1_TOGGLE         gpio_toggle(LED1_PIN)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
