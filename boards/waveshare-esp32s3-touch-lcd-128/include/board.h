/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_waveshare-esp32s3-touch-lcd-128
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Waveshare ESP32-S3 1.28-inch
 *              round display board (Touch version)
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Yahia Abdella <yahia.abdella@tuhh.de>
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

/**
 * @brief   Default button GPIO mode definition
 */
#define BTN0_MODE       GPIO_IN_PU

/**
 * @brief   Default interrupt flank definition for the button GPIO
 */
#ifndef BTN0_INT_FLANK
#  define BTN0_INT_FLANK  GPIO_FALLING
#endif

/**
 * @brief   Definition for compatibility with previous versions
 */
#define BUTTON0_PIN     BTN0_PIN

/** @} */

/**
 * @name    LCD configuration
 *
 * The board features a 240x240 TFT display being driven by a GC9A01 LCD driver
 * The display driver is connected via SPI
 *
 * @{
 */
#if MODULE_GC9A01
#  define LCD_DC              GPIO8     /**< LCD DC signal */
#  define LCD_CS              GPIO9     /**< LCD EN signal */
#  define LCD_RST             GPIO14    /**< LCD RST signal */
#  define LCD_BACKLIGHT       GPIO2     /**< LCD BL signal */

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
#    define GC9A01_PARAM_SPI_CLK        SPI_CLK_10MHZ
#  endif

#  define BACKLIGHT_ON                gpio_set(LCD_BACKLIGHT)
#  define BACKLIGHT_OFF               gpio_clear(LCD_BACKLIGHT)
#endif
/** @} */

/**
 * @name    Touch panel configuration
 *
 * The board features a CST816S touch display driver
 * The touch chip is connected via I2C
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
 * @name    IMU configuration
 *
 * The board features a QMI8658 interial measurement unit
 * The IMU is connected via I2C
 *
 * @{
 */
#if MODULE_QMI8658
#  define QMI8658_PARAM_I2C       I2C_DEV(0)
#endif
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
