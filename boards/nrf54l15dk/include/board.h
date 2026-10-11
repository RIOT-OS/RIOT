/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nrf54l15dk
 * @{
 *
 * @file
 * @brief       Board configuration for the nRF54L15-DK board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "cpu.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   System core clock speed of the nRF54L15 application core
 */
#define CLOCK_CORECLOCK     MHZ(128)

/**
 * @brief   LFCLK source: the board has a 32.768 kHz crystal mounted
 */
#define CLOCK_LFCLK         (CLOCK_LFCLK_SRC_SRC_LFXO)

/**
 * @name    LED pin configuration
 *
 * @note    The LEDs on this board are active HIGH
 * @{
 */
#define LED0_PIN            GPIO_PIN(2, 9)  /**< LED0 pin definition */
#define LED1_PIN            GPIO_PIN(1, 10) /**< LED1 pin definition */
#define LED2_PIN            GPIO_PIN(2, 7)  /**< LED2 pin definition */
#define LED3_PIN            GPIO_PIN(1, 14) /**< LED3 pin definition */

#define LED0_MASK           (1 << 9)        /**< LED0 PORT bitmask */
#define LED1_MASK           (1 << 10)       /**< LED1 PORT bitmask */
#define LED2_MASK           (1 << 7)        /**< LED2 PORT bitmask */
#define LED3_MASK           (1 << 14)       /**< LED3 PORT bitmask */

#define LED0_ON             (NRF_P2->OUTSET = LED0_MASK) /**< LED0 ON macro */
#define LED0_OFF            (NRF_P2->OUTCLR = LED0_MASK) /**< LED0 OFF macro */
#define LED0_TOGGLE         (NRF_P2->OUT   ^= LED0_MASK) /**< LED0 toggle macro */

#define LED1_ON             (NRF_P1->OUTSET = LED1_MASK) /**< LED1 ON macro */
#define LED1_OFF            (NRF_P1->OUTCLR = LED1_MASK) /**< LED1 OFF macro */
#define LED1_TOGGLE         (NRF_P1->OUT   ^= LED1_MASK) /**< LED1 toggle macro */

#define LED2_ON             (NRF_P2->OUTSET = LED2_MASK) /**< LED2 ON macro */
#define LED2_OFF            (NRF_P2->OUTCLR = LED2_MASK) /**< LED2 OFF macro */
#define LED2_TOGGLE         (NRF_P2->OUT   ^= LED2_MASK) /**< LED2 toggle macro */

#define LED3_ON             (NRF_P1->OUTSET = LED3_MASK) /**< LED3 ON macro */
#define LED3_OFF            (NRF_P1->OUTCLR = LED3_MASK) /**< LED3 OFF macro */
#define LED3_TOGGLE         (NRF_P1->OUT   ^= LED3_MASK) /**< LED3 toggle macro */
/** @} */

/**
 * @name    Button pin configuration
 *
 * @note    The buttons on this board are active LOW
 * @{
 */
#define BTN0_PIN            GPIO_PIN(1, 13)  /**< BTN0 pin definition */
#define BTN0_MODE           GPIO_IN_PU       /**< BTN0 default mode */
#define BTN1_PIN            GPIO_PIN(1, 9)   /**< BTN1 pin definition */
#define BTN1_MODE           GPIO_IN_PU       /**< BTN1 default mode */
#define BTN2_PIN            GPIO_PIN(1, 8)   /**< BTN2 pin definition */
#define BTN2_MODE           GPIO_IN_PU       /**< BTN2 default mode */
#define BTN3_PIN            GPIO_PIN(0, 4)   /**< BTN3 pin definition */
#define BTN3_MODE           GPIO_IN_PU       /**< BTN3 default mode */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
