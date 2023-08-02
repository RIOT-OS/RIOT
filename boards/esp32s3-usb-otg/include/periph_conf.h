/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32s3_usb_otg
 * @brief       Peripheral configurations for ESP32-S3-USB-OTG boards
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
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include <stdint.h>

#include "board.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ADC channel configuration
 * @{
 */
/**
 * @brief   Declaration of GPIOs that can be used as ADC channels
 *
 * GPIO1 can be used for USB_DEV voltage monitoring as ADC_LINE(0).
 * GPIO2 can be used for Battery voltage monitoring as ADC_LINE(1).
 */
#ifndef ADC_GPIOS
#define ADC_GPIOS   { GPIO1, GPIO2 }
#endif
/** @} */

/**
 * @name   PWM channel configuration
 * @{
 */

/**
 * @brief Declaration of the channels for device PWM_DEV(0)
 *
 * LED pins are used as PWM channels.
 */
#ifndef PWM0_GPIOS
#define PWM0_GPIOS  { LED0_PIN, LED1_PIN }
#endif
/** @} */

/**
 * @name    SPI configuration
 *
 * SPI_DEV(0) is used for LCD display
 * SPI_DEV(1) is used for SPI SD Card (only configured if `sdcard_spi` is enabled)
 * @{
 */
#ifndef SPI0_CTRL
#define SPI0_CTRL   SPI2_HOST   /**< FSPI is configured as SPI_DEV(0) */
#endif
#ifndef SPI0_SCK
#define SPI0_SCK    GPIO6       /**< LCD SCLK */
#endif
#ifndef SPI0_MOSI
#define SPI0_MOSI   GPIO7       /**< LCD SDA */
#endif
#ifndef SPI0_MISO
#define SPI0_MISO   GPIO3       /**< not used (dummy pin) */
#endif
#ifndef SPI0_CS0
#define SPI0_CS0    GPIO5       /**< LCD CS */
#endif

#if IS_USED(MODULE_SDCARD_SPI)
#ifndef SPI1_CTRL
#define SPI1_CTRL   SPI3_HOST   /**< FSPI is configured as SPI_DEV(0) */
#endif
#ifndef SPI1_SCK
#define SPI1_SCK    GPIO36       /**< SD SCLK */
#endif
#ifndef SPI1_MOSI
#define SPI1_MOSI   GPIO35       /**< SD MOSI */
#endif
#ifndef SPI1_MISO
#define SPI1_MISO   GPIO37       /**< SD MISO */
#endif
#ifndef SPI1_CS0
#define SPI1_CS0    GPIO34       /**< SD CS */
#endif
#endif
/** @} */

/**
 * @name   UART configuration
 *
 * ESP32-S3 provides 3 UART interfaces at maximum:
 *
 * UART_DEV(0) uses fixed standard configuration.<br>
 * UART_DEV(1) is not used.<br>
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

#endif /* PERIPH_CONF_H */
/** @} */
