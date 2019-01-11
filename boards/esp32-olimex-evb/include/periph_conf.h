/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32_olimex-esp32-evb
 * @brief       Peripheral MCU configuration for Olimex ESP32-EVB (ESP32-GATEWAY)
 * @{
 *
 * This configuration is for the Olimex ESP32-EVB, but can also be used
 * for the Olimex ESP32-GATEWAY. To use this board definition with Olimex
 * ESP32-GATEWAY, add
 * ```
 * USEMODULE += olimex_esp32_gateway
 * ```
 * to the makefile of the application to use the according default board
 * configuration.
 *
 * For detailed information about the configuration of ESP32 boards, see
 * section \ref esp32_comm_periph "Common Peripherals".
 *
 * @note
 * Most definitions can be overridden by an \ref esp32_app_spec_conf
 * "application-specific board configuration".
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ADC and DAC channel configuration
 *
 * @note As long as the GPIOs listed in ADC_GPIOS are not initialized as ADC
 * channels with the ```adc_init``` function, they can be used for other
 *
 * purposes.
 * @{
 */
/**
 * @brief   Declaration of GPIOs that can be used as ADC channels
 *
 * @note As long as the GPIOs listed in ADC_GPIOS are not initialized as ADC
 * channels with the ```adc_init``` function, they can be used for other
 * purposes.
 */
#ifndef ADC_GPIOS
#if MODULE_OLIMEX_ESP32_GATEWAY
#define ADC_GPIOS { GPIO32, GPIO35, GPIO36, GPIO39 }
#else /* MODULE_OLIMEX_ESP32_GATEWAY */
#define ADC_GPIOS { }
#endif /* MODULE_OLIMEX_ESP32_GATEWAY */
#endif /* ADC_GPIOS */

/** Olimex ESP32-EVB has no GPIOs left that might be used as DAC channels. */
#ifndef DAC_GPIOS
#define DAC_GPIOS { }
#endif
/** @} */

/**
 * @name    CAN configuration
 *
 * Olimex ESP32-EVB has a CAN interface including a CAN tranceiver on board.
 */
#ifndef MODULE_OLIMEX_ESP32_GATEWAY
#define CAN_TX      GPIO5          /**< CAN TX tranceiver signal */
#define CAN_RX      GPIO35         /**< CAN RX tranceiver signal */
#endif

/**
 * @name    I2C configuration
 *
 * Olimex ESP32-EVB/GATEWAY have one I2C interface ```I2C_DEV(0)```. However,
 * they use different GPIOs. Olimex ESP32-EVB, the interface is also available
 * at the [UEXT] connector.
 *
 * @note The GPIOs listed in the configuration are only initialized as I2C
 * signals when module ```perpih_i2c``` is used. Otherwise they are not
 * allocated and can be used for other purposes.
 *
 * @{
 */
#ifndef I2C0_SPEED
#define I2C0_SPEED  I2C_SPEED_FAST  /**< I2C bus speed of I2C_DEV(0) */
#endif

#ifdef MODULE_OLIMEX_ESP32_GATEWAY
#ifndef I2C0_SCL
#define I2C0_SCL    GPIO16          /**< SCL signal of I2C_DEV(0) [UEXT] */
#endif
#ifndef I2C0_SDA
#define I2C0_SDA    GPIO17          /**< SDA signal of I2C_DEV(0) [UEXT] */
#endif
#else /* MODULE_OLIMEX_ESP32_GATEWAY */
#ifndef I2C0_SCL
#define I2C0_SCL    GPIO16          /**< SCL signal of I2C_DEV(0) */
#endif
#ifndef I2C0_SDA
#define I2C0_SDA    GPIO13          /**< SDA signal of I2C_DEV(0) */
#endif
#endif /* MODULE_OLIMEX_ESP32_GATEWAY */

/** @} */

/**
 * @name    PWM channel configuration
 *
 * @note As long as the according PWM device is not initialized with
 * the ```pwm_init```, the GPIOs declared for this device can be used
 * for other purposes.
 *
 * @{
 */
/**
 * In DOUT and DIO flash mode, GPIO9 and GIO10 are available and can be used
 * as PWM channels with ```PWM_DEV(0)```.
 */
#ifndef PWM0_GPIOS
#if FLASH_MODE_DOUT || FLASH_MODE_DIO || DOXYGEN
#define PWM0_GPIOS { GPIO9, GPIO10 }
#else
#error Configuration problem: Flash mode qio or qout is used, \
       GPIO9 and GPIO10 cannot be used as PWM channels as configured
#define PWM0_GPIOS { }
#endif
#endif

/** PWM_DEV(1) is not used */
#ifndef PWM1_GPIOS
#define PWM1_GPIOS { }
#endif
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
/**
 * @brief   HSPI is used as SPI_DEV(0)
 *
 * It is available at the [UEXT] connector on Olimex ESP32-EVB.
 *
 * Although the SD card interface of the Olimex ESP32-EVB is also available at
 * the ```SPI_DEV(0)``` interface, it does not have a CS signal. Therefore,
 * it cannot be used in SPI mode with the ```sdcard_spi``` module. Olimex
 * ESP32-GATEWAY uses the integrated SD card interface with another GPIO for
 * the CS signal.
 *
 * @note The GPIOs listed in the configuration are first initialized as SPI
 * signals when the corresponding SPI interface is used for the first time
 * by either calling the ```spi_init_cs``` function or the ```spi_acquire```
 * function. That is, they are not allocated as SPI signals before and can
 * be used for other purposes as long as the SPI interface is not used.
 */
#ifndef SPI0_CTRL
#define SPI0_CTRL   HSPI
#endif

#ifndef SPI0_SCK
#define SPI0_SCK    GPIO14  /**< SCK [UEXT] / SD card interface] */
#endif
#ifndef SPI0_MISO
#define SPI0_MISO   GPIO2   /**< MISO [UEXT] / SD card interface] */
#endif
#ifndef SPI0_MOSI
#define SPI0_MOSI   GPIO15  /**< MOSI [UEXT] / SD Card interface] */
#endif

#ifndef SPI0_CS0
#ifndef MODULE_OLIMEX_ESP32_GATEWAY
#define SPI0_CS0    GPIO17  /**< CS0 [UEXT] */
#else /* MODULE_OLIMEX_ESP32_GATEWAY */
#define SPI0_CS0    GPIO13  /**< CS0 SD Card interface */
#endif /* MODULE_OLIMEX_ESP32_GATEWAY */
#endif /* SPI0_CS0 */

/** @} */

/**
 * @name   UART configuration
 *
 * ESP32 provides 3 UART interaces at maximum:
 *
 * UART_DEV(0) uses fixed standard configuration.<br>
 * UART_DEV(1) is defined here.<br>
 * UART_DEV(2) is not used.<br>
 *
 * If the board definition is used for Olimex EVB-GATEWAY, the UART_DEV(1)
 * interface is not available.
 *
 * @{
 */
#define UART0_TXD   GPIO1   /**< direct I/O pin for UART_DEV(0), can't be changed */
#define UART0_RXD   GPIO3   /**< direct I/O pin for UART_DEV(0), can't be changed */

#ifndef UART1_TXD
#define UART1_TXD   GPIO4   /**< UART_DEV(1) TxD */
#endif
#ifndef UART1_RXD
#define UART1_RXD   GPIO36  /**< UART_DEV(1) RxD */
#endif
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common peripheral definitions as last step */
#include "periph_conf_common.h"

#endif /* PERIPH_CONF_H */
/** @} */
