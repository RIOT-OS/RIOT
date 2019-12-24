/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32_wemos-lolin-d32-pro
 * @brief       Board specific definitions for Wemos LOLIN D32 Pro
 * @{
 *
 * Wemos LOLIN D32 Pro is a development board that uses the ESP32-WROVER
 * module which has a built-in 4 MByte SPI RAM. Most important
 * features of the board are
 *
 * - Micro-SD card interface
 * - LCD interface
 * - SPI RAM 4 MByte
 *
 * Furthermore, most GPIOs are broken out for extension.
 *
 * When the TFT display is connected, add
 * ```
 * USEMODULE += esp_lolin_tft
 * ```
 * to the makefile of the application to use the according default board
 * configuration.
 *
 * For detailed information about the configuration of ESP32 boards, see
 * section \ref esp32_comm_periph "Common Peripherals".
 *
 * @note
 * Most definitions can be overridden by an \ref esp32_app_spec_conf
 * "application-specific board configuration".
 *
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name LED (on-board) configuration
 * @{
 */
#define LED0_PIN        GPIO5
#define LED0_ACTIVE     (0)     /* LED is low active */
/** @} */

/**
 * @name   SD card interface configuration
 *
 * SD card interface uses SPI_DEV(0) on this board. This configuration cannot
 * be overridden.
 * @{
 */
#if MODULE_SDCARD_SPI
#define SDCARD_SPI_PARAM_SPI    SPI_DEV(0)  /**< SPI_DEV(0) is used (fixed) */
#define SDCARD_SPI_PARAM_CLK    SPI0_SCK    /**< HSPI SCK  is used (fixed) */
#define SDCARD_SPI_PARAM_MOSI   SPI0_MOSI   /**< HSPI MOSI is used (fixed) */
#define SDCARD_SPI_PARAM_MISO   SPI0_MISO   /**< HSPI MISO is used (fixed) */
#define SDCARD_SPI_PARAM_CS     SPI0_CS1    /**< HSPI CS1  is used (fixed) */
#define SDCARD_SPI_PARAM_POWER  GPIO_UNDEF  /**< power control is not used (fixed) */
#endif
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common board definitions as last step */
#include "board_common.h"

#endif /* BOARD_H */
/** @} */
