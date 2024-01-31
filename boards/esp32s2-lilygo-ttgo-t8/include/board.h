/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32s2_lilygo_ttgo_t8
 * @brief       Board definitions for the LILYGO TTGO T8 ESP32-S2 board
 * @{
 *
 * The board definitions in this file are valid for the LILYGO TTGO T8 ESP32-S2
 * board.
 *
 * @note
 * Most definitions can be overridden by an
 * \ref esp32_application_specific_configurations
 * "application-specific board configuration" if necessary.
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
 * LILYGO TTGO T8 ESP32-S2 boards have a BOOT button connected to GPIO0, which
 * can be used as button during normal operation. Since the GPIO9 pin is pulled up,
 * the button signal is inverted, i.e., pressing the button will give a
 * low signal.
 */
#define BTN0_PIN        GPIO0

/**
 * @brief   Default button GPIO mode definition
 *
 * Since the GPIO of the button is pulled up with an external resistor, the
 * mode for the GPIO pin has to be GPIO_IN.
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

/**
 * @name OLED display configuration
 *
 * OLED display uses SPI_DEV(0) on this board.
 * @{
 */
#if defined(MODULE_ST77XX)
#define ST77XX_PARAM_CNTRL          ST77XX_CNTRL_ST7789
#define ST77XX_PARAM_SPI            SPI_DEV(0)      /**< SPI device */
#define ST77XX_PARAM_SPI_CLK        SPI_CLK_10MHZ   /**< SPI clock frequency */
#define ST77XX_PARAM_SPI_MODE       SPI_MODE_0      /**< SPI mode */
#define ST77XX_PARAM_CS             GPIO34          /**< Chip Select pin */
#define ST77XX_PARAM_DCX            GPIO37          /**< DCX pin */
#define ST77XX_PARAM_RST            GPIO38          /**< Reset pin */
#define ST77XX_PARAM_RGB            1               /**< RGB mode enable */
#define ST77XX_PARAM_INVERTED       1               /**< Inverted mode enable */
#define ST77XX_PARAM_NUM_LINES      240U            /**< Number of lines */
#define ST77XX_PARAM_RGB_CHANNELS   135U            /**< Number of columns */
#define ST77XX_PARAM_ROTATION       ST77XX_ROTATION_270  /**< Rotation */
#define ST77XX_PARAM_OFFSET_X       40              /**< X offset */
#define ST77XX_PARAM_OFFSET_Y       52              /**< Y offset */
#endif
/** @} */

/**
 * @name    Backlight control
 * @{
 */
#define BACKLIGHT_PIN   GPIO33                      /**< Backlight pin */
#define BACKLIGHT_ON    gpio_set(BACKLIGHT_PIN)     /**< Turn backlight on */
#define BACKLIGHT_OFF   gpio_clear(BACKLIGHT_PIN)   /**< Turn backlight off */
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
