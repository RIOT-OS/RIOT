/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32_esp-wrover-kit
 * @brief       Peripheral MCU configuration for Espressif ESP-WROVER-KIT V3
 * @{
 *
 * The Espressif ESP-WROVER-KIT is a development board that uses the
 * ESP32-WROVER module which includes a built-in 4 MByte SPI RAM. Most
 * important features of the board are
 *
 * - Micro-SD card interface
 * - OV7670 camera interface
 * - 3.2" SPI LCD panel
 * - RGB LED
 *
 * Furthermore, many GPIOs are broken out for extension. The USB bridge
 * based on FDI FT2232HL provides a JTAG interface for debugging through
 * the USB interface.
 *
 * When the camera module is connected, add
 * ```
 * USEMODULE += esp32_wrover_kit_camera
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
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
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
 * When the camera is connected there are no GPIOs left that could be used
 * as ADC channels.
 *
 * @note As long as the GPIOs listed in ADC_GPIOS are not initialized as ADC
 * channels with the ```adc_init``` function, they can be used for other
 * purposes.
 */
#ifndef ADC_GPIOS
#ifndef MODULE_ESP32_WROVER_KIT_CAMERA
#define ADC_GPIOS   { GPIO34, GPIO35, GPIO36, GPIO39 }
#else
#define ADC_GPIOS   { }
#endif
#endif

/**
 * @brief   Declaration of GPIOs that can be used as DAC channels
 *
 * ESP-WROVER-KIT has no GPIOs left that might be used as DAC channels.
 */
#ifndef DAC_GPIOS
#define DAC_GPIOS   { }
#endif
/** @} */


/**
 * @name   I2C configuration
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
#ifndef I2C0_SCL
#define I2C0_SCL    GPIO27          /**< SCL signal of I2C_DEV(0) [UEXT1] */
#endif
#ifndef I2C0_SDA
#define I2C0_SDA    GPIO26          /**< SDA signal of I2C_DEV(0) [UEXT1] */
#endif
/** @} */

/**
 * @name   PWM channel configuration
 *
 * LEDs are used as PWM channels for device PWM_DEV(0).
 *
 * @note As long as the according PWM device is not initialized with function
 * pwm_init, the GPIOs declared for this device can be used for other
 * purposes.
 *
 * @note As long as the according PWM device is not initialized with
 * the ```pwm_init```, the GPIOs declared for this device can be used
 * for other purposes.
 *
 * @{
 */
#ifndef PWM0_GPIOS
#if !MODULE_ESP32_WROVER_KIT_CAMERA || DOXYGEN
#define PWM0_GPIOS  { LED0_PIN, LED2_PIN } /**< only available when camera is not connected */
#else
#define PWM0_GPIOS  { }
#endif
#endif

/** PWM_DEV(1) is not used */
#ifndef PWM1_GPIOS
#define PWM1_GPIOS  { }
#endif
/** @} */


/**
 * @name    SPI configuration
 *
 * SPI configuration depends on configured/connected components.
 *
 * HSPI is is always available and therefore used as SPI_DEV(0)
 * VSPI is only available when the camera is not plugged.
 *
 * @{
 */

#ifndef SPI0_DEV
#define SPI0_DEV    HSPI    /**< HSPI is configured as SPI_DEV(0) */
#endif

#ifndef SPI0_SCK
#define SPI0_SCK    GPIO14  /**< SD Card SCL */
#endif
#ifndef SPI0_MOSI
#define SPI0_MOSI   GPIO15  /**< SD Card MOSI */
#endif
#ifndef SPI0_MISO
#define SPI0_MISO   GPIO2   /**< SD Card MISO */
#endif
#ifndef SPI0_CS0
#define SPI0_CS0    GPIO13  /**< SD Card CS */
#endif

#if !MODULE_ESP32_WROVER_KIT_CAMERA || DOXYGEN
/**
 * When the camera is not connected, VSPI is configured and becomes SPI_DEV(1).
 *
 * @note The GPIOs listed in the configuration are first initialized as SPI
 * signals when the corresponding SPI interface is used for the first time
 * by either calling the ```spi_init_cs``` function or the ```spi_acquire```
 * function. That is, they are not allocated as SPI signals before and can
 * be used for other purposes as long as the SPI interface is not used.
 *
 */
#ifndef SPI1_DEV
#define SPI1_DEV    VSPI
#endif

#ifndef SPI1_SCK
#define SPI1_SCK    GPIO19  /**< VSPI SCK used as LCD SCL, can be used to connect peripherals */
#endif
#ifndef SPI1_MOSI
#define SPI1_MOSI   GPIO23  /**< VSPI MOSI used as LCD MOSI, can be used to connect peripherals */
#endif
#ifndef SPI1_MISO
#define SPI1_MISO   GPIO25  /**< VSPI MISO used as LCD MISO, can be used to connect peripherals */
#endif
#ifndef SPI1_CS0
#define SPI1_CS0    GPIO22  /**< VSPI CS0 used as LCD CS, can be used to connect peripherals */
#endif
#endif /* !MODULE_ESP32_WROVER_KIT_CAMERA || DOXYGEN */
/** @} */

/**
 * @name   UART configuration
 *
 * ESP32 provides 3 UART interaces at maximum:
 *
 * UART_DEV(0) uses fixed standard configuration.<br>
 * UART_DEV(1) is not available.<br>
 * UART_DEV(2) is not available.<br>
 * @{
 */
#define UART0_TXD   GPIO10 /**< direct I/O pin for UART_DEV(0) TxD, can't be changed */
#define UART0_RXD   GPIO9  /**< direct I/O pin for UART_DEV(0) RxD, can't be changed */
/** @} */


#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common board definitions as last step */
#include "periph_conf_common.h"

#endif /* PERIPH_CONF_H */
/** @} */
