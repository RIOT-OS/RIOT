/*
 * SPDX-FileCopyrightText: 2024 Isikcan 'Jon' Yilmaz
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_seeedstudio-xiao-esp32s3
 * @brief       Peripheral configurations for Seeed Studio Xiao ESP32S3 boards
 * @{
 *
 * Any modifications required for specific applications
 * can be overridden by \ref esp32_application_specific_configurations
 * "application-specific board configuration".
 *
 * For detailed information about the peripheral configuration for ESP32-S3
 * boards, see section \ref esp32_peripherals "Common Peripherals".
 *
 * @file
 * @author      Isikcan 'Jon' Yilmaz <can158@gmail.com>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ADC Channel Configuration
 * @{
 */
/**
 * @brief   Declaration of GPIOs that can be used as ADC channels
 *
 * All pins of ADC1 are declared as ADC channels. Keep in mind that GPIO10 is
 * not broken out and used as VBAT signal. GPIO3 is a strapping pin if
 * `EFUSE_STRAP_JTAG_SEL` is set, see \ref esp32_gpio_pins_esp32s3_strapping.
 *
 * @note As long as the GPIOs listed in ADC_GPIOS are not initialized as ADC
 * channels with the `adc_init` function, they can be used for other
 * purposes.
 */
#ifndef ADC_GPIOS
#  define ADC_GPIOS { GPIO1, GPIO2, GPIO3, GPIO4, GPIO5, GPIO6, GPIO7, GPIO8, GPIO9 }
#endif
/** @} */

/**
 * @name   I2C configuration
 *
 * For the Seeedstudio Xiao ESP32S3 board, only one I2C interface I2C_DEV(0)
 * is defined.
 *
 * The GPIOs listed in the configuration are only initialized as I2C signals
 * when module `periph_i2c` is used. Otherwise they are not allocated and
 * can be used for other purposes.
 *
 * @{
 */
#ifndef I2C0_SPEED
#  define I2C0_SPEED    I2C_SPEED_FAST  /**< I2C bus speed of I2C_DEV(0) */
#endif
#ifndef I2C0_SCL
#  define I2C0_SCL      GPIO6           /**< SCL signal of I2C_DEV(0) */
#endif
#ifndef I2C0_SDA
#  define I2C0_SDA      GPIO5           /**< SDA signal of I2C_DEV(0) */
#endif
/** @} */

/**
 * @name   PWM channel configuration
 *
 * Two PWM devices are configured. These devices can use all GPIOs that are not
 * defined as I2C, SPI or UART for this board. Generally, all outputs pins
 * could be used as PWM channels.
 *
 * @note As long as the according PWM device is not initialized with
 * the `pwm_init`, the GPIOs declared for this device can be used
 * for other purposes.
 *
 * @{
 */

/**
 * @brief Declaration of the channels for device PWM_DEV(0),
 *        at maximum PWM_CHANNEL_NUM_DEV_MAX.
 */
#ifndef PWM0_GPIOS
#  define PWM0_GPIOS    { GPIO0, GPIO1, GPIO2 }
#endif

/**
 * @brief Declaration of the channels for device PWM_DEV(1),
 *        at maximum PWM_CHANNEL_NUM_DEV_MAX.
 */
#ifndef PWM1_GPIOS
#  define PWM1_GPIOS    { GPIO21 }
#endif
/** @} */

/**
 * @name    SPI configuration
 *
 * @note The GPIOs listed in the configuration are first initialized as SPI
 * signals when the corresponding SPI interface is used for the first time
 * by either calling the `spi_init_cs` function or the `spi_acquire`
 * function. That is, they are not allocated as SPI signals before and can
 * be used for other purposes as long as the SPI interface is not used.
 * @{
 */
#ifndef SPI0_CTRL
#  define SPI0_CTRL FSPI    /**< FSPI is used as SPI_DEV(0) */
#endif
#ifndef SPI0_SCK
#  define SPI0_SCK  GPIO7   /**< FSPI SCK (pin FSPICLK) */
#endif
#ifndef SPI0_MISO
#  define SPI0_MISO GPIO8   /**< FSPI MISO (pin FSPIQ) */
#endif
#ifndef SPI0_MOSI
#  define SPI0_MOSI GPIO9   /**< FSPI MOSI (pin FSPID) */
#endif
#ifndef SPI0_CS0
#  define SPI0_CS0  GPIO3   /**< FSPI CS0 (pin FSPICS0) */
#endif
/** @} */

/**
 * @name   UART configuration
 *
 * ESP32-S3 provides 3 UART interfaces at maximum:
 *
 * UART_DEV(0) uses fixed standard configuration.<br>
 * UART_DEV(1) is not used.<br>
 * UART_DEV(2) is not used.
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
