/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_esp32s3_wt32_sc01_plus
 * @brief       Board definitions for ESP32-S3 WT32-SC01 Plus boards
 * @{
 *
 * The board definitions in this file are for the Espressif ESP32-S3 WT32-SC01 Plus.
 *
 * Any modifications required for specific applications
 * can be overridden by \ref esp32_application_specific_configurations
 * "application-specific board configuration".
 *
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <stdint.h>

/**
 * @name    Button pin definitions
 * @{
 */

/**
 * @brief   Default button GPIO pin definition
 *
 * ESP32-S3 WT32-SC01 Plus boards have a BOOT button connected to GPIO0, which can be
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
 * ESP32-S3 WT32-SC01 Plus uses a 3.5\" LCD 480 x 320 pixel display with
 * an ST7796UI as driver chip and MCU8080 8-bit parallel interface.
 *
 * This configuration cannot be changed.
 * @{
 */
#define LCD_SCREEN_WIDTH    480     /**< LCD width */
#define LCD_SCREEN_HEIGHT   320     /**< LCD height */

#ifdef MODULE_ST77XX
#define LCD_DB0         GPIO9
#define LCD_DB1         GPIO46
#define LCD_DB2         GPIO3
#define LCD_DB3         GPIO8
#define LCD_DB4         GPIO18
#define LCD_DB5         GPIO17
#define LCD_DB6         GPIO16
#define LCD_DB7         GPIO15
#define LCD_TE          GPIO48
#define LCD_WR          GPIO47
#define LCD_RS          GPIO0
#define LCD_RESET       GPIO4
#define LCD_BACKLIGHT   GPIO45

#define BACKLIGHT_ON    gpio_set(LCD_BACKLIGHT)
#define BACKLIGHT_OFF   gpio_clear(LCD_BACKLIGHT)

#define ST77XX_PARAM_CNTRL          ST77XX_CNTRL_ST7796
#define ST77XX_PARAM_SPI            SPI_UNDEF   /* parallel interface is used */
#define ST77XX_PARAM_D0             LCD_DB0
#define ST77XX_PARAM_D1             LCD_DB1
#define ST77XX_PARAM_D2             LCD_DB2
#define ST77XX_PARAM_D3             LCD_DB3
#define ST77XX_PARAM_D4             LCD_DB4
#define ST77XX_PARAM_D5             LCD_DB5
#define ST77XX_PARAM_D6             LCD_DB6
#define ST77XX_PARAM_D7             LCD_DB7
#define ST77XX_PARAM_WRX            LCD_WR
#define ST77XX_PARAM_RDX            GPIO_UNDEF
#define ST77XX_PARAM_DCX            LCD_RS
#define ST77XX_PARAM_RST            LCD_RESET
#define ST77XX_PARAM_CS             GPIO_UNDEF
#define ST77XX_PARAM_RGB            0
#define ST77XX_PARAM_INVERTED       1
#define ST77XX_PARAM_ROTATION       LCD_MADCTL_MV
#define ST77XX_PARAM_NUM_LINES      LCD_SCREEN_WIDTH
#define ST77XX_PARAM_RGB_CHANNELS   LCD_SCREEN_HEIGHT
#endif
/** @} */

/**
 * @name Touch panel configuration
 * @{
 */
#define FT5X06_PARAM_I2C_DEV    I2C_DEV(0)          /**< I2C device */
#define FT5X06_PARAM_INT_PIN    GPIO7               /**< Interrupt pin */
#define FT5X06_PARAM_RST_PIN    GPIO4               /**< Reset pin */
#define FT5X06_PARAM_XMAX       LCD_SCREEN_WIDTH    /**< Max width */
#define FT5X06_PARAM_YMAX       LCD_SCREEN_HEIGHT   /**< Max height */
#define FT5X06_PARAM_TYPE       FT5X06_TYPE_FT6X36  /**< Device type */
#define FT5X06_PARAM_XYCONV     (FT5X06_SWAP_XY | FT5X06_MIRROR_Y) /**< Swap XY, then mirror Y */
/** @} */

/**
 * @name SD-Card interface configuration
 *
 * SD-Card interface uses SPI_DEV(1) on this board.
 * @{
 */
#define SDCARD_SPI_PARAM_SPI        SPI_DEV(0)
#define SDCARD_SPI_PARAM_CS         SPI0_CS0
#define SDCARD_SPI_PARAM_CLK        SPI0_SCK
#define SDCARD_SPI_PARAM_MOSI       SPI0_MOSI
#define SDCARD_SPI_PARAM_MISO       SPI0_MISO
#define SDCARD_SPI_PARAM_POWER      GPIO_UNDEF
/** @} */

/* include common board definitions as last step */
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
