/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_elecrow-crowpanel-esp32s3-rotary-128
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Elecrow CrowPanel
 *              ESP32-S3 1.28-inch rotary round display board (Touch version)
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Yahia Abdella <yahia.abdella@tuhh.de>
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 */

/**
 * @name    Button pin definitions
 * @{
 */

/**
 * @brief   Default button GPIO pin definition
 *
 * The board has a BOOT button connected to GPIO0, which can be
 * used as button during normal operation. Since the GPIO0 pin is pulled up,
 * the button signal is inverted, i.e., pressing the button will give a
 * low signal.
 */
#define BTN0_PIN        GPIO0

/** Default button GPIO mode definition */
#define BTN0_MODE       GPIO_IN_PU

/** Default interrupt flank definition for the BOOT button */
#ifndef BTN0_INT_FLANK
#  define BTN0_INT_FLANK    GPIO_FALLING
#endif

/** Definition for compatibility with previous versions */
#define BUTTON0_PIN     BTN0_PIN

/**
 * @brief   Display push button
 *
 * The display itself can be pressed down (apart from the touch functionality),
 * which activates a button. The button signal is low active.
 */
#define BTN1_PIN        GPIO41

/** Default button GPIO mode definition */
#define BTN1_MODE       GPIO_IN_PU

/** Default interrupt flank definition for the display button */
#ifndef BTN1_INT_FLANK
#  define BTN1_INT_FLANK    GPIO_FALLING
#endif

/** Definition for compatibility with previous versions */
#define BUTTON1_PIN     BTN1_PIN
/** @} */

/**
 * @name    LCD configuration
 *
 * The board features a 240x240 TFT display being driven by a GC9A01 LCD driver.
 * The display driver is connected via SPI.
 *
 * @{
 */
#if MODULE_GC9A01
#  define LCD_DC              GPIO3     /**< LCD DC signal */
#  define LCD_CS              GPIO9     /**< LCD CS signal */
#  define LCD_RST             GPIO14    /**< LCD RST signal */
#  define LCD_BACKLIGHT       GPIO46    /**< LCD BL signal */

#  define GC9A01_PARAM_CS               LCD_CS
#  define GC9A01_PARAM_DCX              LCD_DC
#  define GC9A01_PARAM_RST              LCD_RST
#  define GC9A01_PARAM_NUM_LINES        240
#  define GC9A01_PARAM_RGB_CHANNELS     240
#  define GC9A01_PARAM_INVERTED         1
#  define GC9A01_PARAM_RGB              0
#  ifndef GC9A01_PARAM_ROTATION
#    define GC9A01_PARAM_ROTATION       GC9A01_ROTATION_VERT
#  endif
#  ifndef GC9A01_PARAM_SPI_CLK
#    define GC9A01_PARAM_SPI_CLK        SPI_CLK_1MHZ
#  endif

#  define BACKLIGHT_ON                gpio_set(LCD_BACKLIGHT)
#  define BACKLIGHT_OFF               gpio_clear(LCD_BACKLIGHT)
#endif
/** @} */

/**
 * @name    Touch panel configuration
 *
 * The board features a CST816D touch display driver connected via I2C.
 *
 * @note The CST816D is fully register compatible with the CST816S.
 *
 * @{
 */
#if MODULE_CST816S
#  define CST816S_PARAM_I2C_DEV       I2C_DEV(0)
#  define CST816S_PARAM_IRQ           GPIO5
#  define CST816S_PARAM_RESET         GPIO13
#endif
/** @} */

/**
 * @name    Power Control configuration
 *
 * The Elecrow rotary display has two MOSFETs that control the power domains,
 * one for the LCD's 3.3V domain and one for the external 5V supply of the
 * UART and I2C headers.
 *
 * @{
 */
/** Enable power for the LCD (active high) */
#define POWER_LCD_3V3_PIN       GPIO1
/** Enable power for the external UART and I2C headers (active high) */
#define POWER_EXTERNAL_5V_PIN   GPIO2
/** @} */

/**
 * @name    Neopixel LEDs (WS281x compatible)
 * @{
 */
/** GPIO pin connected to the data pin of the first LED */
#define WS281X_PARAM_PIN    GPIO48
/** Number of LEDs chained */
#define WS281X_PARAM_NUMOF  (5U)
/** @} */

/**
 * @name    LED (on-board) configuration
 *
 * @{
 */
#define LED0_PIN        GPIO40  /**< Red LED */
#define LED0_ACTIVE     (0)     /**< Red LED is active low */
/** @} */

/* include common board definitions as last step */
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/** @} */
