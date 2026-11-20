/*
 * SPDX-FileCopyrightText: 2026 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_adafruit_metro_esp32s3
 * @brief       Peripheral configurations for the Adafruit Metro ESP32-S3 board
 * @{
 *
 * The peripheral configuration follows the Arduino UNO R3 compatible pinout
 * of the board.
 *
 * Any modifications required for specific applications
 * can be overridden by \ref esp32_application_specific_configurations
 * "application-specific board configuration".
 *
 * For detailed information about the peripheral configuration for ESP32-S3
 * boards, see section \ref esp32_peripherals "Common Peripherals".
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
 * ADC_LINE(0) ... ADC_LINE(5) correspond to the Arduino analog pins
 * A0 ... A5. ADC_LINE(6) ... ADC_LINE(17) correspond to the digital
 * pins D2 ... D13.
 *
 * @note A0 ... A4 (GPIO14 ... GPIO18) and D11 ... D13 (GPIO11 ... GPIO13)
 * are ADC2 channels. ADC2 is also used by the WiFi module, so these channels
 * cannot be used while WiFi is active.
 *
 * @note As long as the GPIOs listed in ADC_GPIOS are not initialized as ADC
 * channels with the `adc_init` function, they can be used for other
 * purposes.
 */
#ifndef ADC_GPIOS
#  define ADC_GPIOS   { GPIO14, GPIO15, GPIO16, GPIO17, GPIO18, GPIO1, \
                        GPIO2, GPIO3, GPIO4, GPIO5, GPIO6, GPIO7, \
                        GPIO8, GPIO9, GPIO10, GPIO11, GPIO12, GPIO13 }
#endif
/** @} */

/**
 * @name   I2C configuration
 *
 * I2C_DEV(0) is connected to the SDA/SCL pins of the Arduino header and to
 * the STEMMA QT connector. The on-board MAX17048 battery monitor is
 * connected to this bus with address 0x36.
 *
 * @note The GPIOs listed in the configuration are only initialized as I2C signals
 * when module `periph_i2c` is used. Otherwise they are not allocated and
 * can be used for other purposes.
 *
 * @{
 */
#ifndef I2C0_SPEED
#  define I2C0_SPEED  I2C_SPEED_FAST    /**< I2C bus speed of I2C_DEV(0) */
#endif
#ifndef I2C0_SCL
#  define I2C0_SCL    GPIO48            /**< SCL signal of I2C_DEV(0) */
#endif
#ifndef I2C0_SDA
#  define I2C0_SDA    GPIO47            /**< SDA signal of I2C_DEV(0) */
#endif
/** @} */

/**
 * @name   PWM channel configuration
 *
 * Two PWM devices are configured which use the PWM capable pins of an
 * Arduino UNO R3, i.e., D3, D5, D6, D9, D10 and D11.
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
#  define PWM0_GPIOS  { GPIO3, GPIO5, GPIO6, GPIO9 }
#endif

/**
 * @brief Declaration of the channels for device PWM_DEV(1),
 *        at maximum PWM_CHANNEL_NUM_DEV_MAX.
 */
#ifndef PWM1_GPIOS
#  define PWM1_GPIOS  { GPIO10, GPIO11 }
#endif

/** @} */

/**
 * @name    SPI configuration
 *
 * SPI_DEV(0) is connected to the ICSP header and the microSD card slot.
 * SPI_DEV(1) is connected to the Arduino pins D10 ... D13.
 *
 * @note The GPIOs listed in the configuration are first initialized as SPI
 * signals when the corresponding SPI interface is used for the first time
 * by either calling the `spi_init_cs` function or the `spi_acquire`
 * function. That is, they are not allocated as SPI signals before and can
 * be used for other purposes as long as the SPI interface is not used.
 * @{
 */
#ifndef SPI0_CTRL
#  define SPI0_CTRL   FSPI      /**< FSPI (SPI Controller 2) is used as SPI_DEV(0) */
#endif
#ifndef SPI0_SCK
#  define SPI0_SCK    GPIO39    /**< SCK (ICSP header, microSD card) */
#endif
#ifndef SPI0_MISO
#  define SPI0_MISO   GPIO21    /**< MISO (ICSP header, microSD card) */
#endif
#ifndef SPI0_MOSI
#  define SPI0_MOSI   GPIO42    /**< MOSI (ICSP header, microSD card) */
#endif
#ifndef SPI0_CS0
#  define SPI0_CS0    GPIO45    /**< SD_CS of the microSD card */
#endif

#ifndef SPI1_CTRL
#  define SPI1_CTRL   SPI3_HOST     /**< SPI3 is used as SPI_DEV(1) */
#endif
#ifndef SPI1_SCK
#  define SPI1_SCK    GPIO13    /**< SCK  (Arduino pin D13) */
#endif
#ifndef SPI1_MISO
#  define SPI1_MISO   GPIO12    /**< MISO (Arduino pin D12) */
#endif
#ifndef SPI1_MOSI
#  define SPI1_MOSI   GPIO11    /**< MOSI (Arduino pin D11) */
#endif
#ifndef SPI1_CS0
#  define SPI1_CS0    GPIO10    /**< CS0  (Arduino pin D10) */
#endif
/** @} */

/**
 * @name   UART configuration
 *
 * ESP32-S3 provides 3 UART interfaces at maximum, two are defined by RIOT:
 *
 * UART_DEV(0) uses fixed standard configuration (Debug header).<br>
 * UART_DEV(1) is connected to the Arduino pins D0 (RX) and D1 (TX).
 *
 * @{
 */
#define UART0_TXD   GPIO43  /**< direct I/O pin for UART_DEV(0) TxD, can't be changed */
#define UART0_RXD   GPIO44  /**< direct I/O pin for UART_DEV(0) RxD, can't be changed */

#ifndef UART1_TXD
#  define UART1_TXD   GPIO40    /**< UART_DEV(1) TxD (Arduino pin D1) */
#endif
#ifndef UART1_RXD
#  define UART1_RXD   GPIO41    /**< UART_DEV(1) RxD (Arduino pin D0) */
#endif
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common peripheral definitions as last step */
#include "periph_conf_common.h"

/** @} */
