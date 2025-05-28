/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_esp32s3_usb_otg
 * @brief       Board definitions for the ESP32-S3-USB-OTG boards
 * @{
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
 * @brief   BOOT/OK button GPIO pin definition
 *
 * The BOOT button is connected to GPIO0 and is used as OK button during
 * normal operation. The button is LOW active.
 */
#define BTN0_PIN        GPIO0

/**
 * @brief   BOOT/OK button GPIO mode definition
 */
#define BTN0_MODE       GPIO_IN_PU

/**
 * @brief   Default interrupt flank definition for the BOOT/OK button GPIO
 */
#ifndef BTN0_INT_FLANK
#define BTN0_INT_FLANK  GPIO_FALLING
#endif

/**
 * @brief   Definition for compatibility with previous versions
 */
#define BUTTON0_PIN     BTN0_PIN

/**
 * @brief   MENU button GPIO pin definition
 *
 * The MENU button is connected to GPIO14. It is LOW active.
 */
#define BTN1_PIN        GPIO14

/**
 * @brief   MENU button GPIO mode definition
 */
#define BTN1_MODE       GPIO_IN_PU

/**
 * @brief   Default interrupt flank definition for the MENU button GPIO
 */
#ifndef BTN1_INT_FLANK
#define BTN1_INT_FLANK  GPIO_FALLING
#endif

/**
 * @brief   Definition for compatibility with previous versions
 */
#define BUTTON1_PIN     BTN1_PIN

/**
 * @brief   DW- button GPIO pin definition
 *
 * The DW- button is connected to GPIO11. It is LOW active.
 */
#define BTN2_PIN        GPIO11

/**
 * @brief   DW- button GPIO mode definition
 */
#define BTN2_MODE       GPIO_IN_PU

/**
 * @brief   Default interrupt flank definition for the DW- button GPIO
 */
#ifndef BTN2_INT_FLANK
#define BTN2_INT_FLANK  GPIO_FALLING
#endif

/**
 * @brief   Definition for compatibility with previous versions
 */
#define BUTTON2_PIN     BTN2_PIN

/**
 * @brief   UP+ button GPIO pin definition
 *
 * The MENU button is connected to GPIO10. It is LOW active.
 */
#define BTN3_PIN        GPIO10

/**
 * @brief   UP+ button GPIO mode definition
 */
#define BTN3_MODE       GPIO_IN_PU

/**
 * @brief   Default interrupt flank definition for the UP+ button GPIO
 */
#ifndef BTN3_INT_FLANK
#define BTN3_INT_FLANK  GPIO_FALLING
#endif

/**
 * @brief   Definition for compatibility with previous versions
 */
#define BUTTON3_PIN     BTN3_PIN

/** @} */

/**
 * @name    LED (on-board) configuration
 *
 * ESP32-S3-USB-OTG boards have two LEDs on board.
 * @{
 */
#define LED0_PIN        GPIO15  /**< Green LED */
#define LED0_ACTIVE     (1)     /**< Green LED is high active */

#define LED1_PIN        GPIO16  /**< Yellow LED */
#define LED1_ACTIVE     (1)     /**< Yellow LED is high active */
/** @} */

/**
 * @name    LCD configuration
 *
 * This configuration cannot be changed.
 * @{
 */
#if MODULE_ST77XX
#define LCD_DC              GPIO4   /**< LCD DC signal */
#define LCD_CS              GPIO5   /**< LCD EN signal */
#define LCD_RST             GPIO8   /**< LCD RST signal */
#define LCD_BACKLIGHT       GPIO9   /**< LCD BL signal */

#define ST77XX_PARAM_CNTRL          ST77XX_CNTRL_ST7789
#define ST77XX_PARAM_CS             LCD_CS
#define ST77XX_PARAM_DCX            LCD_DC
#define ST77XX_PARAM_RST            LCD_RST
#define ST77XX_PARAM_NUM_LINES      240
#define ST77XX_PARAM_RGB_CHANNELS   240
#define ST77XX_PARAM_ROTATION       ST77XX_ROTATION_0
#define ST77XX_PARAM_INVERTED       1
#define ST77XX_PARAM_RGB            1

#define BACKLIGHT_ON                gpio_set(LCD_BACKLIGHT)
#define BACKLIGHT_OFF               gpio_clear(LCD_BACKLIGHT)
#endif
/** @} */

/**
 * @name SD-Card interface configuration
 *
 * SD-Card interface uses SPI_DEV(1) on this board.
 * @{
 */
#if (MODULE_SDCARD_SPI) || DOXYGEN
#define SDCARD_SPI_PARAM_SPI        SPI_DEV(1)
#define SDCARD_SPI_PARAM_CS         SPI1_CS0
#define SDCARD_SPI_PARAM_CLK        SPI1_SCK
#define SDCARD_SPI_PARAM_MOSI       SPI1_MOSI
#define SDCARD_SPI_PARAM_MISO       SPI1_MISO
#define SDCARD_SPI_PARAM_POWER      GPIO_UNDEF
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

/** @} */
