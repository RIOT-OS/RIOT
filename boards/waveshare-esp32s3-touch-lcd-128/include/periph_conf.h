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
 * @brief       Configuration of CPU peripherals for the Waveshare ESP32-S3
 *              1.28-inch round display board (Touch version)
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Yahia Abdella <yahia.abdella@tuhh.de>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ADC channel configuration
 * @{
 */
/**
 * @brief   Declaration of GPIOs that can be used as ADC channels
 *
 * GPIO1 can be used for Battery voltage monitoring as ADC_LINE(0):
 *  V_BAT = 3.3 / (1 << 12) * 3 * ADC_Value
 */
#ifndef ADC_GPIOS
#  define ADC_GPIOS { GPIO1 }
#endif
/** @} */

/**
 * @name    SPI configuration
 *
 * SPI_DEV(0) is used for the LCD display
 *
 * @note The GPIOs listed in the configuration are first initialized as SPI
 *       signals when the corresponding SPI interface is used for the first
 *       time by either calling the `spi_init_cs` function or the `spi_acquire`
 *       function. Otherwise they are not allocated as SPI signals before and
 *       can be used for other purposes as long as the SPI interface is not
 *       used.
 * @{
 */
#ifndef SPI0_CTRL
#  define SPI0_CTRL   SPI2_HOST     /**< SPI2 (FSPI) is configured as SPI_DEV(0) */
#endif
#ifndef SPI0_SCK
#  define SPI0_SCK    GPIO10        /**< LCD SCLK */
#endif
#ifndef SPI0_MOSI
#  define SPI0_MOSI   GPIO11        /**< LCD SDA */
#endif
#ifndef SPI0_MISO
#  define SPI0_MISO   GPIO12        /**< LCD SDO */
#endif
#ifndef SPI0_CS0
#  define SPI0_CS0    GPIO9         /**< LCD CS */
#endif
/** @} */

/**
 * @name   I2C configuration
 *
 * I2C_DEV(0) is used for the IMU and the touch controller.
 *
 * @note The GPIOs listed in the configuration are only initialized as I2C
 *       signals when module `periph_i2c` is used. Otherwise they are not
 *       allocated and can be used for other purposes.
 *
 * @{
 */
#ifndef I2C0_SPEED
#  define I2C0_SPEED      I2C_SPEED_FAST      /**< I2C bus speed of I2C_DEV(0) */
#endif
#ifndef I2C0_SCL
#  define I2C0_SCL        GPIO7               /**< SCL signal of I2C_DEV(0) */
#endif
#ifndef I2C0_SDA
#  define I2C0_SDA        GPIO6               /**< SDA signal of I2C_DEV(0) */
#endif
/** @} */

/**
 * @name   UART configuration
 *
 * ESP32-S3 provides 3 UART interfaces at maximum:
 *
 * UART_DEV(0) uses fixed standard configuration.<br>
 * UART_DEV(1) is not used.<br>
 *
 * @{
 */
#define UART0_TXD   GPIO43  /**< direct I/O pin for UART_DEV(0) TxD, can't be changed */
#define UART0_RXD   GPIO44  /**< direct I/O pin for UART_DEV(0) RxD, can't be changed */
/** @} */

#ifdef __cplusplus
}
#endif

/* include common peripheral definitions as last step */
#include "periph_conf_common.h"

/** @} */
