/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_esp32_olimex-esp32-evb
 * @brief       Board specific definitions for Olimex ESP32-EVB (ESP32-GATEWAY)
 *
 * This configuration is for the Olimex ESP32-EVB, but can also be used
 * for the Olimex ESP32-GATEWAY. To use this board definition with Olimex
 * ESP32-GATEWAY, add
 * ```
 * USEMODULE += olimex_esp32_gateway
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
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include <stdint.h>

/**
 * @name    Button pin definitions
 * @{
 */

/**
 * @brief   Default button GPIO pin definition
 */
#define BTN0_PIN        GPIO34

/**
 * @brief   Default button GPIO mode definition
 *
 * Since the GPIO of the button is pulled up with an external resistor, the
 * mode for the GPIO pin has to be GPIO_IN.
 */
#define BTN0_MODE       GPIO_IN

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
 * @name    LED (on-board) configuration
 *
 * Olimex ESP32-GATEWAY has an on-board LED.
 * @{
 */
#if MODULE_OLIMEX_ESP32_GATEWAY
#define LED0_PIN        GPIO33
#define LED0_ACTIVE     (1)     /**< LED is high active */
#endif
/** @} */

/**
 * @name SD-Card interface configuration
 *
 * SD-Card interface uses SPI_DEV(1) on this board.
 *
 * @note On Olimex ESP32-EVB, the CD/CS pin is not connected and
 * simply pulled-up. Therefore, SPI bus mode is not available and the card
 * interface can be used only in 1-bit SD bus mode. That is, SPI SD-Card
 * is not working. On Olimex ESP32-GATEWAY, the CD/CS pin is connected to
 * GPIO13. The SPI SD-Card driver should work on this board.
 * @{
 */
#if (MODULE_SDCARD_SPI && MODULE_OLIMEX_ESP32_GATEWAY) || DOXYGEN
#define SDCARD_SPI_PARAM_SPI        SPI_DEV(0)
#define SDCARD_SPI_PARAM_CS         GPIO13
#define SDCARD_SPI_PARAM_CLK        SPI0_SCK
#define SDCARD_SPI_PARAM_MOSI       SPI0_MOSI
#define SDCARD_SPI_PARAM_MISO       SPI0_MISO
#define SDCARD_SPI_PARAM_POWER      GPIO_UNDEF
#endif
/** @} */

/**
 * @name    ESP32 Ethernet (EMAC) configuration
 * @{
 */
#if MODULE_ESP_ETH || DOXYGEN
#define EMAC_PHY_LAN8720        1                   /**< LAN8710 used as PHY interface */
#define EMAC_PHY_ADDRESS        0                   /**< PHY0 used as base address */
#define EMAC_PHY_SMI_MDC_PIN    23                  /**< SMI MDC pin */
#define EMAC_PHY_SMI_MDIO_PIN   18                  /**< SMI MDC pin */
#define EMAC_PHY_CLOCK_MODE     ETH_CLOCK_GPIO0_IN  /**< external 50 MHz clock */
#define EMAC_PHY_POWER_PIN      GPIO_UNDEF          /**< power enable pin not used */
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
