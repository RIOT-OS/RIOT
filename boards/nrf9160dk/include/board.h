/*
 * SPDX-FileCopyrightText: 2021 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nrf9160dk
 * @{
 *
 * @file
 * @brief       Board configuration for the nrf9160dk board
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 *
 * @note    The LTE modem will only work with the high accuracy RC oscillator
 *
 * @{
 */
#define CLOCK_HFCLK         (32U)           /**< set to  0: internal RC oscillator
                                             *        32: 32MHz crystal */
/* LFCLK Source clock selection:*/
/* - CLOCK_LFCLKSRC_SRC_LFRC: 32.768 kHz RC oscillator
 * - CLOCK_LFCLKSRC_SRC_LFXO: 32.768 kHz crystal oscillator */
#define CLOCK_LFCLK         (CLOCK_LFCLKSRC_SRC_LFXO) /**< LFCLK Source */
/** @} */

/**
 * @name    LED pin configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 2) /**< LED0 pin definition */
#define LED1_PIN            GPIO_PIN(0, 3) /**< LED1 pin definition */
#define LED2_PIN            GPIO_PIN(0, 4) /**< LED2 pin definition */
#define LED3_PIN            GPIO_PIN(0, 5) /**< LED3 pin definition */

#define LED0_MASK           (1 << 2)       /**< LED0 PORT bitmask */
#define LED1_MASK           (1 << 3)       /**< LED1 PORT bitmask */
#define LED2_MASK           (1 << 4)       /**< LED2 PORT bitmask */
#define LED3_MASK           (1 << 5)       /**< LED3 PORT bitmask */

#define LED_PORT            (NRF_P0_S) /**< Default LED PORT */

#define LED0_ON             (LED_PORT->OUTSET = LED0_MASK) /**< LED0 ON macro */
#define LED0_OFF            (LED_PORT->OUTCLR = LED0_MASK) /**< LED0 OFF macro */
#define LED0_TOGGLE         (LED_PORT->OUT   ^= LED0_MASK) /**< LED0 toggle macro */

#define LED1_ON             (LED_PORT->OUTSET = LED1_MASK) /**< LED1 ON macro */
#define LED1_OFF            (LED_PORT->OUTCLR = LED1_MASK) /**< LED1 OFF macro */
#define LED1_TOGGLE         (LED_PORT->OUT   ^= LED1_MASK) /**< LED1 toggle macro */

#define LED2_ON             (LED_PORT->OUTSET = LED2_MASK) /**< LED2 ON macro */
#define LED2_OFF            (LED_PORT->OUTCLR = LED2_MASK) /**< LED2 OFF macro */
#define LED2_TOGGLE         (LED_PORT->OUT   ^= LED2_MASK) /**< LED2 toggle macro */

#define LED3_ON             (LED_PORT->OUTSET = LED3_MASK) /**< LED3 ON macro */
#define LED3_OFF            (LED_PORT->OUTCLR = LED3_MASK) /**< LED3 OFF macro */
#define LED3_TOGGLE         (LED_PORT->OUT   ^= LED3_MASK) /**< LED3 toggle macro */
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(0, 6)  /**< BTN0 pin definition */
#define BTN0_MODE           GPIO_IN_PU      /**< BTN0 default mode */
#define BTN1_PIN            GPIO_PIN(0, 7)  /**< BTN1 pin definition */
#define BTN1_MODE           GPIO_IN_PU      /**< BTN1 default mode */
#define BTN2_PIN            GPIO_PIN(0, 8)  /**< BTN2 pin definition */
#define BTN2_MODE           GPIO_IN_PU      /**< BTN2 default mode */
#define BTN3_PIN            GPIO_PIN(0, 9)  /**< BTN3 pin definition */
#define BTN3_MODE           GPIO_IN_PU      /**< BTN3 default mode */
/** @} */

/**
 * @name    WS281x RGB LED configuration
 * @{
 */
#define WS281X_TIMER_DEV    TIMER_DEV(1)            /**< Timer device */
#define WS281X_TIMER_MAX_VALUE TIMER_1_MAX_VALUE    /**< Timer max value */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
