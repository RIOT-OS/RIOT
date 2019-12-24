/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_esp32
 * @brief       Board definitions that are common for all ESP32 boards.
 *
 * This file contains board configurations that are valid for all ESP32.
 *
 * For detailed information about the configuration of ESP32 boards, see
 * section \ref esp32_comm_periph "Common Peripherals".
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef BOARD_COMMON_H
#define BOARD_COMMON_H

/* not required when compiling ESP32 vendor code parts */
#ifndef ESP32_IDF_CODE

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"
#include "arduino_pinmap.h"

#include "periph/gpio.h"
#include "sdk_conf.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name   External clock crystal frequency (MHz)
 *
 * Most boards use a 40MHz crystal, including all those based on Espressif's
 * WROOM-32 and WROVER-32 modules. Those that don't, like the SparkFun ESP32
 * Thing (26MHz), must define ESP32_XTAL_FREQ appropriately in their board
 * configuration.
 *
 * An obvious side effect of a mismatch is that the UART won't sync and the
 * development terminal will show garbage instead of log output.
 * @{
 */
#ifndef ESP32_XTAL_FREQ
#define ESP32_XTAL_FREQ             (40)
#endif
/** @} */

/**
 * @name   LED configuration (three predefined LEDs at maximum)
 *
 * @note LEDx_ACTIVE value must be declared in board configuration.
 * @{
 */
#if defined(LED0_PIN) || DOXYGEN
#define LED0_MASK       (BIT(LED0_PIN))
#define LED0_ON         (gpio_write(LED0_PIN,  LED0_ACTIVE))
#define LED0_OFF        (gpio_write(LED0_PIN, !LED0_ACTIVE))
#define LED0_TOGGLE     (gpio_toggle(LED0_PIN))
#endif

#if defined(LED1_PIN) || DOXYGEN
#define LED1_MASK       (BIT(LED1_PIN))
#define LED1_ON         (gpio_write(LED1_PIN,  LED1_ACTIVE))
#define LED1_OFF        (gpio_write(LED1_PIN, !LED1_ACTIVE))
#define LED1_TOGGLE     (gpio_toggle(LED1_PIN))
#endif

#if defined(LED2_PIN) || DOXYGEN
#define LED2_MASK       (BIT(LED2_PIN))
#define LED2_ON         (gpio_write(LED2_PIN,  LED2_ACTIVE))
#define LED2_OFF        (gpio_write(LED2_PIN, !LED2_ACTIVE))
#define LED2_TOGGLE     (gpio_toggle(LED2_PIN))
#endif
/** @} */


/**
 * @name    STDIO configuration
 * @{
 */
/**< Default baudrate of UART for stdio */
#ifndef STDIO_UART_BAUDRATE
#define STDIO_UART_BAUDRATE      (115200)
#endif
/** @} */


#if MODULE_MTD || DOXYGEN
/**
 * @name    MTD system drive configuration
 *
 * Built-in SPI flash memory is used as MTD system drive.
 * @{
 */
#include "mtd.h"

/**
 * @brief   MTD drive start address in SPI flash memory
 *
 * Defines the start address of the MTD system device in the SPI
 * flash memory. It can be overridden by \ref esp32_app_spec_conf
 * "application-specific board configuration"
 *
 * If the MTD start address is not defined or is 0, the first possible
 * multiple of 0x100000 (1 MByte) is used in free SPI flash memory,
 * which was determined from the partition table.
 */
#ifndef SPI_FLASH_DRIVE_START
#define SPI_FLASH_DRIVE_START  0
#endif

/** Default MTD drive definition */
#define MTD_0 mtd0

/** Pointer to the default MTD drive structure */
extern mtd_dev_t *mtd0;

/** @} */
#endif /* MODULE_MTD || DOXYGEN */


#if MODULE_SPIFFS || DOXYGEN
/**
 * @name    SPIFFS configuration for the system MTD device
 *
 * Configuration of the SPIFFS that can be used on the system MTD device.
 * @{
 */
#define SPIFFS_ALIGNED_OBJECT_INDEX_TABLES 1
#define SPIFFS_READ_ONLY 0
#define SPIFFS_SINGLETON 0
#define SPIFFS_HAL_CALLBACK_EXTRA 1
#define SPIFFS_CACHE 1
/** @} */
#endif /* MODULE_SPIFFS || DOXYGEN */


/**
 * @brief Initialize board specific hardware
 *
 * Since all features of ESP32 boards are provided by the SOC, almost all
 * initializations are done during the CPU initialization that is called from
 * boot loader.
 */
void board_init (void);

/**
  * @brief Print the board configuration in a human readable format
  */
void print_board_config (void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* ESP32_IDF_CODE */
#endif /* BOARD_COMMON_H */
/** @} */
