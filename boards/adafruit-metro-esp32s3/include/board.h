/*
 * SPDX-FileCopyrightText: 2026 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_adafruit_metro_esp32s3
 * @brief       Board definitions for the Adafruit Metro ESP32-S3 board
 * @{
 *
 * The board definitions in this file are valid for the Adafruit Metro
 * ESP32-S3 board with 16 MB Flash and 8 MB PSRAM.
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
 * The Adafruit Metro ESP32-S3 board has a BOOT button connected to GPIO0,
 * which can be used as button during normal operation. Since the GPIO0 pin
 * is pulled up, the button signal is inverted, i.e., pressing the button
 * will give a low signal.
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
 * @name    LED (on-board) configuration
 *
 * The Adafruit Metro ESP32-S3 board has a red LED labeled `L` that is
 * connected to GPIO13 (Arduino pin D13) and one 24-bit RGB-LED (NeoPixel)
 * connected to GPIO46.
 * @{
 */
#define LED0_PIN            GPIO13  /**< Red LED `L` */
#define LED0_ACTIVE         (1)     /**< Red LED is high active */

#ifndef WS281X_PARAM_PIN
#  define WS281X_PARAM_PIN    (GPIO46)    /**< GPIO pin connected to the NeoPixel */
#endif
#ifndef WS281X_PARAM_NUMOF
#  define WS281X_PARAM_NUMOF  (1U)        /**< Number of LEDs chained */
#endif
/** @} */

/**
 * @name    SD-Card interface configuration
 *
 * The microSD card slot is connected to SPI_DEV(0), which is also routed
 * to the ICSP header.
 * @{
 */
#if MODULE_SDCARD_SPI || DOXYGEN
#  define SDCARD_SPI_PARAM_SPI      SPI_DEV(0)  /**< SPI device */
#  define SDCARD_SPI_PARAM_CS       SPI0_CS0    /**< SD_CS (GPIO45) */
#  define SDCARD_SPI_PARAM_CLK      SPI0_SCK    /**< SCK (GPIO39) */
#  define SDCARD_SPI_PARAM_MOSI     SPI0_MOSI   /**< MOSI (GPIO42) */
#  define SDCARD_SPI_PARAM_MISO     SPI0_MISO   /**< MISO (GPIO21) */
#  define SDCARD_SPI_PARAM_POWER    GPIO_UNDEF  /**< not used */
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
