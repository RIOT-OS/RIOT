/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_esp32_wroom-32
 * @brief       Peripheral MCU configuration for generic ESP32-WROOM-32 boards
 * @{
 *
 * This configuration can be used for a large set of ESP32 boards that
 * use an ESP32-WROOM-32 module and simply break out all GPIOs to external
 * pads without having any special hardware or interfaces on-board.
 * Examples are Espressif's EPS32-DEVKIT or NodeMCU-ESP32S and a large
 * number of clones.
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
 * For generic boards, all ADC pins that have broken out are declared as ADC
 * channels.
 *
 * @note As long as the GPIOs listed in ADC_GPIOS are not initialized as ADC
 * channels with the `adc_init` function, they can be used for other
 * purposes.
 */
#ifndef ADC_GPIOS
#define ADC_GPIOS   { GPIO0 , GPIO2 , GPIO4 , GPIO12, GPIO13, GPIO14, \
                      GPIO15, GPIO25, GPIO26, GPIO27, GPIO32, GPIO33, \
                      GPIO34, GPIO35, GPIO36, GPIO39 }
#endif

/**
 * @brief   Declaration of GPIOs that can be used as DAC channels
 *
 * For generic boards the 2 DAC lines GPIO25 and GPIO26 are declared as
 * DAC channels.
 *
 * @note As long as the GPIOs listed in DAC_GPIOS are not initialized as DAC
 * channels with the `dac_init` function, they can be used for other
 * purposes.
 */
#ifndef DAC_GPIOS
#define DAC_GPIOS   { GPIO25, GPIO26 }
#endif
/** @} */

/**
 * @name   I2C configuration
 *
 * For generic boards, only one I2C interface I2C_DEV(0) is defined.
 *
 * The GPIOs listed in the configuration are only initialized as I2C signals
 * when module `periph_i2c` is used. Otherwise they are not allocated and
 * can be used for other purposes.
 *
 * @{
 */
#ifndef I2C0_SPEED
#define I2C0_SPEED  I2C_SPEED_FAST  /**< I2C bus speed of I2C_DEV(0) */
#endif
#ifndef I2C0_SCL
#define I2C0_SCL    GPIO22          /**< SCL signal of I2C_DEV(0) [UEXT1] */
#endif
#ifndef I2C0_SDA
#define I2C0_SDA    GPIO21          /**< SDA signal of I2C_DEV(0) [UEXT1] */
#endif
/** @} */

/**
 * @name   PWM channel configuration
 *
 * For generic boards, two PWM devices are configured. These devices
 * contain all GPIOs that are not defined as I2C, SPI or UART for this board.
 * Generally, all outputs pins could be used as PWM channels.
 *
 * @note As long as the according PWM device is not initialized with
 * the `pwm_init`, the GPIOs declared for this device can be used
 * for other purposes.
 *
 * @{
 */

/**
 * @brief Declaration of the channels for device PWM_DEV(0),
 *        at maximum six channels.
 */
#ifndef PWM0_GPIOS
#define PWM0_GPIOS  { GPIO0, GPIO2, GPIO4, GPIO16, GPIO17 }
#endif

/**
 * @brief Declaration of the channels for device PWM_DEV(1),
 *        at maximum six channels.
 */
#ifndef PWM1_GPIOS
#define PWM1_GPIOS  { GPIO27, GPIO32, GPIO33 }
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
#define SPI0_CTRL   VSPI    /**< VSPI is used as SPI_DEV(0) */
#endif
#ifndef SPI0_SCK
#define SPI0_SCK    GPIO18  /**< VSPI SCK */
#endif
#ifndef SPI0_MISO
#define SPI0_MISO   GPIO19  /**< VSPI MISO */
#endif
#ifndef SPI0_MOSI
#define SPI0_MOSI   GPIO23  /**< VSPI MOSI */
#endif
#ifndef SPI0_CS0
#define SPI0_CS0    GPIO5   /**< VSPI CS0 */
#endif

#ifndef SPI1_CTRL
#define SPI1_CTRL   HSPI    /**< HSPI is used as SPI_DEV(1) */
#endif
#ifndef SPI1_SCK
#define SPI1_SCK    GPIO14  /**< HSPI SCK */
#endif
#ifndef SPI1_MISO
#define SPI1_MISO   GPIO12  /**< HSPI MISO */
#endif
#ifndef SPI1_MOSI
#define SPI1_MOSI   GPIO13  /**< HSPI MOSI */
#endif
#ifndef SPI1_CS0
#define SPI1_CS0    GPIO15  /**< HSPI CS0 */
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

#if CONFIG_FLASHMODE_DOUT || CONFIG_FLASHMODE_DIO || DOXYGEN
#ifndef UART1_TXD
#define UART1_TXD   GPIO10  /**< direct I/O pin for UART_DEV(1) TxD */
#endif
#ifndef UART1_RXD
#define UART1_RXD   GPIO9   /**< direct I/O pin for UART_DEV(1) RxD */
#endif
#else
#warning Configuration problem: Flash mode is qio or qout, \
         GPIO9 and GPIO10 are not available for UART1 as configured
#endif
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common peripheral definitions as last step */
#include "periph_conf_common.h"

/** @} */
