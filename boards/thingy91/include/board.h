/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_thingy91
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Nordic Thingy:91 board
 *
 * @author      Thanh-Viet Nguyen  <topologicrose@protonmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph/gpio.h"
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
#define CLOCK_LFCLK         (3)             /**< set to  0: internal RC oscillator
                                             *         3: High Accuracy oscillator */
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(0, 26)
#define BTN0_MODE           GPIO_IN_PU
#define LED0_ON             (PORTB |=  LED0_MASK)
#define LED0_OFF            (PORTB &= ~LED0_MASK)
#define LED0_TOGGLE         (PORTB ^=  LED0_MASK)
#define LED0_PIN        GPIO_PIN(0, 29) /**< red */
#define LED1_PIN        GPIO_PIN(0, 30) /**<  green */
#define LED2_PIN        GPIO_PIN(0, 31) /**<  blue */   
/** @} */

/**
 * @name    Power switch pins definitions
 * @{
 */
/** @} */

/**
 * @name    LIS2DH12 low power accelerometer configuration
 * @{
 */
//#define LIS2DH12_PARAM_I2C  I2C_DEV(1)

/** @} */

/**
 * @name    LPS22HB device address
 * @{
 */
//#define LPSXXX_PARAM_ADDR   (0x5c)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
