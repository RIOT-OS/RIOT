/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_esp32_esp-wrover-kit
 * @brief       Board specific definitions for Espressif ESP-WROVER-KIT V3
 * @{
 *
 * The Espressif ESP-WROVER-KIT is a development board that uses the
 * ESP32-WROVER module which includes a built-in 4 MByte SPI RAM. Most
 * important features of the board are
 *
 * - Micro-SD card interface
 * - OV7670 camera interface
 * - 3.2\" SPI LCD panel
 * - RGB LED
 *
 * Furthermore, many GPIOs are broken out for extension. The USB bridge
 * based on FDI FT2232HL provides a JTAG interface for debugging through
 * the USB interface.
 *
 * When the camera module is connected, add
 * ```
 * USEMODULE += esp32_wrover_kit_camera
 * ```
 * to the makefile of the application to use the according default board
 * configuration.
 *
 * For detailed information about the configuration of ESP32 boards, see
 * section \ref esp32_peripherals "Common Peripherals".
 *
 * @note
 * Most definitions can be overridden by an \ref esp32_application_specific_configurations
 * "application-specific board configuration".
 *
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <stdint.h>

/**
 * @name    LED (on-board) configuration
 * @{
 */

#if !MODULE_ESP32_WROVER_KIT_CAMERA || DOXYGEN
#define LED0_PIN        GPIO0   /**< LED0 is available when the camera is not plugged in */
#define LED0_ACTIVE     (1)     /**< LED0 is high active */
#endif

#if !MODULE_PERIPH_SPI || DOXYGEN
#define LED1_PIN        GPIO2    /**< LED1 is available when the SPI bus is not used */
#define LED1_ACTIVE     (1)      /**< LED1 is high active */
#endif

#if (!MODULE_ESP32_WROVER_KIT_CAMERA && !MODULE_PERIPH_SDMMC) || DOXYGEN
#define LED2_PIN        GPIO4    /**< LED2 is available when the camera is not plugged in
                                      and SDMMD is not used */
#define LED2_ACTIVE     (1)      /**< LED2 is high active */
#endif

#ifdef  LED0_PIN
#define LED_RED_PIN     LED0_PIN /**< LED0 is a red LED */
#endif
#ifdef  LED1_PIN
#define LED_GREEN_PIN   LED1_PIN /**< LED1 is a green LED */
#endif
#ifdef  LED2_PIN
#define LED_BLUE_PIN    LED2_PIN /**< LED2 is a blue LED */
#endif

/** @} */

/**
 * @name   SD-Card interface configuration
 *
 * The SD-Card interface is connected to the HSPI interface. Since HSPI
 * becomes SPI_DEV(0) when the camera is connected, the SD card interface
 * uses SPI_DEV(0) in this case. Otherwise, SPI_DEV(1) is used.
 *
 * This configuration cannot be changed.
 * @{
 */
#if MODULE_SDCARD_SPI || DOXYGEN
#define SDCARD_SPI_PARAM_SPI    SPI_DEV(0)  /**< SPI_DEV(0) is used when camera is connected */
#define SDCARD_SPI_PARAM_CLK    SPI0_SCK    /**< HSPI SCK  is used (fixed) */
#define SDCARD_SPI_PARAM_MOSI   SPI0_MOSI   /**< HSPI MOSI is used (fixed) */
#define SDCARD_SPI_PARAM_MISO   SPI0_MISO   /**< HSPI MISO is used (fixed) */
#define SDCARD_SPI_PARAM_CS     SPI0_CS0    /**< HSPI CS1  is used (fixed) */
#define SDCARD_SPI_PARAM_POWER  GPIO_UNDEF  /**< power control is not used (fixed) */
#endif
/** @} */

/**
 * @name    LCD configuration
 *
 * This configuration cannot be changed.
 * @{
 */
#if MODULE_ILI9341 || DOXYGEN
#define LCD_CS                  GPIO22
#define LCD_RST                 GPIO18
#define LCD_DC                  GPIO21
#define LCD_BACKLIGHT           GPIO5

#define BACKLIGHT_ON            gpio_clear(LCD_BACKLIGHT)
#define BACKLIGHT_OFF           gpio_set(LCD_BACKLIGHT)

#define ILI9341_PARAM_SPI       SPI_DEV(1)
#define ILI9341_PARAM_SPI_CLK   SPI_CLK_10MHZ
#define ILI9341_PARAM_CS        LCD_CS
#define ILI9341_PARAM_DCX       LCD_DC
#define ILI9341_PARAM_RST       LCD_RST
#define ILi9341_PARAM_RGB       0
#define ILI9341_PARAM_INVERTED  0
#define ILI9341_PARAM_ROTATION  ILI9341_ROTATION_HORZ_FLIP
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
