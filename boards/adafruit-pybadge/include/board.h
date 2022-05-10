/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_adafruit-pybadge
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Adafruit PyBadge
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "mtd.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PA, 23)                    /**< LED0 pin */

#define LED_PORT            PORT->Group[PA]                     /**< LED0 port */
#define LED0_MASK           (1 << 23)                           /**< LED0 mask */

#define LED0_ON             (LED_PORT.OUTSET.reg = LED0_MASK)   /**< LED0 on macro */
#define LED0_OFF            (LED_PORT.OUTCLR.reg = LED0_MASK)   /**< LED0 off macro */
#define LED0_TOGGLE         (LED_PORT.OUTTGL.reg = LED0_MASK)   /**< LED0 toggle macro */

#define LED0_NAME           "LED(Red)"                          /**< LED0 name */
/** @} */

/**
 * @name    Buttons control (TI SN74HC165 shift register)
 * @{
 */
#define BUTTON_LATCH        GPIO_PIN(PB, 0)                     /**< Latch pin */
#define BUTTON_CLK          GPIO_PIN(PB, 31)                    /**< Clock pin */
#define BUTTON_OUT          GPIO_PIN(PB, 30)                    /**< Serial output pin */
/** @} */

/**
 * @name    Backlight control
 * @{
 */
#define BACKLIGHT_PIN          GPIO_PIN(PA, 1)                  /**< Backlight pin */
#define BACKLIGHT_PORT         PORT->Group[PA]                  /**< Backlight pin port */
#define BACKLIGHT_MASK         (1 << 1)                         /**< Backlight pin mask */
#define BACKLIGHT_ON           (BACKLIGHT_PORT.OUTSET.reg = BACKLIGHT_MASK) /**< Turn backlight on */
#define BACKLIGHT_OFF          (BACKLIGHT_PORT.OUTCLR.reg = BACKLIGHT_MASK) /**< Turn backlight off */
/** @} */

/**
 * @name    Display configuration (not supported yet)
 * @{
 */
#define ST7735_PARAM_SPI        SPI_DEV(1)                      /**< SPI device */
#define ST7735_PARAM_CS         GPIO_PIN(PB, 7)                 /**< Chip select pin */
#define ST7735_PARAM_DCX        GPIO_PIN(PB, 5)                 /**< DCX pin */
#define ST7735_PARAM_RST        GPIO_PIN(PA, 0)                 /**< Reset pin */
#define ST7735_PARAM_NUM_LINES  (160U)                          /**< Number of screen lines */
#define ST7735_PARAM_RGB_CHANNELS   (128U)                      /**< Number of screen rgb channel (height) */
#define ST7735_PARAM_RGB        (1)                             /**< RGB configuration */
#define ST7735_PARAM_INVERTED   (0)                             /**< Inversion configuration */
#define ST7735_PARAM_ROTATION   (ST7735_ROTATION_HORZ_FLIP)     /**< Rotation mode */
#define LCD_SCREEN_WIDTH        (ST7735_PARAM_NUM_LINES)        /**< LCD screen width */
#define LCD_SCREEN_HEIGHT       (ST7735_PARAM_RGB_CHANNELS)     /**< LCD screen height */
/** @} */

/**
 * @name    Neopixel LEDs (not supported yet)
 * @{
 */
#define WS281X_PARAM_PIN        (GPIO_PIN(PA, 15))              /**< GPIO pin connected to the data pin of the first LED */
#define WS281X_PARAM_NUMOF      (5U)                            /**< Number of LEDs chained */
/** @} */

/**
 * @name    Speaker (supported via DAC DDS module)
 * @{
 */
#define SPEAKER_ENABLE_PIN      GPIO_PIN(PA, 27)                /**< Speaker enable pin */
#define SPEAKER_PIN             GPIO_PIN(PA, 2)                 /**< Speaker pin (A0) */
/** @} */

/**
 * @name MTD configuration
 * @{
 */
extern mtd_dev_t *mtd0;                                         /**< pointer to mtd0 */
#define MTD_0 mtd0                                              /**< mtd0 constant */
/** @} */

/**
 * @name    Xtimer configuration
 * @{
 */
#define XTIMER_WIDTH                (32)                        /**< Default timer is 32bit width */
#define XTIMER_HZ                   (1000000ul)                 /**< Default timer runs at 1MHz */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
