/*
 * Copyright (C) 2018 Gunar Schorcht
 * Copyright (C) 2020 Google LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32_esp-ethernet-kit
 * @brief       Peripheral MCU configuration for Espressif ESP32-Ethernet-Kit
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Erik Ekman <eekman@google.com>
 * @{
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
 * channels with the `adc_init` function, they can be used for other
 * purposes.
 */
#ifndef ADC_GPIOS
#define ADC_GPIOS   { GPIO34, GPIO35, GPIO36, GPIO39 }
#endif
/** @} */

/**
 * @name    Declaration of GPIOs that can be used as DAC channels
 *
 * ESP32-Ethernet-Kit has no GPIOs left that might be used as DAC channels.
 * @{
 */
#ifndef DAC_GPIOS
#define DAC_GPIOS   { }
#endif
/** @} */

/**
 * @name   I2C configuration
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
#define I2C0_SCL    GPIO33          /**< SCL signal of I2C_DEV(0) */
#endif
#ifndef I2C0_SDA
#define I2C0_SDA    GPIO32          /**< SDA signal of I2C_DEV(0) */
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
#ifndef PWM0_GPIOS
#ifdef BOARD_ESP32_ETHERNET_KIT_V1_0
#define PWM0_GPIOS  { GPIO4 }
#else
#define PWM0_GPIOS  { GPIO2, GPIO4 }
#endif
#endif /* PWM0_GPIOS */

/** @} */

/**
 * @name    SPI configuration
 *
 * SPI configuration depends on configured/connected components.
 *
 * HSPI is only available when all JTAG pins are disabled.
 *
 * @{
 */
#ifndef MODULE_ESP_JTAG
#define SPI0_CTRL   HSPI    /**< HSPI is used as SPI_DEV(0) */
#define SPI0_SCK    GPIO14  /**< HSPI SCK */
#define SPI0_MISO   GPIO12  /**< HSPI MISO */
#define SPI0_MOSI   GPIO13  /**< HSPI MOSI */
#define SPI0_CS0    GPIO15  /**< HSPI CS0 */
#endif /* MODULE_ESP_JTAG not defined */
/** @} */

/**
 * @name   UART configuration
 *
 * ESP32 provides 3 UART interfaces at maximum:
 *
 * UART_DEV(0) uses fixed standard configuration.<br>
 * UART_DEV(1) is not available.<br>
 * UART_DEV(2) is not available.<br>
 * @{
 */
#define UART0_TXD   GPIO1  /**< direct I/O pin for UART_DEV(0) TxD, can't be changed */
#define UART0_RXD   GPIO3  /**< direct I/O pin for UART_DEV(0) RxD, can't be changed */
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common board definitions as last step */
#include "periph_conf_common.h"

#endif /* PERIPH_CONF_H */
/** @} */
