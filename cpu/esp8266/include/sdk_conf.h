/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       SDK configuration compatible to the ESP-IDF
 *
 * This file defines configuration parameters that are only required for source
 * code compatibility with the SDK. These configuration parameters are not used
 * directly to configure the compilation of RIOT-OS. However, some of them can
 * be overrien overridden by application-specific board configuration.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */


#ifndef DOXYGEN

#include "board.h"
#include "esp_image_format.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   SDK version number
 *
 * Determined with `git describe --tags` in `$ESP8266_SDK_DIR`
 */
#if !defined(IDF_VER) || DOXYGEN
#include "esp8266_idf_version.h"
#endif

/**
 * @name Default console configuration
 *
 * STDIO_UART_BAUDRATE is used as CONFIG_CONSOLE_UART_BAUDRATE and
 * can be overridden by an application specific configuration.
 *
 * @{
 */
#define CONFIG_CONSOLE_UART_NUM         (0)
#ifndef CONFIG_CONSOLE_UART_BAUDRATE
#define CONFIG_CONSOLE_UART_BAUDRATE    (STDIO_UART_BAUDRATE)
#endif
/** @} */

#define CONFIG_APP1_SIZE                (0xf0000)
#define CONFIG_APP1_OFFSET              (0x10000)

#define CONFIG_SOC_IRAM_SIZE            (0xc000)

#define CONFIG_TASK_WDT_PANIC
#define CONFIG_TASK_WDT_TIMEOUT_S       (15)
#define CONFIG_RESET_REASON             (1)

#define CONFIG_WIFI_PPT_TASKSTACK_SIZE  (3584)
#define CONFIG_MAIN_TASK_STACK_SIZE     (2048)
#define CONFIG_EVENT_LOOP_STACK_SIZE    (2048)

#define CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE
#define CONFIG_ESP_PHY_INIT_DATA_IN_PARTITION       (0)

#define CONFIG_SPI_FLASH_FREQ           (ESP_IMAGE_SPI_SPEED_40M) /* 40 MHz */
#define CONFIG_SPI_FLASH_MODE           (ESP_IMAGE_SPI_MODE_DIO)  /* DIO mode */
#define CONFIG_SPI_FLASH_SIZE           (0x100000)

#define CONFIG_SCAN_AP_MAX              (32)

#define CONFIG_USING_NEW_ETS_VPRINTF

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
/** @} */
