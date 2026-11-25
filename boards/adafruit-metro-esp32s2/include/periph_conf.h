/*
 * SPDX-FileCopyrightText: 2026 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_adafruit_metro_esp32s2
 * @brief       Peripheral configurations for the Adafruit Metro ESP32-S2 board
 * @{
 *
 * The peripheral configurations in this file can be used for the Adafruit
 * Metro ESP32-S2 board.
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
 * The analog pins A0 ... A5 are mapped to ADC_LINE(0) ... ADC_LINE(5).
 *
 * @note A0 (GPIO17) and A1 (GPIO18) are ADC2 channels which cannot be used
 * while WiFi is active.
 *
 * @note As long as the GPIOs listed in ADC_GPIOS are not initialized as ADC
 * channels with the `adc_init` function, they can be used for other
 * purposes.
 */
#ifndef ADC_GPIOS
#  define ADC_GPIOS   { GPIO17, GPIO18, GPIO1, GPIO2, GPIO3, GPIO4 }
#endif

/**
 * @brief   Declaration of GPIOs that can be used as DAC channels
 *
 * DAC_LINE(0) and DAC_LINE(1) are available at the analog pins A0 and A1.
 *
 * @note As long as the GPIOs listed in DAC_GPIOS are not initialized as DAC
 * channels with the `dac_init` function, they can be used for other
 * purposes.
 */
#ifndef DAC_GPIOS
#  define DAC_GPIOS   { GPIO17, GPIO18 }
#endif
/** @} */

/**
 * @name   I2C configuration
 *
 * Only I2C interface I2C_DEV(0) is used. It is connected to the SDA/SCL pins
 * of the Arduino header and to the STEMMA QT connector.
 *
 * @note The GPIOs listed in the configuration are only initialized as I2C
 * signals when module `periph_i2c` is used. Otherwise they are not
 * allocated and can be used for other purposes.
 *
 * @{
 */
#ifndef I2C0_SPEED
#  define I2C0_SPEED  I2C_SPEED_FAST   /**< I2C bus speed of I2C_DEV(0) */
#endif
#ifndef I2C0_SCL
#  define I2C0_SCL    GPIO34           /**< SCL signal of I2C_DEV(0) */
#endif
#ifndef I2C0_SDA
#  define I2C0_SDA    GPIO33           /**< SDA signal of I2C_DEV(0) */
#endif
/** @} */

/**
 * @name   PWM channel configuration
 *
 * The PWM channels are mapped to the Arduino UNO PWM pins D3, D5, D6, D9,
 * D10 and D11.
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
#  define PWM0_GPIOS  { GPIO8, GPIO10, GPIO11, GPIO14 }
#endif

/**
 * @brief Declaration of the channels for device PWM_DEV(1),
 *        at maximum PWM_CHANNEL_NUM_DEV_MAX.
 */
#ifndef PWM1_GPIOS
#  define PWM1_GPIOS  { GPIO15, GPIO16 }
#endif
/** @} */

/**
 * @name    SPI configuration
 *
 * SPI_DEV(0) uses the FSPI controller and is connected to the SPI (ICSP)
 * header. SPI_DEV(1) uses the HSPI controller and is connected to the
 * Arduino pins D11 (MOSI), D12 (MISO) and D13 (SCK). Both use the Arduino
 * pin D10 as default CS signal.
 *
 * @note SCK of SPI_DEV(1) is connected to the same pin (GPIO42) as the
 * red on-board LED.
 *
 * @note The GPIOs listed in the configuration are first initialized as SPI
 * signals when the corresponding SPI interface is used for the first time
 * by either calling the `spi_init_cs` function or the `spi_acquire`
 * function. That is, they are not allocated as SPI signals before and can
 * be used for other purposes as long as the SPI interface is not used.
 * @{
 */
#ifndef SPI0_CTRL
#  define SPI0_CTRL   FSPI     /**< FSPI is used as SPI_DEV(0) */
#endif
#ifndef SPI0_SCK
#  define SPI0_SCK    GPIO36   /**< FSPI SCK (pin FSPICLK) */
#endif
#ifndef SPI0_MISO
#  define SPI0_MISO   GPIO37   /**< FSPI MISO (pin FSPIQ) */
#endif
#ifndef SPI0_MOSI
#  define SPI0_MOSI   GPIO35   /**< FSPI MOSI (pin FSPID) */
#endif
#ifndef SPI0_CS0
#  define SPI0_CS0    GPIO15   /**< FSPI CS0 (Arduino pin D10) */
#endif

#ifndef SPI1_CTRL
#  define SPI1_CTRL   HSPI     /**< HSPI (SPI Controller 3) is used as SPI_DEV(1) */
#endif
#ifndef SPI1_SCK
#  define SPI1_SCK    GPIO42   /**< HSPI SCK (Arduino pin D13) */
#endif
#ifndef SPI1_MISO
#  define SPI1_MISO   GPIO21   /**< HSPI MISO (Arduino pin D12) */
#endif
#ifndef SPI1_MOSI
#  define SPI1_MOSI   GPIO16   /**< HSPI MOSI (Arduino pin D11) */
#endif
#ifndef SPI1_CS0
#  define SPI1_CS0    GPIO15   /**< HSPI CS0 (Arduino pin D10) */
#endif
/** @} */

/**
 * @name   UART configuration
 *
 * ESP32-S2 provides 2 UART interfaces at maximum:
 *
 * UART_DEV(0) uses the DEBUG TX/RX pins GPIO43 and GPIO44 (default
 *             configuration from `periph_conf_common_esp32s2.h`).<br>
 * UART_DEV(1) uses the Arduino pins D0 (RX, GPIO5) and D1 (TX, GPIO6).
 *
 * @note The stdio is provided via the USB interface of the ESP32-S2 by
 * default (module `stdio_cdc_acm`).
 *
 * @{
 */
#ifndef UART1_TXD
#  define UART1_TXD   GPIO6    /**< direct I/O pin for UART_DEV(1) TxD */
#endif
#ifndef UART1_RXD
#  define UART1_RXD   GPIO5    /**< direct I/O pin for UART_DEV(1) RxD */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/* include common peripheral definitions as last step */
#include "periph_conf_common.h"

/** @} */
