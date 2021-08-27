/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_adafruit-clue
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Adafruit Clue board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LEDs pin configuration
 * @{
 */
#define LED0_PIN                        GPIO_PIN(1, 1)
#define LED0_MASK                       (1 << 1)
#define LED0_ON                         (NRF_P1->OUTSET = LED0_MASK)
#define LED0_OFF                        (NRF_P1->OUTCLR = LED0_MASK)
#define LED0_TOGGLE                     (NRF_P1->OUT   ^= LED0_MASK)
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN                        GPIO_PIN(1, 2)  /* Button A */
#define BTN0_MODE                       GPIO_IN_PU

#define BTN1_PIN                        GPIO_PIN(1, 10) /* Button B */
#define BTN1_MODE                       GPIO_IN_PU
/** @} */

/**
 * @name    APDS9960 sensor configuration
 * @{
 */
#define APDS99XX_PARAM_DEV              I2C_DEV(1)
#define APDS99XX_PARAM_INT_PIN          GPIO_PIN(0, 9)
/** @} */

/**
 * @name    BMP280 sensor configuration
 * @{
 */
#define BMX280_PARAM_I2C_DEV            I2C_DEV(1)
/** @} */

/**
 * @name    LIS3MDL 3-axis magnetometer I2C address
 */
#define LIS3MDL_PARAM_I2C               I2C_DEV(1)
#define LIS3MDL_PARAM_ADDR              (0x1C)
/** @} */

/**
 * @name    STH31 temperature and humidity sensor I2C address
 */
#define SHT3X_PARAM_I2C_DEV             I2C_DEV(1)
#define SHT3X_PARAM_I2C_ADDR            (SHT3X_I2C_ADDR_1)
/** @} */

/**
 * @name    Speaker pin
 * @{
 */
#define SPKR_PIN                        GPIO_PIN(1, 0)
/** @ */

/**
 * @name    Backlight control defines, default uses LCD_BACKLIGHT_LOW values
 * @{
 */
#define BACKLIGHT_PIN                   GPIO_PIN(1, 5)
#define BACKLIGHT_MASK                  (1 << 5)
#define BACKLIGHT_ON                    (NRF_P1->OUTSET = BACKLIGHT_MASK)
#define BACKLIGHT_OFF                   (NRF_P1->OUTCLR = BACKLIGHT_MASK)
/** @ */

/**
 * @name    Display configuration
 * @{
 */
#define ILI9341_PARAM_SPI               SPI_DEV(1)
#define ILI9341_PARAM_CS                GPIO_PIN(0, 12)
#define ILI9341_PARAM_DCX               GPIO_PIN(0, 13)
#define ILI9341_PARAM_RST               GPIO_PIN(1, 3)
#define ILI9341_PARAM_NUM_LINES         (240U)
#define ILI9341_PARAM_RGB               (1)
#define ILI9341_PARAM_INVERTED          (1)
#define ILI9341_PARAM_ROTATION          (ILI9341_ROTATION_HORZ_FLIP)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
