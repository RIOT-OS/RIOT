/*
 * Copyright (C) 2020 Freie Universität Berlin
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_feather-nrf52840-sense
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Adafruit Feather nRF52840
 *              Sense
 *
 * @author      Martine S. Lenders <m.lenders@fu-berlin.de>
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "board_common.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(1, 9)
#define LED1_PIN            GPIO_PIN(1, 10)

#define LED_PORT            (NRF_P1)
#define LED0_MASK           (1 << 9)
#define LED1_MASK           (1 << 10)
#define LED_MASK            (LED0_MASK | LED1_MASK)

#define LED0_ON             (LED_PORT->OUTSET = LED0_MASK)
#define LED0_OFF            (LED_PORT->OUTCLR = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->OUT   ^= LED0_MASK)

#define LED1_ON             (LED_PORT->OUTSET = LED1_MASK)
#define LED1_OFF            (LED_PORT->OUTCLR = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT->OUT   ^= LED1_MASK)
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(1, 2)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    BMP280 sensor configuration
 * @{
 */
#define BMX280_PARAM_I2C_DEV   I2C_DEV(0)   /**< I2C device */
/** @} */

/**
 * @name    LIS3MDL 3-axis magnetometer
 * @{
 */
#define LIS3MDL_PARAM_I2C      I2C_DEV(0)   /**< I2C device */
#define LIS3MDL_PARAM_ADDR     (0x1C)       /**< I2C address */
/** @} */

/**
 * @name    SHT30 temperature and humidity sensor
 * @{
 */
#define SHT3X_PARAM_I2C_DEV    I2C_DEV(0)          /**< I2C device */
#define SHT3X_PARAM_I2C_ADDR   (SHT3X_I2C_ADDR_1)  /**< I2C address */
/** @} */

/**
 * @name    LSM6DSXX accelerometer sensor configuration
 * @{
 */
#define LSM6DSXX_PARAM_I2C I2C_DEV(0)
#define LSM6DSXX_PARAM_ADDR (0x6A)
/** @} */

/**
 * @name    WS281x RGB LEDs configuration
 * @{
 */
#ifndef WS281X_PARAM_PIN
#define WS281X_PARAM_PIN    GPIO_PIN(0, 16) /**< GPIO pin connected to the data pin of the first LED */
#endif
#ifndef WS281X_PARAM_NUMOF
#define WS281X_PARAM_NUMOF  (1U)      /**< Number of LEDs chained */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
