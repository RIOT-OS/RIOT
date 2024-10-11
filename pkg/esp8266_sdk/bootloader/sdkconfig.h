/*
 * Copyright (C) 2021 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_esp8266_sdk
 * @{
 *
 * @file
 * @brief       RIOT-OS modification of the bootloader SDK configuration
 *
 * The bootloader build of the ESP8266 SDK needs some settings from the SDK
 * configuration. These are normally generated by the menuconfig in the vendor
 * SDK.
 *
 * Some of these parameters are configurable by the application. For example,
 * the UART baudrate used by the console and the verbose level of the
 * bootloader.
 *
 * @author      iosabi <iosabi@protonmail.com>
 */

#ifndef SDKCONFIG_H
#define SDKCONFIG_H

#if !DOXYGEN

#include "riotbuild.h"

#include "esp8266_idf_version.h"
#include "sdkconfig_default.h"

#ifdef __cplusplus
extern "C" {
#endif

#if MODULE_ESP_LOG_COLORED
#define CONFIG_LOG_COLORS   1
#endif

#ifndef CONFIG_LOG_BOOTLOADER_LEVEL
/*
 * SDK Log levels:
 *
 *  0 = NONE
 *  1 = ERROR
 *  2 = WARN
 *  3 = INFO
 *  4 = DEBUG
 *  5 = VERBOSE
 */
#if MODULE_ESP_LOG_STARTUP
#define CONFIG_LOG_BOOTLOADER_LEVEL     3 /* INFO */
#else
#define CONFIG_LOG_BOOTLOADER_LEVEL     0 /* NONE */
#endif
#endif

/*
 * Bootloader output baudrate, defined by the app settings as BAUD or
 * BOOTLOADER_BAUD.
 */
#ifndef CONFIG_CONSOLE_UART_BAUDRATE
#define CONFIG_CONSOLE_UART_BAUDRATE    (RIOT_BOOTLOADER_BAUD)
#endif

/*
 * The makefile FLASH_SIZE value is set in MB, but set as bytes to
 * CONFIG_SPI_FLASH_SIZE.
 */
#ifndef CONFIG_SPI_FLASH_SIZE
#define CONFIG_SPI_FLASH_SIZE           ((RIOT_FLASH_SIZE) * 1024 * 1024)
#endif

#ifdef __cplusplus
}
#endif

#endif /* !DOXYGEN */
/** @} */
#endif /* SDKCONFIG_H */
