/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_esp32x
 * @brief       Board definitions that are common for all ESP32x boards.
 *
 * This file contains board configurations that are valid for all ESP32.
 *
 * For detailed information about the configuration of ESP32 boards, see
 * section \ref esp32_peripherals "Common Peripherals".
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef BOARD_COMMON_H
#define BOARD_COMMON_H

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"
#if MODULE_ARDUINO
#include "arduino_pinmap.h"
#endif

#include "periph/gpio.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

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

/**
 * @brief   MTD drive start address in SPI flash memory
 *
 * Defines the start address of the MTD system device in the SPI
 * flash memory. It can be overridden by \ref esp32_application_specific_configurations
 * "application-specific board configuration"
 *
 * If the MTD start address is not defined or is 0, the first possible
 * multiple of 0x100000 (1 MByte) is used in free SPI flash memory,
 * which was determined from the partition table.
 */
#ifndef SPI_FLASH_DRIVE_START
#define SPI_FLASH_DRIVE_START  0
#endif

#define MTD_0 mtd_dev_get(0)          /**< MTD device for the internal Flash */

#if MODULE_MTD_SDCARD_DEFAULT || DOXYGEN
#define MTD_1 mtd_dev_get(1)          /**< MTD device for the SD Card */
#elif MODULE_MTD_SDMMC_DEFAULT
#define MTD_1 mtd_dev_get(1)          /**< MTD device for the SD/MMC Card */
#endif /* MODULE_MTD_SDCARD_DEFAULT || DOXYGEN */

/**
 * @brief   Default MTD offset for SPI SD Card interfaces
 *
 * mtd1 is used for SPI SD Cards by default if module `mtd_sdcard_default`
 * is used.
 */
#ifndef CONFIG_SDCARD_GENERIC_MTD_OFFSET
#define CONFIG_SDCARD_GENERIC_MTD_OFFSET    1
#endif

/**
 * @brief   Default MTD offset for SD/MMC interfaces
 *
 * mtd1 is used for SD/MMCs by default if module `mtd_sdmmc_default`
 * is used.
 */
#ifndef CONFIG_SDMMC_GENERIC_MTD_OFFSET
#define CONFIG_SDMMC_GENERIC_MTD_OFFSET    1
#endif

/** @} */
#endif /* MODULE_MTD || DOXYGEN */

/**
  * @brief Print the board configuration in a human readable format
  */
void print_board_config(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
#endif /* BOARD_COMMON_H */
