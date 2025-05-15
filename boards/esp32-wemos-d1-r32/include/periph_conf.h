/*
 * Copyright (C) 2025 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_esp32_wemos_d1_r32
 * @brief       Peripheral MCU configuration for Wemos D1 R32 (ESPDuino-32) board
 * @{
 *
 * The peripheral configuration corresponds to the Arduino Uno pinout and
 * guarantees the compatibility with Arduino Uno Shields.
 *
 * For detailed information about the configuration of ESP32 boards, see
 * section \ref esp32_peripherals "Common Peripherals".
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
 */
#define ADC_GPIOS   { GPIO2, GPIO4, GPIO35, GPIO34, GPIO36, GPIO39 }

/**
 * @brief   Declaration of GPIOs that can be used as DAC channels
 */
#define DAC_GPIOS   { GPIO25, GPIO26 }
/** @} */

/**
 * @name   I2C configuration
 * @{
 */
#ifndef I2C0_SPEED
#  define I2C0_SPEED  I2C_SPEED_FAST    /**< I2C bus speed of I2C_DEV(0) */
#endif

#define I2C0_SCL    GPIO22              /**< SCL signal of I2C_DEV(0) */
#define I2C0_SDA    GPIO21              /**< SDA signal of I2C_DEV(0) */
/** @} */

/**
 * @name   PWM channel configuration
 * @{
 */

/**
 * @brief Declaration of PWM_DEV(0) channels, at maximum six channels.
 */
#define PWM0_GPIOS  { GPIO25, GPIO16, GPIO27, GPIO13, GPIO2 }

/**
 * @brief Declaration of PWM_DEV(1) channels, at maximum six channels.
 *
 * GPIO5 and GPIO23 are also used for SPI_DEV(0) an can only be used as
 * PWM channels if SPI_DEV(0) is not used an vice versa.
 */
#define PWM1_GPIOS  { GPIO5, GPIO23 }
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
#define SPI0_CTRL   VSPI    /**< VSPI is used as SPI_DEV(0) */
#define SPI0_SCK    GPIO18  /**< VSPI SCK */
#define SPI0_MISO   GPIO19  /**< VSPI MISO */
#define SPI0_MOSI   GPIO23  /**< VSPI MOSI */
#define SPI0_CS0    GPIO5   /**< VSPI CS0 */
/** @} */

/**
 * @name   UART configuration
 *
 * UART_DEV(0) uses fixed standard configuration.<br>
 * UART_DEV(1) is defined here.<br>
 *
 * @{
 */
#define UART0_TXD   GPIO1  /**< direct I/O pin for UART_DEV(0) TxD, can't be changed */
#define UART0_RXD   GPIO3  /**< direct I/O pin for UART_DEV(0) RxD, can't be changed */

#define UART1_TXD   GPIO17  /**< direct I/O pin for UART_DEV(1) TxD */
#define UART1_RXD   GPIO16  /**< direct I/O pin for UART_DEV(1) RxD */
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common peripheral definitions as last step */
#include "periph_conf_common.h"

/** @} */
