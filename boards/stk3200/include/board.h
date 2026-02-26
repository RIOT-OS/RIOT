/*
 * SPDX-FileCopyrightText: 2015-2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_stk3200
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STK3200 starter kit
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
 * @name    Xtimer configuration
 *
 * The timer runs at 250 kHz to increase accuracy.
 * @{
 */
#define CONFIG_ZTIMER_USEC_DEV              (TIMER_DEV(0))  /**< Timer peripheral for ztimer */
#define CONFIG_ZTIMER_USEC_BASE_FREQ        (250000UL)      /**< Running at 250 kHz */
#define CONFIG_ZTIMER_USEC_WIDTH            (16)            /**< Running on a 16-bit timer */
#define CONFIG_ZTIMER_USEC_ADJUST_SET       (56)            /**< Overhead for ztimer_set */
#define CONFIG_ZTIMER_USEC_ADJUST_SLEEP     (64)            /**< Overhead for ztimer_sleep */
/** @} */

/**
 * @name    Board controller configuration
 *
 * Define the GPIO pin to enable the BC, to allow serial communication
 * via the USB port.
 * @{
 */
#define BC_PIN              GPIO_PIN(PA, 9)
/** @} */

/**
 * @name    Push button pin definitions
 * @{
 */
#define PB0_PIN             GPIO_PIN(PC, 8)
#define PB1_PIN             GPIO_PIN(PC, 9)
/** @} */

/**
 * @name    LED pin definitions
 * @{
 */
#define LED0_PIN            GPIO_PIN(PC, 10)
#define LED1_PIN            GPIO_PIN(PC, 11)
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
 * @name    Display configuration
 *
 * Connection to the on-board Sharp Memory LCD (LS013B7DH03).
 * @{
 */
#define DISP_SPI            SPI_DEV(0)
#define DISP_COM_PIN        GPIO_PIN(PE, 10)
#define DISP_CS_PIN         GPIO_PIN(PE, 11)
#define DISP_SELECT_PIN     GPIO_PIN(PA, 8)
#define DISP_POWER_PIN      GPIO_PIN(PA, 10)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
