/*
 * Copyright (C) 2019 Yegor Yefremov
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32_ttgo-t-beam
 * @brief       Peripheral MCU configuration for TTGO T-Beam board
 * @{
 *
 * TTGO T-Beam is an ESP32 development board with 4 MB Flash that
 * uses the EPS32 chip directly. It integrates a SemTech SX1276 or SX1278 for
 * LoRaWAN communication in the 433 MHz or the 868/915 MHz band, respectively.
 * Additionally, it has an GPS receiver connected via UART on board.
 *
 * For detailed information about the configuration of ESP32 boards, see
 * section \ref esp32_comm_periph "Common Peripherals".
 *
 * @note
 * Most definitions can be overridden by an \ref esp32_app_spec_conf
 * "application-specific board configuration".
 *
 * @file
 * @author      Yegor Yefremov <yegorslists@googlemail.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

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
 * @note As long as the GPIOs listed in ADC_GPIOS are not initialized as ADC
 * channels with the \ref adc_init function, they can be used for other
 * purposes.
 */
#ifndef ADC_GPIOS
#if MODULE_ESP32_TTGO_T_BEAM_V1_0
#define ADC_GPIOS   { GPIO0, GPIO2, GPIO4, GPIO13, GPIO25, \
                      GPIO32, GPIO33, GPIO35 }
#else
#define ADC_GPIOS   { GPIO0, GPIO2, GPIO4, GPIO13, GPIO25, \
                      GPIO32, GPIO33, GPIO34, GPIO35 }
#endif
#endif

/**
 * @brief   Declaration of GPIOs that can be used as DAC channels
 *
 * @note As long as the GPIOs listed in DAC_GPIOS are not initialized as DAC
 * channels with the \ref dac_init function, they can be used for other
 * purposes.
 *
 * Since all DAC GPIOs are used for board control functions, there are no
 * GPIOs left that can be used as DAC channels.
 */
#ifndef DAC_GPIOS
#define DAC_GPIOS   { GPIO25 }
#endif
/** @} */

/**
 * @name   I2C configuration
 *
 * Only I2C interface I2C_DEV(0) is used.
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
#define I2C0_SCL    GPIO22          /**< SCL signal of I2C_DEV(0) */
#endif
#ifndef I2C0_SDA
#define I2C0_SDA    GPIO21          /**< SDA signal of I2C_DEV(0) */
#endif
/** @} */

/**
 * @name   PWM channel configuration
 *
 * @note As long as the according PWM device is not initialized with
 * the `pwm_init`, the GPIOs declared for this device can be used
 * for other purposes.
 *
 * @{
 */
/** PWM channels for device PWM_DEV(0) */
#ifndef PWM0_GPIOS
#define PWM0_GPIOS  { GPIO14, GPIO0, GPIO2, GPIO25 }
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
 *
 * @{
 */
#ifndef SPI0_CTRL
#define SPI0_CTRL   VSPI    /**< VSPI is used as SPI_DEV(0) */
#endif
#ifndef SPI0_SCK
#define SPI0_SCK    GPIO5   /**< VSPI SCK */
#endif
#ifndef SPI0_MISO
#define SPI0_MISO   GPIO19  /**< VSPI MISO */
#endif
#ifndef SPI0_MOSI
#define SPI0_MOSI   GPIO27  /**< VSPI MOSI */
#endif
#ifndef SPI0_CS0
#define SPI0_CS0    GPIO18  /**< VSPI CS0 */
#endif
/** @} */

/**
 * @name   UART configuration
 *
 * ESP32 provides 3 UART interfaces at maximum:
 *
 * UART_DEV(0) uses fixed standard configuration.<br>
 * UART_DEV(1) is defined here.<br>
 * UART_DEV(2) is not used.<br>
 *
 * @{
 */
#define UART0_TXD   GPIO1  /**< direct I/O pin for UART_DEV(0) TxD, can't be changed */
#define UART0_RXD   GPIO3  /**< direct I/O pin for UART_DEV(0) RxD, can't be changed */

#ifndef UART1_TXD
#if MODULE_ESP32_TTGO_T_BEAM_V1_0
#define UART1_TXD   GPIO12  /**< direct I/O pin for UART_DEV(1) TxD */
#else
#define UART1_TXD   GPIO15  /**< direct I/O pin for UART_DEV(1) TxD */
#endif /* MODULE_ESP32_TTGO_T_BEAM_V1_0 */
#endif
#ifndef UART1_RXD
#if MODULE_ESP32_TTGO_T_BEAM_V1_0
#define UART1_RXD   GPIO34  /**< direct I/O pin for UART_DEV(1) RxD */
#else
#define UART1_RXD   GPIO12  /**< direct I/O pin for UART_DEV(1) RxD */
#endif /* MODULE_ESP32_TTGO_T_BEAM_V1_0 */
#endif
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common board definitions as last step */
#include "periph_conf_common.h"

#endif /* PERIPH_CONF_H */
/** @} */
