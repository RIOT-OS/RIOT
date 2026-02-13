/*
 * SPDX-FileCopyrightText: 2015-2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_slstk3301a
 * @{
 *
 * @file
 * @brief       Board specific definitions for the SLSTK3301A starter kit
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/adc.h"
#include "periph/gpio.h"
#include "periph/spi.h"

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
#if IS_ACTIVE(CONFIG_EFM32_XTIMER_USE_LETIMER)
#  define CONFIG_ZTIMER_USEC_DEV            (TIMER_DEV(1))
#  define CONFIG_ZTIMER_USEC_BASE_FREQ      (32768UL)
#  define CONFIG_ZTIMER_USEC_WIDTH          (16)
#  define CONFIG_ZTIMER_USEC_ADJUST_SET     (0)
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP   (0)
#else
#  define CONFIG_ZTIMER_USEC_DEV            (TIMER_DEV(0))
#  define CONFIG_ZTIMER_USEC_BASE_FREQ      (250000UL)
#  define CONFIG_ZTIMER_USEC_WIDTH          (16)
#  define CONFIG_ZTIMER_USEC_ADJUST_SET     (0)
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP   (0)
#endif
/** @} */

/**
 * @name    Board controller configuration
 *
 * Define the GPIO pin to enable the BC, to allow serial communication
 * via the USB port.
 * @{
 */
#define BC_PIN              GPIO_PIN(PA, 8)
/** @} */

/**
 * @name    Push button pin definitions
 * @{
 */
#define PB0_PIN             GPIO_PIN(PD, 5)
#define PB1_PIN             GPIO_PIN(PC, 9)
/** @} */

/**
 * @name    LED pin definitions
 * @{
 */
#define LED0_PIN            GPIO_PIN(PD, 2)
#define LED1_PIN            GPIO_PIN(PC, 2)
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

/**
 * @name    Core temperature sensor configuration
 *
 * Connection to the on-chip temperature sensor.
 * @{
 */
#define CORETEMP_ADC        ADC_LINE(0)
/** @} */

/**
 * @name    Temperature sensor configuration
 *
 * Connection to the on-board temperature/humidity sensor (Si7021).
 * @{
 */
#define SI7021_I2C              I2C_DEV(0)
#define SI7021_EN_PIN           GPIO_PIN(PC, 12)

#define SI70XX_PARAM_I2C_DEV    SI7021_I2C
/** @} */

/**
 * @name    Hall-effect sensor configuration
 *
 * Connection to the on-board hall-effect sensor (Si7210).
 * @{
 */
#define SI7210_I2C              I2C_DEV(0)
#define SI7210_EN_PIN           GPIO_PIN(PC, 12)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
