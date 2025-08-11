/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_arduino_nano_esp32
 * @brief       Peripheral configurations for Arduino Nano ESP32 boards
 * @{
 *
 * The peripheral configurations in this file can be used for the
 * Arduino Nano ESP32 board.
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
 * @name    ADC and DAC channel configuration
 * @{
 */
/**
 * @brief   Declaration of GPIOs that can be used as ADC channels
 *
 * If the `periph_i2c` module is used, two GPIOs connected to ADC2 (GPIO11 and
 * GPIO12) are used as I2C signals. To avoid conflicts, only the GPIOs
 * connected to ADC1 are defined as ADC lines in this case. Otherwise,
 * GPIOs connected to ADC1 and ADC2 are defined as ADC lines.
 *
 * Please keep in mind that GPIO3 is a strapping pin if `EFUSE_STRAP_JTAG_SEL`
 * is set, see \ref esp32_gpio_pins_esp32s3_strapping.
 *
 * @note As long as the GPIOs listed in ADC_GPIOS are not initialized as ADC
 * channels with the `adc_init` function, they can be used for other
 * purposes.
 */
#ifndef ADC_GPIOS
#  if !MODULE_PERIPH_I2C
#    define ADC_GPIOS   { GPIO1, GPIO2, GPIO3, GPIO4, GPIO11, GPIO12, GPIO13, GPIO14 }
#  else /* !MODULE_PERIPH_I2C */
#    define ADC_GPIOS   { GPIO1, GPIO2, GPIO3, GPIO4 }
#  endif /* !MODULE_PERIPH_I2C */
#endif
/** @} */

/**
 * @name   I2C configuration
 *
 * For generic boards, only one I2C interface I2C_DEV(0) is defined.
 *
 * @note The GPIOs listed in the configuration are only initialized as I2C
 * signals when module `periph_i2c` is used. Otherwise they are not allocated
 * and can be used for other purposes.
 *
 * @{
 */
#ifndef I2C0_SPEED
#  define I2C0_SPEED    I2C_SPEED_FAST  /**< I2C bus speed of I2C_DEV(0) */
#endif

#ifndef I2C0_SCL
#  define I2C0_SCL      GPIO12           /**< SCL signal of I2C_DEV(0) */
#endif

#ifndef I2C0_SDA
#  define I2C0_SDA      GPIO11           /**< SDA signal of I2C_DEV(0) */
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
 *        at maximum eight channels (PWM_CH_NUMOF_MAX).
 *
 * Classic Arduino pins with PWM capability that are not configured as SPI
 * pins are defined as PWM channels of PWM_DEV(0). These are the Arduino pins
 * D3, D5, D6, and D9.
 */
#ifndef PWM0_GPIOS
#  define PWM0_GPIOS  { GPIO6, GPIO8, GPIO9, GPIO18 }
#endif

/**
 * @brief Declaration of the channels for device PWM_DEV(1),
 *        at maximum eight channels (PWM_CH_NUMOF_MAX).
 *
 * GPIOs connected to the RGB LED are defined as PWM channels of PWM_DEV(1)
 * as follows:
 * - PWM1:0 - Red LED (GPIO46)
 * - PWM1:1 - Green LED (GPIO0)
 * - PWM1:2 - Blue LED (GPIO45)
 */
#ifndef PWM1_GPIOS
#  define PWM1_GPIOS  { GPIO46, GPIO0, GPIO45 }
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
#  define SPI0_SCK  GPIO48  /**< Routed vio the GPIO matrix to FSPI CLK signal */
#endif

#ifndef SPI0_MISO
#  define SPI0_MISO GPIO47  /**< Routed vio the GPIO matrix to FSPI MISO signal */
#endif

#ifndef SPI0_MOSI
#  define SPI0_MOSI GPIO38  /**< Routed vio the GPIO matrix to FSPI MOSI signal */
#endif

#ifndef SPI0_CS0
#  define SPI0_CS0  GPIO21  /**< Routed vio the GPIO matrix to FSPI CS0 signal */
#endif
/** @} */

/**
 * @name   UART configuration
 *
 * ESP32-S3 provides 3 UART interfaces at maximum:
 *
 * UART_DEV(0) uses fixed standard configuration.<br>
 * UART_DEV(1) is not used.<br>
 * UART_DEV(2) is not used.<br>
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
