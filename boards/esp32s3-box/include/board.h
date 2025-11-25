/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32s3_box
 * @brief       Board definitions for ESP32-S3-Box boards
 * @{
 *
 * The board definitions in this file are for the Espressif ESP32-S3-Box.
 *
 * Any modifications required for specific applications
 * can be overridden by \ref esp32_application_specific_configurations
 * "application-specific board configuration".
 *
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

/**
 * @name    Button pin definitions
 * @{
 */

/**
 * @brief   Default button GPIO pin definition
 *
 * ESP32-S3-Box boards have a BOOT button connected to GPIO0, which can be
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
#define BTN0_INT_FLANK  GPIO_FALLING
#endif

/**
 * @brief   Definition for compatibility with previous versions
 */
#define BUTTON0_PIN     BTN0_PIN

/** @} */

/**
 * @name    LCD display configuration
 *
 * ESP32-S3-Box uses a LCD display with an ILI9342C as driver chip.

 * This configuration cannot be changed.
 * @{
 */
#if MODULE_ILI9341
#define LCD_DC          GPIO4
#define LCD_CS          GPIO5
#define LCD_RST         GPIO48
#define LCD_BACKLIGHT   GPIO45

#define BACKLIGHT_ON    gpio_set(LCD_BACKLIGHT)
#define BACKLIGHT_OFF   gpio_clear(LCD_BACKLIGHT)

#define ILI9341_PARAM_SPI           SPI_DEV(1)
#define ILI9341_PARAM_SPI_CLK       SPI_CLK_10MHZ
#define ILI9341_PARAM_CS            LCD_CS
#define ILI9341_PARAM_DCX           LCD_DC
#define ILI9341_PARAM_RST           LCD_RST
#define ILI9341_PARAM_RGB           0
#define ILI9341_PARAM_INVERTED      0
#define ILI9341_PARAM_ROTATION      LCD_MADCTL_MX | LCD_MADCTL_MY
#define ILI9341_PARAM_NUM_LINES     320U
#define ILI9341_PARAM_RGB_CHANNELS  240U
#endif
/** @} */

/* include common board definitions as last step */
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BOARD_H */
/** @} */
