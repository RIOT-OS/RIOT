/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_elecrow-crowpanel-esp32s3-rotary-128
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for the Elecrow CrowPanel
 *              ESP32-S3 1.28-inch rotary round display board (Touch version)
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 */

#include <stdint.h>

#include "board.h"

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
 * GPIO4 and GPIO12 are connected to the external flat flex header.
 */
#ifndef ADC_GPIOS
#  define ADC_GPIOS     { GPIO4, GPIO12 }
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
#  define SPI0_MISO   GPIO_UNDEF    /**< LCD SDO (not connected) */
#endif
#ifndef SPI0_CS0
#  define SPI0_CS0    GPIO9         /**< LCD CS */
#endif
/** @} */

/**
 * @name   I2C configuration
 *
 * I2C_DEV(0) is used for the touch controller.<br/>
 * I2C_DEV(1) is connected to the external I2C header. It has a level shifter
 * and is therefore 5V tolerant.
 *
 * @note The GPIOs listed in the configuration are only initialized as I2C
 *       signals when module `periph_i2c` is used. Otherwise they are not
 *       allocated and can be used for other purposes.
 *
 * @{
 */
#ifndef I2C0_SPEED
#  define I2C0_SPEED      I2C_SPEED_FAST  /**< I2C bus speed of I2C_DEV(0) */
#endif
#ifndef I2C0_SCL
#  define I2C0_SCL        GPIO7           /**< SCL signal of I2C_DEV(0) */
#endif
#ifndef I2C0_SDA
#  define I2C0_SDA        GPIO6           /**< SDA signal of I2C_DEV(0) */
#endif
#ifndef I2C1_SPEED
#  define I2C1_SPEED      I2C_SPEED_FAST  /**< I2C bus speed of I2C_DEV(1) */
#endif
#ifndef I2C1_SCL
#  define I2C1_SCL        GPIO39          /**< SCL signal of I2C_DEV(1) */
#endif
#ifndef I2C1_SDA
#  define I2C1_SDA        GPIO38          /**< SDA signal of I2C_DEV(1) */
#endif
/** @} */

/**
 * @brief Declaration of the channels for device PWM_DEV(0)
 *
 * LED pins are used as PWM channels.
 */
#ifndef PWM0_GPIOS
#  define PWM0_GPIOS    { LED0_PIN }
#endif

#ifdef __cplusplus
}
#endif

/* include common peripheral definitions as last step */
#include "periph_conf_common.h"

/** @} */
