/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_adafruit-clue
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Adafruit Clue board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "cpu.h"
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LEDs pin configuration
 * @{
 */
#define LED0_PIN               GPIO_PIN(1, 1)                  /**< LED0 pin */
#define LED0_MASK              (1 << 1)                        /**< LED0 mask */
#define LED0_ON                (NRF_P1->OUTSET = LED0_MASK)    /**< Turn LED0 on */
#define LED0_OFF               (NRF_P1->OUTCLR = LED0_MASK)    /**< Turn LED0 off */
#define LED0_TOGGLE            (NRF_P1->OUT   ^= LED0_MASK)    /**< Toggle LED0 */
/** @} */

/**
 * @name    Buttons pin configuration
 * @{
 */
#define BTN0_PIN               GPIO_PIN(1, 2)  /**< Button A pin */
#define BTN0_MODE              GPIO_IN_PU      /**< Button A mode */

#define BTN1_PIN               GPIO_PIN(1, 10) /**< Button B pin */
#define BTN1_MODE              GPIO_IN_PU      /**< Button B mode */
/** @} */

/**
 * @name    APDS9960 sensor configuration
 * @{
 */
#define APDS99XX_PARAM_DEV     I2C_DEV(1)       /**< I2C device */
#define APDS99XX_PARAM_INT_PIN GPIO_PIN(0, 9)   /**< Interrupt pin */
/** @} */

/**
 * @name    BMP280 sensor configuration
 * @{
 */
#define BMX280_PARAM_I2C_DEV   I2C_DEV(1)   /**< I2C device */
/** @} */

/**
 * @name    LIS3MDL 3-axis magnetometer
 * @{
 */
#define LIS3MDL_PARAM_I2C      I2C_DEV(1)   /**< I2C device */
#define LIS3MDL_PARAM_ADDR     (0x1C)       /**< I2C address */
/** @} */

/**
 * @name    STH31 temperature and humidity sensor
 * @{
 */
#define SHT3X_PARAM_I2C_DEV    I2C_DEV(1)          /**< I2C device */
#define SHT3X_PARAM_I2C_ADDR   (SHT3X_I2C_ADDR_1)  /**< I2C address */
/** @} */

/**
 * @name    Speaker pin
 * @{
 */
#define SPKR_PIN               GPIO_PIN(1, 0)   /**< Speaker pin */
/** @} */

/**
 * @name    Backlight control defines, default uses LCD_BACKLIGHT_LOW values
 * @{
 */
#define BACKLIGHT_PIN          GPIO_PIN(1, 5)                       /**< Backlight pin */
#define BACKLIGHT_MASK         (1 << 5)                             /**< Backlight pin mask */
#define BACKLIGHT_ON           (NRF_P1->OUTSET = BACKLIGHT_MASK)    /**< Turn backlight on */
#define BACKLIGHT_OFF          (NRF_P1->OUTCLR = BACKLIGHT_MASK)    /**< Turn backlight off */
/** @} */

/**
 * @name    Display configuration
 * @{
 */
#define ILI9341_PARAM_SPI       SPI_DEV(1)                      /**< SPI device */
#define ILI9341_PARAM_CS        GPIO_PIN(0, 12)                 /**< Chip select pin */
#define ILI9341_PARAM_DCX       GPIO_PIN(0, 13)                 /**< DCX pin */
#define ILI9341_PARAM_RST       GPIO_PIN(1, 3)                  /**< Reset pin */
#define ILI9341_PARAM_NUM_LINES (240U)                          /**< Number of screen lines */
#define ILI9341_PARAM_RGB       (1)                             /**< RGB configuration */
#define ILI9341_PARAM_INVERTED  (1)                             /**< Inversion configuration */
#define ILI9341_PARAM_ROTATION  (ILI9341_ROTATION_VERT)         /**< Rotation mode */
#define ILI9341_PARAM_OFFSET_X  (80)                            /**< Vertical rotation requires a 80 pixel offset */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
