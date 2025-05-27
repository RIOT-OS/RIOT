/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_esp32s3_box
 * @brief       Peripheral configurations for ESP32-S3-Box boards
 * @{
 *
 * The peripheral configurations in this file are for the Espressif
 * ESP32-S3-Box.
 *
 * Any modifications required for specific applications
 * can be overridden by \ref esp32_application_specific_configurations
 * "application-specific board configuration".
 *
 * For detailed information about the peripheral configuration for ESP32-S3
 * boards, see section \ref esp32_peripherals "Common Peripherals".
 *
 * @note
 * Most definitions can be overridden by an \ref esp32_application_specific_configurations
 * "application-specific board configuration" if necessary.
 *
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   I2C configuration
 *
 * I2C_DEV(0) is available at the PMOD1 connector.
 *
 * @note The GPIOs listed in the configuration are only initialized as I2C
 * signals when module `periph_i2c` is used. Otherwise they are not
 * allocated and can be used for other purposes.
 *
 * @{
 */
#ifndef I2C0_SPEED
#define I2C0_SPEED  I2C_SPEED_FAST  /**< I2C bus speed of I2C_DEV(0) */
#endif
#ifndef I2C0_SCL
#define I2C0_SCL    GPIO40          /**< SCL signal of I2C_DEV(0) */
#endif
#ifndef I2C0_SDA
#define I2C0_SDA    GPIO41          /**< SDA signal of I2C_DEV(0) */
#endif
/** @} */

/**
 * @name    SPI configuration
 *
 * SPI_DEV(0) is available at the PMOD2 connector.
 * SPI_DEV(1) is used for the LCD display with an ILI9342C as driver chip.
 *
 * @note The GPIOs listed in the configuration are first initialized as SPI
 * signals when the corresponding SPI interface is used for the first time
 * by either calling the `spi_init_cs` function or the `spi_acquire`
 * function. That is, they are not allocated as SPI signals before and can
 * be used for other purposes as long as the SPI interface is not used.
 * @{
 */
#ifndef SPI0_CTRL
#define SPI0_CTRL   SPI2_HOST   /**< FSPI is used as SPI_DEV(0) */
#endif
#ifndef SPI0_SCK
#define SPI0_SCK    GPIO12      /**< FSPI SCK (pin FSPICLK) */
#endif
#ifndef SPI0_MISO
#define SPI0_MISO   GPIO13      /**< FSPI MISO (pin FSPIQ) */
#endif
#ifndef SPI0_MOSI
#define SPI0_MOSI   GPIO11      /**< FSPI MOSI (pin FSPID) */
#endif
#ifndef SPI0_CS0
#define SPI0_CS0    GPIO10      /**< FSPI CS0 (pin FSPICS0) */
#endif

#ifndef SPI1_CTRL
#define SPI1_CTRL   SPI3_HOST   /**< SPI3 is used as SPI_DEV(1) */
#endif
#ifndef SPI1_SCK
#define SPI1_SCK    GPIO7       /**< LCD_SCK */
#endif
#ifndef SPI1_MISO
#define SPI1_MISO   GPIO35      /**< not used and not broken out on ESP32-S3-WROOM-1 */
#endif
#ifndef SPI1_MOSI
#define SPI1_MOSI   GPIO6       /**< LCD_SDA */
#endif
#ifndef SPI1_CS0
#define SPI1_CS0    GPIO5       /**< LCD_CS */
#endif
/** @} */

/**
 * @name   UART configuration
 *
 * UART_DEV(0) is available at the PMOD2 connector.
 *
 * @{
 */
#define UART0_TXD   GPIO43  /**< direct I/O pin for UART_DEV(0) TxD, can't be changed */
#define UART0_RXD   GPIO44  /**< direct I/O pin for UART_DEV(0) RxD, can't be changed */

/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common peripheral definitions as last step */
#include "periph_conf_common.h"

/** @} */
